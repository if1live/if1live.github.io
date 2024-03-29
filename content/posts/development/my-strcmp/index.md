---
title: 사람 낚는 strcmp()의 리턴타입 재설계하기
subtitle: 연산자 오버로딩의 입문
tags: [strcmp]
slug: my-strcmp
author: if1live
date: 2015-04-21
url: /posts/my-strcmp
---
## strcmp()로 낚시하기

"좋은 함수 함수의 예를 쓰시오."
이것은 어려운 질문이다.
사람마다 좋다고 생각하는 기준이 다르기 때문에 좋은 함수의 기준이 달라진다.
반대로 후진 함수를 쓰라는건 어떨까?
모든 사람이 공감할수 있는 후진 함수가 존재하지 않을까?
이런 생각을 하다 [strcmp()로 간단한 코드][problem_twitter]를 작성했다.

```cpp
#include <string.h>
#include <stdio.h>

int main() {
  if(strcmp("foo", "foo") == true) {
    printf("same\n");
  } else {
    printf("different\n");
  }
  return 0;
}
```

출력 결과가 무엇일까? `different`이다.
strcmp 함수는 두 문자열이 같을때 0을 반환한다.
0과 true를 비교하면 false이므로 different가 출력된다.
하나씩 해석하면 쉬운 코드다. 하지만 이런 코드에는 원래 개발하던 사람도 낚인다.
원래 개발하던 사람도 낚이는 코드를 작성할 수 있다면 이 함수는 나쁜 함수 아닐까?

* [@kindtis님의 반응][res_1_twitter]
* [@Le_Ig_18님의 반응][res_2_twitter]

## strcmp()

strcmp()를 [자세히][strcmp_cpp] 보자.

`int strcmp ( const char * str1, const char * str2 );`

* <0 : the first character that does not match has a lower value in ptr1 than in ptr2
* 0 : the contents of both strings are equal
* \>0 : the first character that does not match has a greater value in ptr1 than in ptr2

두 문자열이 같으면 0을 반환하고 다르면 0보다 작거나 큰 값을 반환한다.
두 문자열의 대소를 비교할수 있기 때문에 정렬에 사용할수 있다.

## 개선하기
두 문자열이 같을때 함수의 리턴이 0이다. 두 문자열이 같을때 진입하는 if문은 다음과 같이 작성해야 한다.

```c
if(strcmp(foo, bar) == 0) { ... }
```

실수로 다음과 같이 작성하면 의도와는 다르게 동작한다.
```c
if(strcmp(foo, bar) == true) { ... }
if(strcmp(foo, bar)) { ... }
```

만약 3가지 if문이 동일한 동작을 하도록 my_strcmp() 라는 함수를 만들수 있으면 실수를 줄일수 있지 않을까?

```c
// 기존 strcmp와의 호환성 유지
if(my_strcmp(foo, bar) == 0) { ... }

// 바꿔치기 해야되는 동작
if(my_strcmp(foo, bar) == true) { ... }
if(my_strcmp(foo, bar)) { ... }
```

* 두 문자열이 같은 경우
    * retval == 0 : true
    * retval == true : true
    * retval : true
* 리턴타입은 기존의 strcmp()와의 호환성을 유지해야한다.

이런 동작을 하는 my_strcmp()와 리턴타입을 만들어보자.


## obj-c

obj-c에서 strcmp()에 대응되는 함수의 리턴값은 enum이다.
각각의 enum은 -1, 0, 1에 대응하기 때문에 strcmp()와의 호환성을 유지한다.
strcmp의 리턴값을 단순한 int가 아니라 enum으로 바꾸면 개선할수 있지 않을까?

```c
typedef enum {
  NSOrderedAscending = -1,
  NSOrderSame,
  NSOrderedDescending
} NSComparisonResult;
```

## my_strcmp : enum version

enum과 연산자 오버로딩을 이용해서 my_strcmp()의 대안 함수인 my_strcmp_enum()을 구현하면 다음과 같다.

```cpp
#include <cstdio>
#include <cstring>
#include <cassert>

typedef enum {
  MyOrderedAscending = -1,
  MyOrderSame,
  MyOrderedDescending
} MyComparisonResult;

MyComparisonResult my_strcmp_enum(const char * str1, const char * str2) {
  int val = strcmp(str1, str2);
  if(val < 0) {
    return MyOrderedAscending;
  } else if(val > 0) {
    return MyOrderedDescending;
  } else {
    return MyOrderSame;
  }
}

bool operator==(MyComparisonResult a, int b) {
  return ((int)a == b);
}

bool operator==(MyComparisonResult a, bool b) {
  return !((bool)a == b);
}

bool operator!=(MyComparisonResult a, int b) { return !(a == b); }
bool operator!=(MyComparisonResult a, bool b) { return !(a == b); }

// compile error
// error: conversion function must be a non-static member function
// operator bool(MyComparisonResult a) { return true; }

int main()
{
  assert(my_strcmp_enum("foo", "foo") == 0);
  assert(my_strcmp_enum("foo", "foo") == true);

  assert(my_strcmp_enum("foo", "bar") != 0);
  assert(my_strcmp_enum("foo", "bar") != true);

  // compatible with strcmp
  assert(my_strcmp_enum("1", "1") == strcmp("1", "1"));
  assert(my_strcmp_enum("1", "2") == strcmp("1", "2"));
  assert(my_strcmp_enum("2", "1") == strcmp("2", "1"));

  // if(my_strcmp...)
  printf("same : %d\n", my_strcmp_enum("foo", "foo"));
  printf("different : %d\n", !my_strcmp_enum("foo", "foo"));
  return 0;
}
```

```bash
$ make enum_version.exe && ./enum_version.exe
make: `enum_version.exe' is up to date.
same : 0
different : 1
```

my_strcmp()의 조건을 대부분 만족한다.

하지만 if문 안에서 my_strcmp를 바로 사용할 경우 의도한 것과 반대로 동작한다.
MyComparisonResult 를 bool 또는 int로 캐스팅하는 연산자를 오버로딩할 수 있으면 가능할거 같지만
전역함수로 conversion function (캐스팅 연산자)를 오버로딩할 수 없다.
아무래도 다른 방법을 찾아야겠다.

> operator bool(MyComparisonResult a)
>
> error: conversion function must be a non-static member function

## my_strcmp : class version
C++에서는 class를 이용해서 새로운 타입을 만들 수 있다.
enum 대신 클래스를 이용해서 구현하자.
클래스의 멤버함수로 conversion function을 만들면 enum 버전에서 구현하지 못한 것도 할 수 있다.

```cpp
#include <cstdio>
#include <cstring>
#include <cassert>

class ComparisonResult {
public:
  enum {
    ASC = -1,
    EQUAL,
    DESC
  };

  ComparisonResult(int val) : val(val) {}

  bool operator==(int x) {
    return ((int)val == x);
  }

  bool operator==(bool x) {
    return !((bool)val == x);
  }

  bool operator!=(int x) { return !(*this == x); }
  bool operator!=(bool x) { return !(*this == x); }

  operator bool() {
    return (val == EQUAL);
  }

  static ComparisonResult strcmp(const char *str1, const char *str2) {
    int raw = ::strcmp(str1, str2);
    if(raw < 0) {
      return ComparisonResult(ASC);
    } else if(raw > 0) {
      return ComparisonResult(DESC);
    } else {
      return ComparisonResult(EQUAL);
    }
  }
private:
  int val;
};

ComparisonResult my_strcmp(const char *str1, const char *str2)
{
  return ComparisonResult::strcmp(str1, str2);
}

int main()
{
  assert(my_strcmp("foo", "foo") == 0);
  assert(my_strcmp("foo", "foo") == true);
  assert(my_strcmp("foo", "foo"));

  assert(my_strcmp("foo", "bar") != 0);
  assert(my_strcmp("foo", "bar") != true);
  assert(!my_strcmp("foo", "bar"));

  // compatible with strcmp
  assert(my_strcmp("1", "1") == strcmp("1", "1"));
  assert(my_strcmp("1", "2") == strcmp("1", "2"));
  assert(my_strcmp("2", "1") == strcmp("2", "1"));
  return 0;
}
```

## 정리
* strcmp() 쓸때 조심하자.
* C++ 연산자 오버로딩을 사용하면 strcmp()와의 호환성을 유지하면서 strcmp()보다 사고 덜치게 생긴 my_strcmp()를 구현할 수 있다.
* 연산자 오버로딩을 이용하면 x와 bool을 비교할때, x와 int를 비교할때 다른 방식으로 처리하는게 가능하다.
* 캐스팅 연산자는 전역함수로 오버로딩 불가능하다.
* 새로운 타입을 만들때는 enum보다 class를 사용하는게 자유롭다.
* 코드는 [Gist][gist]에 올렸다.

## Reference
* [Operators in C and C++][operator_wiki]
* [strcmp][strcmp_cpp]

[strcmp_cpp]: http://www.cplusplus.com/reference/cstring/strcmp/
[operator_wiki]: http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
[gist]: https://gist.github.com/if1live/3e8f24a4b8bdcc15a3e9

[problem_twitter]: https://twitter.com/if1live/status/589972518246617089
[res_1_twitter]: https://twitter.com/kindtis/status/589974383583571968
[res_2_twitter]: https://twitter.com/Le_Ig_18/status/589972814704214016
