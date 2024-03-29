---
title: delete this 이용해서 터미널에 삼각형 그리기
subtitle: delete this + operator delete => recursion
tags: [black magic, destructor, delete, recursion]
slug: recursion-with-destructor
author: if1live
date: 2016-03-19
url: /posts/recursion-with-destructor
---

어제 회사에서 `delete this` 에 대해 이것저것 이야기를 했었다.
그리고 꿈에서 `delete this` 로 재귀를 구현하는 흑마법이 생각나서 짜봤다.

* 목표 : `delete this`를 이용해서 다음과 같은 삼각형을 출력해야한다.
    * `for`, `while` 같은 키워드 사용 금지
    * 명시적인 재귀호출 없음, 단 `delete this`는 허용

```
*
**
***
```

## 기본 구현

`delete`를 쓰면 메모리를 해제하고 소멸자를 호출한다.
그렇다면 소멸자 안에서 `delete this`를 사용하면 자기 자신의 소멸자를 다시 호출할테니 재귀호출이 되는거 아니냐?

```cpp
#include <cstdio>
#include <string>

int from = 0;
int to = 3;
int counter = from;
class DtorRecursion {
public:
  ~DtorRecursion() {
    if(counter < to) {
      line += "*";
      printf("%s\n", line.data());

      counter += 1;
      delete(this);
    }
  }

private:
  std::string line;
};

int main()
{
  { DtorRecursion loop; }
  return 0;
}
```

```bash
$ clang++ simple.cpp
$ ./a.out
*
**
***
a.out(4308,0x7fff77901000) malloc: *** error for object 0x7fff54a28be0: pointer being freed was not allocated
*** set a breakpoint in malloc_error_break to debug
```

이미 해제한 메모리를 다시 해제하는 것으로 인해 문제가 생기는 것을 제외하면 의도한대로 돌아간다.
현재의 코드에는 루프과 끝이 전역변수로 하드코딩 되어있다. 이를 개선해보자.

## 범용적으로 바꿔보자

```cpp
#include <cstdio>
#include <string>
#include <memory>
#include <iostream>

template<int From, int To, typename Runnable>
class DestructorRecursion {
public:
  static const int counter_from = From;
  static const int counter_to = To;

  ~DestructorRecursion() {
    if(_counter < counter_to) {
      _runnable();
      _counter += ((From < To) ? +1 : -1);
      delete this;
    }
  }

private:
  static int _counter;
  static Runnable _runnable;
};

template<int From, int To, typename Runnable>
int DestructorRecursion<From, To, Runnable>::_counter = From;

template<int From, int To, typename Runnable>
Runnable DestructorRecursion<From, To, Runnable>::_runnable;

class TrianglePrinter {
public:
  void operator()() {
    line += "*";
    printf("%s\n", line.data());
  }
private:
  std::string line;
};


int main()
{
  DestructorRecursion<0, 3, TrianglePrinter> foo;
  return 0;
}
```

간단한 구현에서 부족했던 부분을 약간 개선해봤다.

1. 루프의 시작과 끝을 템플릿 인자로 넘길수 있다.
2. 루프에서 어떤 기능을 수행할지를 템플릿 인자로 넘길수 있다.


```bash
$ clang++ destructor_recursion.cpp
$ ./a.out
*
**
***
a.out(4246,0x7fff77901000) malloc: *** error for object 0x7fff52a90bf8: pointer being freed was not allocated
*** set a breakpoint in malloc_error_break to debug
```

하지만 아직 몇가지 문제가 남아있다.

첫번째는 thread-safe 하지 않다는 점이다.
루프의 작업 내용과 카운터의 값이 클래스 정적변수에 저장되어있다.
만약 똑같은 루프 시작값, 끝값, 작업용 클래스를 넘겨서 또다른 루프를 생성하면 값을 덮어쓸 가능성이 있다.

두번째 문제는 이미 해제한 메모리를 다시 해제하는 문제를 해결하지 못했다는 점이다.
`delete this` 대신에 [소멸자를 수동으로 호출]({{< ref call-ctor-dtor >}})한다면 이미 해제된 메모리를 다시 해제하는 문제를 피할수 있지만 명시적인 함수 호출이 되니 피하고 싶다.


## 죽지 않도록 바꿔보자

멤버 변수를 이용하면 클래스 정적 변수를 제거할수 있다.
루프의 시작값, 끝값, 현재 상태, 루프에서 어떤 작업을 수행할지를 멤버 변수에 저장하도록 바꿨다.

`delete this`의 메모리 해제까지 직접 개입하려면 `operator delete`를 수정하면 된다.
소멸자를 여러번 호출해도 메모리가 한번만 해제되도록하려고 `_deleted` 멤버 변수를 추가했다.
디버깅 목적으로 실제로 메모리를 해제할때 메세지를 출력하도록 수정했다.

이미 지워진 객체의 멤버 변수(`_deleted`)에 접근하니까 프로그램이 죽지 않을까?
직전까지 유효했던 주소값에서 bool 타입의 값을 읽어오는 방식이라서 바로 죽진 않더라.
해당 주소값의 페이지가 바로 사라지진 않을테니까 프로그램이 크래시나진 않더라.
(물론 올바른 코드는 아니다)

```cpp
#include <cstdio>
#include <string>
#include <memory>

template<typename Runnable>
class DestructorRecursion {
public:
  typedef DestructorRecursion<Runnable> self_type;
public:
  DestructorRecursion(int from, int to, Runnable &&r)
    : _counter_from(from),
      _counter_to(to),
      _counter(from),
      _runnable(r),
      _deleted(false)
    {}

  ~DestructorRecursion() {
    if(_counter < _counter_to) {
      _runnable();
      _counter += ((_counter_from < _counter_to) ? +1 : -1);
      delete(this);
    }
  }

  static void operator delete(void* ptr) {
    self_type *self = reinterpret_cast<self_type*>(ptr);
    if(!self->_deleted) {
      self->_deleted = true;
      ::operator delete(ptr);
      printf("operator delete -> memory free\n");
    }
  }

private:
  const int _counter_from;
  const int _counter_to;
  int _counter;
  Runnable _runnable;

  bool _deleted;
};


class TrianglePrinter {
public:
  void operator()() {
    line += "*";
    printf("%s\n", line.data());
  }
private:
  std::string line;
};

int main()
{
  auto sp1 = std::make_unique<DestructorRecursion<TrianglePrinter>>(0, 3, TrianglePrinter());;
  return 0;
}
```

```bash
$ clang++ safe_destructor_recursion.cpp -std=c++14 && ./a.out
*
**
***
operator delete -> memory free
```

출력결과를 보니 메모리 해제는 한번만 발생했고 프로그램은 크래시 없이 제대로 종료되었다.
야호! 새로운 루프 구문을 발견했다!

## 이거 써도 되는거야?

`delete this`를 이용해서 루프구문을 만든다는 정신나간 발상을 진짜로 안전한건지 다시 확인해보자.
재귀안에서 멤버변수로 객체를 하나 넣어두었다.
객체의 생성자/소멸자에서 printf를 찍어보고 쌍이 맞는지 확인하자.

```cpp
#include <cstdio>
#include <string>
#include <memory>

template<typename Runnable>
class DestructorRecursion {
public:
  typedef DestructorRecursion<Runnable> self_type;
public:
  DestructorRecursion(int from, int to, Runnable &&r)
    : _counter_from(from),
      _counter_to(to),
      _counter(from),
      _runnable(r),
      _deleted(false)
    {}

  ~DestructorRecursion() {
    if(_counter < _counter_to) {
      _runnable();
      _counter += ((_counter_from < _counter_to) ? +1 : -1);
      delete(this);
    }
    printf("end of recusion destructor: %lx\n", (unsigned long)this);
  }

  static void operator delete(void* ptr) {
    self_type *self = reinterpret_cast<self_type*>(ptr);
    if(!self->_deleted) {
      self->_deleted = true;
      ::operator delete(ptr);
      printf("operator delete -> memory free\n");
    }
  }

private:
  const int _counter_from;
  const int _counter_to;
  int _counter;
  Runnable _runnable;

  bool _deleted;
};


class TrianglePrinter {
public:
  TrianglePrinter(){
    printf("constructor\t: %lx\n", (unsigned long)this);
  }
  TrianglePrinter(const TrianglePrinter &o){
    printf("copy constructor: %lx\n", (unsigned long)this);
  }
  ~TrianglePrinter() {
    printf("destructor\t: %lx\n", (unsigned long)this);
  }
  void operator()() {
    line += "*";
    printf("%s\n", line.data());
  }
private:
  std::string line;
};

int main()
{
  auto sp1 = std::make_unique<DestructorRecursion<TrianglePrinter>>(0, 3, TrianglePrinter());;
  return 0;
}
```

```bash
$ clang++ member_var_dtor_recursion.cpp -std=c++14 && ./a.out
constructor	: 7fff5c817be8
copy constructor: 7ffe99d00010
destructor	: 7fff5c817be8
*
**
***
end of recusion destructor: 7ffe99d00000
destructor	: 7ffe99d00010
operator delete -> memory free
end of recusion destructor: 7ffe99d00000
destructor	: 7ffe99d00010
end of recusion destructor: 7ffe99d00000
destructor	: 7ffe99d00010
end of recusion destructor: 7ffe99d00000
destructor	: 7ffe99d00010
```

첫번째 줄의 생성자는 `DestructorRecursion` 객체를 생성할때 생성된 임시객체이다.
이것은 세번째 줄에서 소멸자가 호출되어서 사라졌다.

두번째 줄은 `DestructorRecursion` 안의 객체에 값을 대입하면서 호출된 복사생성자이다.
그런데 이것의 소멸자는 여러번 호출되었다.
소멸자의 역할중 하나가 멤버 변수의 소멸자를 호출하는 것이기 때문이다.
그래서 `~DestructorRecursion()`의 맨 마지막줄의 `end of recusion destructor`가 출력된 다음 `destructor`가 출력된다.

생성자/소멸자의 쌍이 안맞기 때문에 `delete this`를 이용한 재귀구문은 실제 생활에서는 쓰면 안된다.
이 프로그램이 크래시나지 않은건 운이 좋아서일뿐이다.
`operator()()`의 내용이 조금만 바뀌었으면 크래시가 발생할수 있다.
(물론 의도적으로 크래시가 발생하지 않도록 짜는 것도 가능하다.)


## 언제 쓸수 있는가?

바로 위에서 사용해서는 안되는 코드라고 했지만 예외가 존재한다.

[아주 간단한 C++ 퀴즈 하나](https://kldp.org/node/99915)

```
갑자기 옛날에 봤던 문제가 생각나서 올려봅니다.
정말 간단하고 쉬운 문제인데요,
아마 제가 알고있는 답보다 재밌는 답이 많이 나올 수 있을 것 같습니다 ㅎㅎ
보통 처음 C++을 시작하고 for구분을 배우면 짜게되는 대표적인 코드가 삼각형 별표출력코드인데요

*
**
***

뭐 대충 이런식이 되겠지요

문제는..
위 2중 루프를 루프 하나로 줄이는것입니다.
특별한 제한은 없긴한데 루프내부에서 함수호출해서 그함수에서 루프돌거나 실질적 루프인 재귀함수.. 그런건 무효입니다 -_-;
아무튼 자기가 작성하는 코드에 루프가 하나만 들어가면 됩니다.
```

[극악의 c언어 문제](http://www.todayhumor.co.kr/board/view.php?table=programmer&no=12810)

```
-----

  *
 ***
*****
 ***
  *
-----

for문을 이용해서
별을 출력하는 문제인데
주변에 프로그래밍좀 한다는 친구들도
이 문제는 못풀더군요.
과연 오유 여러분의 실력은 어느정도 될런지..
```

* 서로 흑마법을 뽐낼때
* 별것도 아닌 문제로 키배 뜨고있을때
* "C언어로 별표 삼각형 어떻게 그려요?"같은 과제수준의 질문을 봤을때

위와 같은 상황에서 트롤링 목적으로 사용할수 있다.
이 코드를 쓰면 웬만해서는 변태로 인정받을수 있을거다.
