---
title: 디버깅 포스트모템 - 박살난 소멸자
subtitle: LLDB를 이용해서 버퍼 오버플로우 찾기
tags: [lldb, buffer overflow, buffer overrun, memory bug, debug, debugging, destructor, postmortem]
slug: buffer-overflow-broken-dtor-debugging-post-mortem
author: if1live
date: 2016-01-17
url: /posts/buffer-overflow-broken-dtor-debugging-post-mortem
---
개요
----

최근에 레거시 코드에 숨겨진 버퍼 오버플로우 버그를 잡느라고 하루를
날렸습니다. 찾고나니 간단한 버그였지만 하루씩이나 걸렸습니다. 버그를
잡은 다음에 생각하니 여러가지 요소가 결합되어서 디버깅이 오래 걸린거
같았습니다.

* 콜스택이 이상하게 나와서 버퍼 오버플로우 버그인지 감을 잡는데 오래 걸림
* step in, step out, continue, next 이외의 디버거 기능을 안써봤다.
* 말로만 듣고 한번도 안써본 watchpoint 사용법을 찾아보는데 오래 걸림
* gdb만 쓰다가 lldb를 처음 써봄. gdb와 lldb의 명령어는 다르다

버퍼 오버플로우 문제가 발생하는 가상 시나리오를 작성하고 이를
디버깅함으로써 나중에 같은 버그를 만났을때 빠르게 대응하는걸 목표로
디버깅 포스트모템을 작성해보았습니다.

* 버퍼 오버플로우이 발생하는 예제 코드를 작성한다.
* LLDB의 간단한 사용법을 익힌다.
* watchpoint를 이용해서 버퍼 오버플로우를 찾는다.

간단한 스크립트 엔진을 만들자
-----------------------------

간단한 스크립트 엔진을 만들어 봅시다. 스크립트 엔진은 텍스트 파일을
읽을수 있습니다. 스크립트 문법 형태는 `<command> <name>:<text>` 입니다.
모든 명령은 한줄 단위로 처리됩니다.

```txt
say 철수:hello
play 영희:game
??? 민수:dummy text
```

스크립트 엔진과 스크립트 파일을 같이 실행하면 아래와 같은 출력이
나옵니다. 현재는 콘솔로만 출력하지만 GUI, 게임 엔진을 붙여서 확장하면
간단한 텍스트 어드벤쳐는 만들수 있을겁니다.

```
$ clang++ main.cpp -g -W -Wall -std=c++11
$ ./a.out normal.txt
철수 say "hello".
영희 play game.
Unknown: cmd=???, name=민수, text=dummy text.
```

어떤걸 만들어야하는지 알았으니 구현해보았습니다.

코드
----

```cpp
class ScriptEngine {
public:
  ScriptEngine(const char *filename);
  ~ScriptEngine();

  void parseLine(bool *has_next);
  void executeLine();

private:
  void open();
  void close();

private:
  // state
  int curr_pos_;
  char cmd_[8];
  char name_[8];
  char text_[8];

  // raw script content
  std::string filename_;
  const char *data_;
  int length_;
  int fd_;
};
```

스크립트 엔진의 헤더입니다. 생성자로 스크립트 파일 이름을 받습니다.
한줄씩 읽어서 명령을 실행합니다. 읽은 명령을 임시로 저장하기 위해 문자열
배열 `cmd_`, `name_`, `text_` 을 이용합니다. 열어놓은 스크립트 파일을
관리하는 목적의 변수도 멤버 변수로 포함되어 있습니다.

```cpp
int main(int argc, char *argv[0])
{
  if(argc != 2) {
    printf("Usage: %s <script filepath>\n", argv[0]);
    exit(0);
  }

  ScriptEngine script_engine(argv[1]);

  bool has_next = true;
  while(has_next) {
    script_engine.parseLine(&has_next);
    script_engine.executeLine();
  }
  return 0;
}
```

`main()` 입니다. 명령줄 인수로 실행할 스크립트 파일명을 받습니다.
스크립트 엔진은 다음 명령이 없을때까지 한줄씩 읽고 처리합니다.

```cpp
ScriptEngine::ScriptEngine(const char *filename)
  : curr_pos_(0), filename_(filename), data_(nullptr), length_(-1), fd_(-1)
{
  std::fill(cmd_, cmd_ + sizeof(cmd_), 0);
  std::fill(name_, name_ + sizeof(name_), 0);
  std::fill(text_, text_ + sizeof(text_), 0);

  open();
}

ScriptEngine::~ScriptEngine()
{
  close();
}
```

생성자에서는 멤버 변수를 초기화하고 스크립트 파일을 엽니다. 소멸자에서는
스크립트 파일을 닫습니다. 파일을 열고 닫는 상세 과정은 별도의 함수에
구현했습니다.

```cpp
void ScriptEngine::open()
{
  // open
  fd_ = ::open(filename_.data(), O_RDONLY, 0);
  if(fd_ == -1) {
    err(1, "open");
  }

  // get file size
  struct stat sb;
  if(fstat(fd_, &sb) < 0) {
    err(1, "fstat");
  }
  length_ = sb.st_size;

  //memory map
  data_ = (char *)mmap(NULL, length_, PROT_READ, MAP_SHARED, fd_, 0);
  if(data_ == MAP_FAILED) {
    err(1, "mmap");
  }
}

void ScriptEngine::close()
{
  if(fd_ != -1) {
    munmap((void*)data_, length_);
    ::close(fd_);

    fd_ = -1;
  }
}
```

`ScriptEngine::open()`, `ScriptEngine::close()` 는
[mmap](https://en.wikipedia.org/wiki/Mmap) 을 이용해서 구현했습니다.
파일을 열어서 `data_` 에 연결하고 파일의 크기는 `length_` 에
저장해둡니다. 스크립트 파일을 한줄씩 읽어서 처리할때 위의 두 변수를
이용하게 됩니다.

```cpp
void ScriptEngine::parseLine(bool *has_next)
{
```

스크립트 파일로부터 명령을 한줄씩 읽는 함수입니다. 읽은 내용은 `cmd_`,
`name_`, `text_` 에 저장합니다.

```cpp
  // syntax
  // <cmd> <name>:<text>
  enum ParseState {
    STATE_CMD,
    STATE_NAME,
    STATE_TEXT,
    STATE_FINISH,
  };

  struct ParseCommand {
    ParseState curr_state;
    ParseState next_state;
    char delim;
    char *buffer;
    int buffer_size;
  };

  ParseCommand cmds[] = {
    { STATE_CMD, STATE_NAME, ' ', cmd_, sizeof(cmd_) },
    { STATE_NAME, STATE_TEXT, ':', name_, sizeof(name_) },
    { STATE_TEXT, STATE_FINISH, '\n', text_, sizeof(text_) },
  };

  ParseState parse_state = STATE_CMD;
```

FSM을 이용해서 명령줄 파싱을 구현했습니다. 상태는 `STATE_CMD` ->
`STATE_NAME` -> `STATE_TEXT` -> `STATE_FINISH` 로 바뀌면서 각각의
내용을 읽습니다. 특별한 문자열(delim)을 만나면 다음 상태로 전이합니다.
각각의 상태에서 읽은 내용을 저장하기 위한 버퍼의 주소, 크기도 상태에
포함시킵니다.

```cpp
  char buffer[1024];
  int len = 0;

  while((curr_pos_ < length_) && (parse_state != STATE_FINISH)) {
    char ch = data_[curr_pos_++];

    for(const ParseCommand &cmd : cmds) {
      if(cmd.curr_state == parse_state) {
        if(cmd.delim == ch) {
          memset(cmd.buffer, 0, cmd.buffer_size);
          memcpy(cmd.buffer, buffer, len);

          parse_state = cmd.next_state;
          len = 0;
        } else {
          buffer[len++] = ch;
        }
        break;
      }
    }
  }
```

스크립트 파일의 끝이나 `STATE_FINISH` 를 만나기 전까지 한 글자씩
읽습니다. delim을 만나서 상태를 바꿔야되는 상황이 오면 지역 변수
`buffer` 에 저장된 내용을 결과 버퍼에 복사합니다. 이때 `memset()`,
`memcpy()` 를 이용합니다.

```cpp
  *has_next = (curr_pos_ < length_);
}
```

더이상 파싱할수 없으면 스크립트가 끝났다는걸 알려줍니다.

```cpp
void ScriptEngine::executeLine()
{
  // add complex feature here
  if(std::string("play") == cmd_) {
    printf("%s %s %s.\n", name_, cmd_, text_);

  } else if(std::string("say") == cmd_) {
    printf("%s %s \"%s\".\n", name_, cmd_, text_);

  } else {
    printf("Unknown: cmd=%s, name=%s, text=%s.\n", cmd_, name_, text_);
  }
}
```

`parseLine()` 에서 읽어둔 내용을 이용해서 명령을 실행합니다. 스크립트가
어떤 작업을 수행할지는 여기에 적절히 구현합니다.

EPIC FAIL
---------

최초에 예제로 정한 파일에 대해서는 잘 돌아가지만 다음 스크립트 파일
앞에서는 망합니다.

```txt
say 철수:abcdefghijklmnopqrstuvwxyz123456
play 영희:game
```

```
$ clang++ main.cpp -g -W -Wall -std=c++11
$ ./a.out long.txt
철수 say "abcdefghijklmnopqrstuvwxyz123456".
영희 play game.
a.out(6937,0x7f...) malloc: *** error for object 0x36353433: pointer being freed was not allocated
*** set a breakpoint in malloc_error_break to debug
make: *** [run] Abort trap: 6
```

??? new, alloc과 같은 동적할당을 직접 수행한적이 없는데 프로그램은
malloc와 관련된 문제로 죽습니다. 왜 죽었나 디버깅을 합시다.

LLDB와 함께하는 디버깅
----------------------

[LLDB](https://en.wikipedia.org/wiki/LLDB_%28debugger%29) 는 Clang에
붙어있는 디버거입니다. Max OSX에서 xcode를 설치했더니 gdb대신 lldb밖에
없어서 이거로 디버깅해봅니다. (주의: lldb와 gdb는 명령이 다릅니다)

### 어디서 죽었나 찾아내기

콘솔에 찍힌 정보만으로는 어디서 죽었나 잘 모르겠으니 일단 죽여봅시다.
main.cpp 를 `-g` 옵션으로 컴파일해서 디버그 정보를 넣어둡니다. 그리고
lldb를 실행합니다.

```
$ clang++ main.cpp -g -W -Wall -std=c++11
$ lldb a.out
(lldb) target create "a.out"
Current executable set to 'a.out' (x86_64).
```

일단 돌려보고 어디서 죽나 봅시다. process launch 를 이용해서 프로그램을
실행할 수 있습니다. 다음과 같은 형태로 명령줄 인수를 같이 넘길수
있습니다. `process launch -- long.txt`

```
(lldb) process launch -- long.txt
Process 7611 launched: '..../a.out' (x86_64)
철수 say "abcdefghijklmnopqrstuvwxyz123456".
영희 play game.
a.out(7611,0x7f...) malloc: *** error for object 0x36353433: pointer being freed was not allocated
*** set a breakpoint in malloc_error_break to debug
Process 7611 stopped
* thread #1: tid = 0x13ca2, 0x00007fff869280ae libsystem_kernel.dylib`__pthread_kill + 10,...
    frame #0: 0x00007fff869280ae libsystem_kernel.dylib`__pthread_kill + 10
    libsystem_kernel.dylib`__pthread_kill:
->  0x7fff869280ae <+10>: jae    0x7fff869280b8            ; <+20>
    0x7fff869280b0 <+12>: movq   %rax, %rdi
    0x7fff869280b3 <+15>: jmp    0x7fff869233ef            ; cerror_nocancel
    0x7fff869280b8 <+20>: retq
```

죽는걸 확인했으니 어디에서 죽었나 콜스택을 찍어봅시다.
`thread backtrace` 를 이용하면 볼수 있습니다. 줄여서 `bt` 를 쓸수도
있습니다. 대부분의 lldb 명령어는 줄여서 쓰는게 가능합니다.

```
(lldb) bt
* thread #1: tid = 0x13ca2, 0x00007fff869280ae libsystem_kernel.dylib`__pthread_kill + 10,...
  * frame #0: 0x00007f... libsystem_kernel.dylib`__pthread_kill + 10
    frame #1: 0x00007f... libsystem_pthread.dylib`pthread_kill + 90
    frame #2: 0x00007f... libsystem_c.dylib`abort + 129
    frame #3: 0x00007f... libsystem_malloc.dylib`free + 425
    frame #4: 0x000000... a.out`ScriptEngine::~ScriptEngine(this=0x00007f...) + 47 at main.cpp:72
    frame #5: 0x000000... a.out`ScriptEngine::~ScriptEngine(this=0x00007f...) + 21 at main.cpp:70
    frame #6: 0x000000... a.out`main(argc=2, argv=0x00007fff5fbff1c8) + 197 at main.cpp:55
    frame #7: 0x00007f... libdyld.dylib`start + 1
    frame #8: 0x00007f... libdyld.dylib`start + 1
(lldb)
```

콜스택을 찍어보니 frame #4에 `ScriptEngine::~ScriptEngine()` 가
들어있습니다. 소멸자 안에서 프로그램이 죽었습니다.

???

### 소멸자에서 죽는다?

소멸자에서 죽는 경우는 그렇게 많지 않습니다. 어디에선가 vtable (가상
함수 테이블)을 건드려서 죽는 경우도 있습니다만 예제 코드에는 virtual이
없기 때문에 vtable 로 인한 문제는 없을겁니다.

그렇다면 어떤 문제가 남아있을까요? 죽은 지점을 조금더 자세히 봅시다.
읽을 수 있는 코드 중에서 `main.cpp:72` 가 가장 위에 있습니다. 72번째 줄
코드의 내용은 소멸자의 `}` 입니다. 소멸자의 가장 마지막에서 문제가
생긴것으로 보입니다. 객체가 소멸될때 멤버 변수를 정리하는 것도 소멸자의
역할중 하나입니다. 이 과정중에 문제가 생긴거 아닐까요?

### 소멸자에서 죽는 지점 분석

ScriptEngine에서 객체로 된 멤버 변수가 `filename_` 하나뿐입니다.
나머지는 멤버 변수는 primitive data type이기 때문에 소멸자로 문제가 생길
여지가 없습니다.

죽은 시점의 `filename_` 의 값을 확인해봅시다. bt로 보았을때 현재 스택
프레임은 0입니다. `filename_` 로 접근하기 위해서 4번 스택 프레임으로
바꾸고 값을 확인해봅시다. `frame select 4` 로 프레임을 선택할수 있고
`p filename_` 로 변수를 볼 수 있습니다.

```
(lldb) frame select 4
frame #4: 0x000000010... a.out`ScriptEngine::~ScriptEngine(this=0x00007f...) + 47 at main.cpp:72
   69   ScriptEngine::~ScriptEngine()
   70  {
   71     close();
-> 72   }
   73
   74   void ScriptEngine::open()
   75   {
(lldb) p filename_
(std::__1::string) $0 = ""
```

???

`filename_` 의 값은 생성자에서 스크립트 파일 이름을 저장한 이후
건드린적이 없는데 바뀌어있습니다. 코드에서도 `filename_` 를 건드린적은
없습니다. 버퍼 오버플로우(buffer overflow), 버퍼 오버런(buffer
overrun)이 발생해서 의도하지 않게 `filename_` 가 바뀐 것으로 보입니다.
문제의 원인은 찾은거 같으니 어디에서 누가 버퍼 오버플로우를 일으켰는지
확인해봅시다.

### watchpoint 이용해서 버퍼 오버플로우 찾기

`filename_` 의 값이 바뀌는 순간을 찾을 수 있으면 버퍼 오버플로우가
발생한 시점을 알 수 있을겁니다. watchpoint 명령을 이용하면 특정 주소값을
감시하고 있다 값이 바뀌는 상황에 브레이크 포인트가 걸립니다.

생성자에 브레이크 포인트를 걸어두고 프로그램을 다시 실행합니다. 브레이크
포인트는 `b main.cpp:62` 로 걸수 있습니다. 예제에서는 파일이 하나뿐이기
때문에 `b 62` 로도 가능합니다.

```
(lldb) b 62
Breakpoint 1: where = a.out`ScriptEngine::ScriptEngine(char const*) + 275 at main.cpp:62,...
(lldb) process launch -- long.txt
There is a running process, kill it and restart?: [Y/n] y
Process 7611 exited with status = 9 (0x00000009)
Process 7625 launched: '.../a.out' (x86_64)
Process 7625 stopped
* thread #1: tid = 0x.... a.out`ScriptEngine::ScriptEngine(...) + 275 at main.cpp:62..
    frame #0: 0x000... a.out`ScriptEngine::ScriptEngine(...) + 275 at main.cpp:62
   59   ScriptEngine::ScriptEngine(const char *filename)
   60     : curr_pos_(0), filename_(filename), data_(nullptr), length_(-1), fd_(-1)
   61   {
-> 62     std::fill(cmd_, cmd_ + sizeof(cmd_), 0);
   63     std::fill(name_, name_ + sizeof(name_), 0);
   64     std::fill(text_, text_ + sizeof(text_), 0);
   65
(lldb)
```

lldb에서는 `watchpoint set variable this->length_` 와 같은 방식으로 멤버
변수에 watchpoint를 걸수 있습니다.

```
(lldb) watchpoint set variable this->length_
Watchpoint created: Watchpoint 1: addr = 0x7f... size = 4 state = enabled type = w
    watchpoint spec = 'this->length_'
    new value: -1
```

그렇다면 `watchpoint set variable this->filename_` 로 `filename_` 에
watchpoint를 걸면 되겠군요!

```
(lldb) watchpoint set variable this->filename_
error: Watchpoint creation failed (addr=0x7f..., size=24, variable expression='this->filename_').
error: watch size of 24 is not supported
```

는 안됩니다. 그래도 다른 방법으로 watchpoint를 걸수 있습니다. 멤버
변수에 watchpoint를 거는게 안된다면 멤버 변수의 주소에 watchpoint를
걸면됩니다. 주소값으로 watchpoint를 추가할때는 variable대신 expression가
들어갑니다.

```
(lldb) watchpoint set expression -- (void*)(&(this->filename_))
Watchpoint created: Watchpoint 3: addr = 0x7f... size = 8 state = enabled type = w
new value: 0x78742e676e6f6c10
```

watchpoint를 걸었으니 프로그램을 진행시키고 어디에서 메모리 침범했는지
확인해봅시다.

```
(lldb) c
Process 7661 resuming
Process 7661 stopped
* thread #1: tid = 0x175d8, 0x00007fff8e20efb1 libsystem_platform.dylib`_platform_memmove$VARI...
    frame #0: 0x00007fff8e20efb1 libsystem_platform.dylib`_platform_memmove$VARIANT$Haswell + 145
libsystem_platform.dylib`_platform_memmove$VARIANT$Haswell:
->  0x7fff8e20efb1 <+145>: movups %xmm4, (%rdi,%rdx)
    0x7fff8e20efb5 <+149>: popq   %rbp
    0x7fff8e20efb6 <+150>: retq
    0x7fff8e20efb7 <+151>: subq   $0x8, %rdx

Watchpoint 1 hit:
old value: 0x78742e676e6f6c10
new value: 0x78742e67706f6e6d
```

watchpoint가 메모리 침범하는 순간을 잡았습니다. 스택 프레임을 봅시다.

```
(lldb) bt
* thread #1: tid = 0x175d8, 0x00007fff8e20efb1 libsystem_platform.dylib`_platform_memm...
  * frame #0: 0x.. libsystem_platform.dylib`_platform_memmove$VARIANT$Haswell + 145
    frame #1: 0x.. a.out`ScriptEngine::parseLine(this=0x0.., has_next=0x0..) + 495 at main.cpp:143
    frame #2: 0x.. a.out`main(argc=2, argv=0x00007fff5fbff1c8) + 127 at main.cpp:52
    frame #3: 0x.. libdyld.dylib`start + 1
    frame #4: 0x.. libdyld.dylib`start + 1
```

frame #1, `ScriptEngine::parseLine()`, 143번째 줄이 사고쳤습니다.
이지점에 뭐가 있을까요? `l 143` 을 이용해서 코드를 볼 수 있습니다.

```
(lldb) l 143
143            memcpy(cmd.buffer, buffer, len);
...
```

memcpy가 문제를 일으켰습니다.

### 정확히 어떤 상황에서 문제가 발생했는가?

어떤값이 memcpy로 들어갔을때 버퍼 오버플로우를 일으켰을까요? memcpy가
호출되기 직전에 브레이크 포인트를 걸어두고 `buffer` 에 따라서
`filename_` 가 어떻게 바뀌나 확인해봅시다.

```
(lldb) b 143
Breakpoint 3: where = a.out`ScriptEngine::parseLine(bool*) + 462 at main.cpp:143, ...
....
(lldb) c
...
   142            memset(cmd.buffer, 0, cmd.buffer_size);
-> 143            memcpy(cmd.buffer, buffer, len);
...
(lldb) p buffer
(char [1024]) $5 = "abcdefghijklmnopqrstuvwxyz123456"
(lldb) p this->filename_
(std::__1::string) $6 = "long.txt"
(lldb) n
...
-> 145            parse_state = cmd.next_state;
...
(lldb) p this->filename_
(std::__1::string) $7 = ""
```

long.txt의 첫줄의 text에 해당하는 `abcdefghijklmnopqrstuvwxyz123456` 를
`text_` 로 복사할때 버퍼 오버플로우가 발생했습니다. 왜 문제가 되는지
코드를 읽어봅시다.

```cpp
  char text_[8];

  // raw script content
  std::string filename_;
```

`text_` 의 크기는 8입니다. 하지만 복사하려는 text의 크기는 이보다 훨씬
큽니다. 그래서 `text_` 의 다음에 있는 멤버변수 `filename_` 의 내용을
덮어써버렸습니다.

해결책/회피
-----------

버퍼 오버플로우, 버퍼 오버런은 많이 알려진 버그인 동시에 찾기 어려운
버그입니다. 이를 방지하는 방법은 많이 알려져있기 때문에 여기에서는 직접
다루지 않습니다.

* [Buffer overflow :: Protective countermeasures](https://en.wikipedia.org/wiki/Buffer_overflow#Protective_countermeasures)
* [버퍼 오버런 해결!](https://www.microsoft.com/korea/msdn/columns/contents/CodeSecure/secure05202002/default.aspx)
* [Avoiding Buffer Overruns](https://msdn.microsoft.com/ko-kr/library/windows/desktop/ms717795%28v=vs.85%29.aspx)
* [memcpy, memmove, and memset are obsolete!](http://maintainablecode.logdown.com/posts/159916-memcpy-memmove-and-memset-are-deprecated)
