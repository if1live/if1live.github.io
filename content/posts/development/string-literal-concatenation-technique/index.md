---
title: '"foo" "bar" == "foobar"'
subtitle: String literal concatenation을 이용한 기법
tags: [string literal]
slug: string-literal-concatenation-technique
author: if1live
date: 2015-04-20
url: /posts/string-literal-concatenation-technique
---
## 개요

```python
print("foo" "bar" == "foobar")
```

```bash
$ python intro.py
True
```

C에는 문자열과 문자열 사이에 공백문자가 존재할때 두 문자열을 붙여서 하나의 문자열로 취급하는 문법이 있다.
이것을 [String literal concatenation][wiki]라고 부른다.
해당 문법은 C뿐만 아니라 C++, Python 등 몇가지 언어도 존재한다.
이 글에서 직접 사용해본 쓸모있는 String literal concatenation 기법을 정리해보았다.

## 빛
### URL Generate

당신이 String literal concatenation을 모른다고 가정하자.
이 상태에서 아래의 조건을 만족하는 코드를 작성해보자.

* 디버그/릴리즈에 환경에 따라 Hostname이 다르다
* 여러 가지 URL이 존재한다
* URL은 바뀔 필요 없다
* 각각의 URL 길이를 알아야 한다

```cpp
#include <stdio.h>
#include <stdlib.h>

#if _DEBUG
  #define HOST "127.0.0.1"
#else
  #define HOST "libsora.so"
#endif


#define URL_A_FORMAT "http://%s/kasugano"
#define URL_B_FORMAT "http://%s/kasugano/sora"

char url_a[1024];
char url_b[1024];

int length_url_a = sprintf(url_a, URL_A_FORMAT, HOST);
int length_url_b = sprintf(url_b, URL_B_FORMAT, HOST);

int main()
{
  printf("%s %d\n", url_a, length_url_a);
  printf("%s %d\n", url_b, length_url_b);
  return 0;
}
```

디버그/릴리즈 환경에 따라 hostname이 다르기 때문에 전처리기를 이용한 분기를 넣었다.
그리고 전처리기 구역 안에서 HOST라는 매크로 상수를 지정했다.
Hostname이 바뀌는것에 맞춰 URL을 찍어내기 위해 sprintf에 넣을 URL 포맷을 매크로 상수로 만들었다.
sprintf를 이용해서 찍어낸 URL의 길이는 sprintf의 리턴값을 사용했다.

제시된 조건은 모두 만족시켰지만 이를 더 개선할 여지가 있지 않을까?

URL은 바뀔 필요가 없다고 했기 때문에 상수를 그대로 사용할수 있지 않을까?
sprintf()를 호출하지 않아서 컴파일 타임에서 모든걸 끝낼수 있지 않을까?
C로는 컴파일이 안되던데 C언어에서도 돌아가도록 코드를 바꿀수 있지 않을까?
(C로 컴파일 하면 "error: initializer element is not constant" 에러가 발생한다.
C에서는 전역변수의 값을 함수의 리턴값으로 할당할 수 없다.)

String literal concatenation을 이용하면 개선할 수 있다.

```cpp
#include <stdio.h>
#include <stdlib.h>

#if _DEBUG
  #define HOST "127.0.0.1"
#else
  #define HOST "libsora.so"
#endif


#define URL_A "http://" HOST "/kasugano"
#define URL_B "http://" HOST "/kasugano/sora"

// -1 => last '\0'
#define LENGTH_URL_A (sizeof(URL_A) - 1)
#define LENGTH_URL_B (sizeof(URL_B) - 1)

int main()
{
  printf("%s %d\n", URL_A, LENGTH_URL_A);
  printf("%s %d\n", URL_B, LENGTH_URL_B);
  return 0;
}
```

`#define URL_B "http://" HOST "/kasugano/sora"` 와 같은 식으로 매크로를 만들면 간단한 템플릿 엔진처럼 사용할 수 있다.

### Pretty Multiline Printf

printf()를 이용해서 여러줄로 구성된 문자열을 출력하고 싶다.
printf()를 각각의 줄마다 호출할 수도 있지만 printf()를 한번만 쓴다고 가정하자.
String literal concatenation을 모르면 이런식으로 짤 것이다.

```c
#include <stdio.h>
int main()
{
  // 한줄에 출력할 내용을 전부 넣기
  printf("== Help ==\n* option 1\n* option 2\n");

  // 변수에 문자열을 넣은 다음에 출력
  char msg[] = \
    "== Help ==\n"\
    "* option 1\n"\
    "* option 2\n";
  printf("%s", msg);
  return 0;
}
```

한줄로 쓰면 문자열이 뭔지 파악하기 어렵고 그렇다고 여러줄로 쪼개면 매 줄의 끝에 `\`를 추가하는게 귀찮다.
이럴때 String literal concatenation을 사용하면 좋다.

```c
#include <stdio.h>
int main()
{
  printf("== Help ==\n"
         "* option 1\n"
         "* option 2\n");
  return 0;
}
```

### Formatting

콘솔 프로그램을 작성하면 출력 결과를 예쁘게 만드는게 일이다.
다음과 같은 간단한 표를 출력한다고 가정하자.

| name | score | code |
|------|-------|------|
| foo  |  24.5 |  10  |
| bar  | 12.4  | 5    |
| spam | 89.2  |  8   |

```c
#include <stdio.h>
int main()
{
  printf("%6s%6s%6s\n", "name", "score", "code");

  #define LINE_FORMAT "%6s%6.1f%6d\n"
  printf(LINE_FORMAT, "foo", 24.5, 10);
  printf(LINE_FORMAT, "bar", 12.4, 5);
  printf(LINE_FORMAT, "spam", 89.2, 8);
  return 0;
}
```

```bash
$ make format.exe && ./format.exe
make: `format.exe' is up to date.
  name score  code
   foo  24.5    10
   bar  12.4     5
  spam  89.2     8
```

`%6s%6.1f%6d\n` 같은것은 읽기 어렵다.
테이블의 필드가 많아지면 몇번째 필드를 파악하기 어려워진다.
String literal concatenation을 사용하면 각각의 포맷팅을 문자열로 분리하는게 가능하다.

```c
#include <stdio.h>
int main()
{
  #define NAME_COLUMN_FMT "%6s"
  #define NAME_VAL_FMT "%6s"
  #define SCORE_COLUMN_FMT "%6s"
  #define SCORE_VAL_FMT "%6.1f"
  #define CODE_COLUMN_FMT "%6s"
  #define CODE_VAL_FMT "%6d"

  printf(NAME_COLUMN_FMT
         SCORE_COLUMN_FMT
         CODE_COLUMN_FMT "\n", "name", "score", "code");

  #define LINE_FORMAT NAME_VAL_FMT SCORE_VAL_FMT CODE_VAL_FMT "\n"
  printf(LINE_FORMAT, "foo", 24.5, 10);
  printf(LINE_FORMAT, "bar", 12.4, 5);
  printf(LINE_FORMAT, "spam", 89.2, 8);
  return 0;
}
```

다음은 이전에 과제로 작성했던 코드이다.
String literal concatenation가 없었다면 어떤 코드가 되었을지 직접 상상해보자.

```c
#define COMMAND_STR_FMT "%16s"
#define COMMAND_VAL_FMT "%16s"
#define PID_STR_FMT "%6s"
#define PID_VAL_FMT "%6d"
#define PPID_STR_FMT "%7s"
#define PPID_VAL_FMT "%7d"
#define STATE_STR_FMT "%10s"
#define STATE_VAL_FMT "%10s"
#define START_TIME_STR_FMT "%11s"
#define START_TIME_VAL_FMT "%11s"
#define TOTAL_TIME_STR_FMT "%11s"
#define TOTAL_TIME_VAL_FMT "%11s"
#define USER_TIME_STR_FMT "%11s"
#define USER_TIME_VAL_FMT "%11s"
#define KERNEL_TIME_STR_FMT "%11s"
#define KERNEL_TIME_VAL_FMT "%11s"
#define VOLUNTARY_STR_FMT "%11s"
#define VOLUNTARY_VAL_FMT "%11ld"
#define INVOLUNTARY_STR_FMT "%13s"
#define INVOLUNTARY_VAL_FMT "%13ld"

void display_process_field(struct seq_file *m)
{
  seq_printf(m,
               COMMAND_STR_FMT
               PID_STR_FMT
               PPID_STR_FMT
               STATE_STR_FMT
               START_TIME_STR_FMT
               TOTAL_TIME_STR_FMT
               USER_TIME_STR_FMT
               KERNEL_TIME_STR_FMT
               VOLUNTARY_STR_FMT
               INVOLUNTARY_STR_FMT
               "\n",
               "command",
               "pid",
               "ppid",
               "state",
               "start(s)",
               "total(s)",
               "user(s)",
               "kernel(s)",
               "voluntary",
               "involuntary");
}
```

## 그림자
### Multiline String + Copy and Paste
String literal concatenation이 뭔지 모르는 사람이라도 의도하지 않게 사용해본 적이 있을거다.
아마도 여려줄의 문자열을 복붙해서 배열로 만드는 과정일거다.

```python
data = [
    "foo"
    "bar",
    "spam"
]
print(data)
```

```bash
$ python copy_and_paste_trap.py
['foobar', 'spam']
```

의도한 출력은 `['foo', 'bar', 'spam']` 겠지만 comma 하나를 뺴먹어서 String literal concatenation가 발동했다.

## Reference
* [String literal][wiki]

[python_doc]: https://docs.python.org/2/reference/lexical_analysis.html
[wiki]: http://en.wikipedia.org/wiki/String_literal
