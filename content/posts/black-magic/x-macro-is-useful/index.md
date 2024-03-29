---
title: 'int main() { #include "data.txt" return 0; }'
subtitle: 다시 보는 include와 define, 그리고 X-Macro
tags: [black magic, x-macro, define, include, c, preprocessor]
slug: x-macro-is-useful
author: if1live
date: 2017-01-30
url: /posts/x-macro-is-useful
---

> 흑마법은 많이 짜봤지만
> 
> int foo() {
>   #include "data.txt"
> }
> 
> 를 이길 물건은 아직 짜지 못했다. 내 명작 흑마법이었는데 리펙토링 하려다 c++ 접어서 방치

트위터에서 남들의 흑마법 코드를 보다가 5년전에 짠 흑마법 코드가 생각나서 글을 써본다.
C, C++ 많이 해본 사람은 다 알고있을 뻔한 주제다.
하지만 처음하는 사람은 글을 읽고 include, define에 대한 새로운 관점을 배울 수 있을거다.

## 다시 보는 include

C컴파일러는 하나의 소스만 읽고 컴파일할 수 있다.
근데 우리는 stdio.h 같은 파일을 main.c에서 쓰잖아?
C전처리기가 `#include <stdio.h>`를 찾아서 stdio.h의 내용을 main.c에 복사-붙여넣기해서 하나의 파일로 만든후, 이를 컴파일러에게 전달하기때문에 컴파일이 되는거다.
즉, include의 역할은 **복붙**이다.

## 다시 보는 define

C 전처리기의 명령어 중에는 define이 있다.
초급 C 학습서에서는 define을 매크로 상수, 매크로 함수를 만들때 사용한다고 설명한다.
이 설명은 define로 가능한것을 설명한거지 define의 진짜 기능을 말한게 아니다.
define의 진짜 기능은 **치환**이다.

그래서 아래와 같은 코드가 가능하다.
cocos2d-x에 있는 코드인데 네임스페이스 열고 닫는 코드를 직접 쓰지 않고 매크로로 만들어서 사용한다.

```cpp
#ifdef __cplusplus
    #define NS_CC_BEGIN  namespace cocos2d {
    #define NS_CC_END    }
    #define USING_NS_CC  using namespace cocos2d
    #define NS_CC        ::cocos2d
#else
    #define NS_CC_BEGIN 
    #define NS_CC_END 
    #define USING_NS_CC 
    #define NS_CC
#endif 
```

## X-Macro
[X-Macro][wiki_x_macro]는 매크로로 리스트를 만든후 다양한 매크로를 적용해서 코드를 찍어내는 기법이다.
이를 이용하면 노가다 코딩을 줄이는게 가능하다.
말로 설명하는것보다 코드를 보는게 이해하기 쉬울거다.
wikipedia의 예제코드를 그대로 가져왔다.

```cpp
#define LIST_OF_VARIABLES \
    X(value1) \
    X(value2) \
    X(value3)

#define X(name) int name;
LIST_OF_VARIABLES
#undef X

void print_variables()
{
#define X(name) printf("%s = %d\n", #name, name);
LIST_OF_VARIABLES
#undef X
}
```

```cpp
int value1;
int value2;
int value3;

void print_variables()
{
    printf("%s = %d\n", "value1", value1);
    printf("%s = %d\n", "value2", value2);
    printf("%s = %d\n", "value3", value3);
}
```

## 예제

간단한 예제 코드를 만들어봤다. 필요한 기능은 다음과 같다.

* 클래스에는 month, day 멤버 변수가 있어야한다.
* 빈 객체를 생성하면 1, 1로 초기화 되어야한다.
* month, day를 1, 1로 초기화하는 `reset_date()` 함수가 필요하다
* 멤버변수의 값을 출력하는 `print_date()` 함수가 필요하다.

```cpp
#include <cstdio>

struct DateSimple {
    int month = 1;
    int day = 1;
};

void reset_date(DateSimple &obj) {
    obj.month = 1;
    obj.day = 1;
}

void print_date(DateSimple &obj) {
    printf("M: %d\n", obj.month);
    printf("D: %d\n", obj.day);
}

int main() {
    DateSimple d = {2, 5};
    print_date(d);

    reset_date(d);
    print_date(d);
    return 0;
}
```

```bash
$ g++ -o main_simple.exe main_simple.cpp -std=c++14 && ./main_simple.exe
M: 2
D: 5
M: 1
D: 1
```

코드를 잘 보면 중복된 부분이 있다.

* 클래스의 기본값인 1,1이 클래스 안에도 있지만 reset_date에도 있다.
* month, day 변수는 총 3번 등장한다. 클래스, reset_date, print_date

예제는 변수가 고작 2개밖에 없으니까 중복이 심각한 문제는 아니다.
하지만 변수의 갯수가 많아지만 이런 사소한 중복도 유지보수를 귀찮게 만든다.

여기에 X-Macro를 끼얹어보자.

일단은 매크로로 간단한 메타 언어를 만들자.
그리고 메타 언어를 이용해서 클래스를 정의해보자.
이런 느낌이면 될거다.

```txt
STRUCT_BEGIN(Date)
    STRUCT_INT(month, 1, "M: ")
    STRUCT_INT(day, 1, "D: ")
STRUCT_END()
```

코드 중복이 등장하던 부분에 X-Macro를 끼워넣자.

```cpp
#include <cstdio>

#undef STRUCT_BEGIN
#undef STRUCT_INT
#undef STRUCT_END

#define STRUCT_BEGIN(NAME) struct NAME {
#define STRUCT_INT(NAME, VALUE, LABEL) int NAME = VALUE;
#define STRUCT_END() };

#include "struct_data.txt"

void reset_date(Date &obj) {
    #undef STRUCT_BEGIN
    #undef STRUCT_INT
    #undef STRUCT_END

    #define STRUCT_BEGIN(NAME)
    #define STRUCT_INT(NAME, VALUE, LABEL) obj.NAME = VALUE;
    #define STRUCT_END()

    #include "struct_data.txt"
}


void print_date(Date &obj) {
    #undef STRUCT_BEGIN
    #undef STRUCT_INT
    #undef STRUCT_END

    #define STRUCT_BEGIN(NAME)
    #define STRUCT_INT(NAME, VALUE, LABEL) printf(LABEL "%d\n", obj.NAME);
    #define STRUCT_END()

    #include "struct_data.txt"
}

int main() {
    Date d = {2, 5};
    print_date(d);

    reset_date(d);
    print_date(d);
    return 0;
}
```

```bash
$ g++ -o main_xmcaro.exe main_xmacro.cpp -std=c++14 && ./main_xmcaro.exe
M: 2
D: 5
M: 1
D: 1
```

이제 클래스에 멤버변수가 수십개가 되어도 중복으로 고칠 부분은 없다.
메타 언어로 선언된 클래스만 고치면 된다.

## 응용

* 매크로를 잘 이용하면 메타 언어를 만들 수 있을거같은데?
* 매크로를 교체하면 여러 종류의 코드를 찍어낼 수 있을거같은데?

이를 이용해서 5년전에 재밌는 코드를 짠적이 있다.

* 매크로로 표현 가능한 메타 언어를 설계. 이 언어로는 구조체를 표현할수 있다.
* 매크로를 잘 이용해서 구조체 코드를 찍어낸다.
* 매크로를 잘 이용해서 RTTI(Run-Time Type Information)를 구현한다.
* RTTI 정보를 잘 이용해서 구조체를 XML로 직렬화, 역직렬화한다.

[hikaru](https://github.com/if1live/hikaru) : Simple C++ RTTI library

옛날옛적 코드에서 매크로를 이용한 부분을 뜯어낸 라이브러리이다.
X-Macro 가 어떤 물건인지 복습하면서 돌아가도록 만들었다.
(5년 묵은 코드 살려둔거라서 쓰는걸 권장하진 않는다)
대강 이런 느낌으로 쓴다.

매크로 위에서 돌아갈수 있는 메타 언어를 정의한다.
그리고 이 메타 언어를 이용해서 이용해서 클래스의 구조를 명시한다.
클래스 이름은 HKDemoStruct, 버전은 1이다.
(같은 이름의 버전이 다른 클래스의 경우 migration 구현하는게 가능하도록 설계)
이 클래스에는 float 타입의 변수 `float_val`과 int 타입의 변수 `int_val`이 포함된다.

```cpp
HK_STRUCT_BEGIN(HKDemoStruct, 1)
    HK_STRUCT_VAR_FLOAT(float_val)
    HK_STRUCT_VAR_INT(int_val)
HK_STRUCT_END()
```

선언용 매크로를 이용해서 메타 언어를 클래스를 만든다.

```cpp
#define HK_STRUCT_BEGIN(BASE, VERSION)  \
    struct BASE##_##VERSION : public hikaru::BaseStruct {   \
    ....
#define HK_STRUCT_VAR_INT(NAME) int NAME;
#define HK_STRUCT_VAR_FLOAT(NAME) float NAME;
#define HK_STRUCT_END() };
```

정의용 매크로를 이용해서 메타 언어로 만들어진 클래스 정보를 RTTI 시스템에 등록한다.

```cpp
#define HK_STRUCT_BEGIN(BASE, VERSION)  {   \
    typedef BASE##_##VERSION cls;   \
    hikaru::ClassAttribute class_attr(#BASE, #BASE "_" #VERSION, VERSION);
#define HK_STRUCT_VAR_INT(NAME) \
    class_attr.add(hikaru::FieldAttribute::createInt(#NAME, hikaru::offsetOf(&cls::NAME)));
#define HK_STRUCT_VAR_FLOAT(NAME)   \
    class_attr.add(hikaru::FieldAttribute::createFloat(#NAME, hikaru::offsetOf(&cls::NAME)));
#define HK_STRUCT_END() \
    hikaru::ClassDict::getInstance().add(class_attr);   \
}
```

선언용 매크로와 메타 언어를 인클루드해서 클래스를 선언한다.
정의용 매크로와 메타 언어를 인클루드해서 클래스를 RTTI 시스템에 등록하는 함수를 만든다.
그러면 클래스의 멤버변수 정보에 접근할 수 있다.
여기에서 함수안에 include 집어넣는 흑마법 코드가 등장한다.

```cpp

#include "hikaru.h"
#include "hk_declaration.h"
#include "struct_data.txt"

int init_hikaru() {
    #include "hk_definition.h"
    #include "struct_data.txt"
    return 0;
}
static int initialized_hikaru = init_hikaru();

void demo_rtti() {
    auto dict = hikaru::ClassDict::getInstance();
    auto classname = dict.getClassName("HKDemoStruct", 1);
    auto classattr = dict.get(classname);

    printf("class name : %s\n", classattr->class_name().data());
    printf("base name : %s\n", classattr->base_name().data());
    printf("class version : %d\n", classattr->version());
    for (auto it = classattr->begin(); it != classattr->end(); it++) {
        printf("field : name=%s type=%d offset=%d\n", it->name().data(), it->type(), it->offset());
    }
}
```

## 단점

이렇게 좋은 기술이 있는데 많이 쓰이지 않는건 이유가 있다.
매크로는 컴파일러의 통제범위 밖이다. 전처리기의 영역이다.
그래서 잘못 짜면 알아보기 힘든 컴파일 에러가 뜬다.
또한 유지보수가 쉬운 물건도 아니다.
코드 찍어내는 목적으로 매크로를 쓰면 줄끝에 `\`를 넣어야하는데 빼먹기 쉽다.
매크로까지 syntax highlighting이 잘 되는 IDE는 별로 없다.

그래도 단점을 각오하더라도 X-Macro를 쓰면 좋을때가 가끔씩 있다.
RTTI가 그런 사례이다.

## 더 보기

이 글에서는 X-Macro 위주로 다뤘다.
이를 이용해서 RTTI를 어떻게 구현할때는 아래의 글이 도움이 될거다.

* [c++ 에서 구조체 RTTI 정보 남기기](http://lacti.me/2011/09/30/using-rtti-at-cpp/)
* [c++에서 reflection 사용하기 1](http://lacti.me/2012/06/03/using-reflection-at-cpp-1/)

[wiki_x_macro]: https://en.wikipedia.org/wiki/X_Macro
