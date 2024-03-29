---
title: 나는 unsigned가 싫어요
subtitle: Youtube 강남스타일 MV 조회수가 32bit signed integer를 넘어간 시점에 다시 보는 unsigned의 가치
date: 2014-12-04
tags: [unsigned, overflow]
slug: i-hate-unsigned
author: if1live
url: /posts/i-hate-unsigned
---

개요
----

[Youtube, 강남스타일 MV 조회수가 32비트 정수 범위를 넘어섰다고 밝혀](http://www.clien.net/cs2/bbs/board.php?bo_table=news&wr_id=1911757) 를 보니까 댓글에서 "왜 unsigned안쓰고 signed썻냐?" 같은 소리를 하길래 내가 unsigned를 싫어하는 이유를 정리했다.

그림 한장으로 설명하는 내가 unsigned를 싫어하는 이유
----------------------------------------------------

![underflow의 예제](temperature-underflow.jpg)

<https://twitter.com/yun1007com/status/540019367686070272>

왜 나는 unsigned가 싫은가?
--------------------------

계산하기 쉽게 8 bit를 기준으로 했다. 8 bit의 세계에서 signed는 -128~127, unsigned는 0~255까지 표현 가능하다.

### Unsigned - Unsigned

`1 - 2` 를 계산한다고 하자. 0x01과 0x02를 빼면 0xFF가 나온다. 이것은 부호가 있는 세상에서는 -1이지만 부호가 없는 세상에서는 255이다. unsigned에서는 음수를 표현하는 것이 불가능하기 때문이다.

즉, **unsigned간의 뺄셈은 위험하다.**

### Signed + Unsigned

signed와 unsigned를 섞어서 계산하는 로직이 있다고 가정하자. `(signed)(-1) + (unsigned)(255)` 를 계산하자. signed와 unsigned를 같이 계산하면 음수가 발생할 가능성이 있기 때문에 계산 결과는 signed로 취급하는것이 맞을 것이다. `signed = signed + unsigned` 라고 생각하자. 계산 결과가 `-1 + 255 = (signed)(254)` 라고 생각하면 틀렸다. 254는 8-bit signed로는 표현 불가능하다. 8-bit signed로는 127까지밖에 표현하지 못한다. -1과 더해서 signed의 범위에 들어갈수 있는 unsigned는 128 이하의 숫자뿐이다. unsigned의 표현범위가 signed의 2배이지만 실제로 안전하게 쓸 수 있는 범위는 signed랑 별 차이 없다. **범위가 signed의 2배인 unsigned를 썻음에도 불구하고 실제로 사용 가능한 범위가 signed랑 별 차이가 없으면 unsigned를 사용할 필요가 있는가?**

### unsigned >= 0

std::vector의 내용을 뒤에서부터 반복하는 루프를 짯다고 치자. size_t는 unsigned이고 v.size()의 타입은 unsigned이다.

```cpp
std::vector<int> v;
...
for(std::size_t i = v.size() - 1 ; i >= 0 ; i--) { ... }
```

clang++에서 컴파일 하면 다음과 같은 경고가 뜬다. 또한 컴파일된 결과를 실행하면 무한루프에 진입한다.

> warning: comparison of unsigned expression >= 0 is always true:

unsigned를 사용할때 조심하지 않으면 `unsigned >= 0` 같은 코드를 짤 수도 있다. 이것은 항상 참이고 당신이 의도한 대로 코드가 돌아가지 않는다.

### 이 고생을 하고 얻는건 범위 2배

위에서 말했듯이 unsigned는 잘못 사용될 경우 폭탄이다. unsigned를 조심해서 사용하면 2배 큰 범위를 사용할수 있다는건 알겠지만 그렇게까지 해야되는가? **메모리나 CPU도 빵빵한데 32-bit unsigned 쓸 바에 64-bit signed 쓰는게 낫지 않나?** **개발 속도 빠르게 하겠다고 C/C++ 버리고 스크립트 언어 쓰는 세상에 unsigned/signed같은거로 고민할 필요가 있을까?**

그래도 unsigned를 쓰는 상황
---------------------------

### Binary (2진법) / Hexadecimal (16진법)

10진수의 경우는 `-2` 같은 표현을 사용한다. 하지만 `-0x02`, `-b1101` 같은 표현은 아직까지 본 기억이 없다. 2진수나 16진수는 부호없는 세상이라는거 아닐까?

-   Bitmask, Bitwise operation
    -   비트마스크는 on/off 개념이니까 부호가 의미없다

-   Memory Address (Pointer)
    -   포인터가 가리키는 주소값이 음수가 될수 있는가?
    -   32-bit 환경에서 주소를 signed로 표현하고 싶은데 범위가 넘어가서 64-bit signed를 쓰는것이 말이 되는가?
    -   보통 `int*` 같이 쓰니까 볼 일이 없다.

### Type Checking

C++의 std::vector의 size() 의 return type은 `size_t` 이다. <http://www.cplusplus.com/reference/vector/vector/size/> 그리고 size_t는 unsigned 타입이다. 덕분에 간단하게 for 루프를 짜면 워닝을 볼 수 있다.

```cpp
std::vector<int> v;
...
for(int i = 0 ; i < v.size() ; i++) { ... }
```

clang++으로 컴파일 하면 아래와 같이 서로 다른 두 타입을 비교한다고 경고가 발생한다.

> warning: comparison of integers of different signs: 'int' and 'size_type'

경고를 피하려면 std::vector의 루프를 아래의 방법중 하나로 고쳐야한다.

첫번째 방법은 size_t를 이용하는 것이다. 루프에서 사용하는 변수와 벡터의 크기를 unsigned에 맞추는 것이다. 이 경우, 루프용 변수에 뺄셈해서 underflow 발생시키는 만행을 저지르지 않도록 조심해야한다.

```cpp
std::vector<int> v;
...
for(std::size_t i = 0 ; i < v.size() ; i++) { ... }
```

두번째 방법은 vector의 크기를 signed로 바꾸는 것이다. vector의 크기가 21억을 넘어갈리가 없으니까 특별한 문제는 발생하지 않는다. 다만 signed로 캐스팅하는게 좀 귀찮다.

```cpp
std::vector<int> v;
...
for(int i = 0 ; i < (int)v.size() ; i++) { ... }
```

세번째 방법은 foreach를 사용하는 것이다. C++11부터는 range-based for loop가 지원되기 때문에 아래와 같이 하는 것이 가능하다. 문제는 모든 언어가 foreach를 지원하는게 아니라는 점이다. C만 해도 그런거 없다.

```cpp
std::vector<int> v;
...
for(int x : v) { ... }
```

내 경우, foreach를 사용할 수 있으면 foreach를 사용한다. foreach를 사용할 수 없는 경우에는 size_t를 이용해서 unsigned로 맞춰준다. 루프 인덱스가 필요하거나 foreach가 없는 언어를 사용할때가 여기에 속한다. 단, 백터를 뒤에서 부터 도는 경우에는 벡터의 크기를 signed로 캐스팅한다. 루프를 뒤에서부터 돌때 unsigned를 쓰면 위에서 경고했듯이 `unsigned >= 0` 같은 실수를 할수 있기 때문이다.

### API에게 강요받고 있는거다!

다음은 OpenGL의 `glDrawElements()` 문서의 일부이다. <https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDrawElements.xml>

> count: Specifies the number of elements to be rendered.
>
> type: Specifies the type of the values in indices. Must be GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT.
>
> indices: Specifies a pointer to the location where the indices are stored.

해당 GL함수는 인자로 넘어오는 type, count를 이용해서 indices 포인터를 기준으로 데이터를 읽어서 뭔가를 수행한다. 내가 쓰는 타입하고 GL에서 읽는 타입하고 다르게 짜면 문제 생겼을때 확인하기 어려우니까 GL에서 원하는 대로 indices의 배열을 unsigned char나 unsigned short으로 만들어줬다.

API가 시키는대로 안쓰다가 함정 밟으면 나만 고생하는거니까 OpenGL같이 API가 unsigned를 넣으라고 하면 그것에 맞춰줬다.

### 특수한 환경

-   메모리가 제한되어 있는 임베디드 환경
    -   안정성을 위해 변수의 크기를 늘리는 것보다 메모리 사용을 주의해야 되는 경우
    -   임베디드는 전문분야가 아니라서 더이상의 자세한 설명은 생략한다

-   unsigned/signed가 성능에 영향을 줄 때
    -   64-bit signed 대신 32-bit unsigned를 사용할때 성능이 빠르고 이게 병목지점인 경우
    -   SSE2 2개의 64-bit 대신 4개의 32-bit를 사용하고 싶을 때

-   etc

### 결론

이것은 내가 unsigned를 사용할지 선택할 때의 방식을 요약한 것이다. 몇가지 예외가 있지만 이 정도 범위에서 결정한다.

1.  API에서 unsigned를 사용하라고 하면 unsigned
2.  2진수, 16진수가 개입된 로직은 unsigned `unsigned int bitmask = 0x1234`
3.  나머지는 전부 signed
