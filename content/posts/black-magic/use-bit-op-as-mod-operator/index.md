---
title: 나머지 연산자 없이 x % (2의 거듭제곱) 계산하기
tags: [modulo operator]
slug: use-bit-op-as-mod-operator
author: if1live
date: 2018-08-20
url: /posts/use-bit-op-as-mod-operator
---

이전에 [나머지 연산자 없이 x % 256 계산하기][blog-use-casting-as-mod-operator]라는 글을 썻다.
% 256만 계산할 수 있으면 재미없으니까 더 확장해보자.

% 256을 구하기 위해서 캐스팅을 이용했다.
4바이트 정수를 1바이트 정수로 캐스팅해서 하위 1바이트만 남겼다.
이를 바꿔말하면 256진법의 4자리수에서 마지막 한자리만 남겼다고 표현할 수 있다.
1234를 100으로 나누는 것을 생각해보자. 
진짜로 100으로 나누는 사람이 있나 모르겠다.
백의 자리에서 숫자를 자르면 같은 값을 얻을 수 있고 더 빠르게 계산 가능하다.

256진법, 10진법말고 2진법에도 이것을 적용하자.
비트 연산을 쓰면 된다.

## 구현

https://ideone.com/nMpPFH

```c
#include <assert.h>

unsigned int mod_power_of_two(unsigned int a, unsigned int b)
{
  //assert(b && (b & (b - 1)) == 0);
  return a & (b - 1);
}

unsigned int mod(unsigned int a, unsigned int b)
{
  return a % b;
}

int main()
{
  assert(mod_power_of_two(17, 1) == mod(17, 1));
  assert(mod_power_of_two(17, 2) == mod(17, 2));
  assert(mod_power_of_two(17, 4) == mod(17, 4));
  assert(mod_power_of_two(17, 8) == mod(17, 8));
  return 0;
}
```

부호를 신경쓰기 귀찮으니 unsigned를 기준으로 짰다.
비트 and 연산자를 이용해서 제수(divider) 아래 자리의 비트만 남긴다.
십진수를 10의 거듭제곱으로 나누는걸 2진법으로 바꿨을뿐이다.


https://godbolt.org/z/ygxVdQ

```s
mod_power_of_two(unsigned int, unsigned int):                 # @mod_power_of_two(unsigned int, unsigned int)
        lea     eax, [rsi - 1]
        and     eax, edi
        ret
mod(unsigned int, unsigned int):                               # @mod(unsigned int, unsigned int)
        xor     edx, edx
        mov     eax, edi
        div     esi
        mov     eax, edx
        ret
main:                                   # @main
        xor     eax, eax
        ret
```

x86-64 clang 6.0.0으로 `-O3` 옵션으로 컴파일한 결과다.
비트연산을 이용해서 나머지를 계산하면 % 연산자를 사용했을때보다 어셈이 짧다.


## 재대결

위의 테스트에서는 한가지 함정이 있다.
제수(divider)가 2의 거듭제곱이 될 것이라는걸 우리는 알고있다.
하지만 컴파일러는 모른다.
컴파일러는 정직하게 나머지 연산을 계산해야하기 떄문에 더 많은 명령이 필요했을지 모른다.

아래의 코드는 divider를 16으로 하드코딩했다.
이렇게하면 컴파일러도 16으로만 나누면 된다는걸 알거다.

https://ideone.com/jBy5kV

```c
#include <assert.h>

unsigned int mod_16(unsigned int a)
{
  return a & (16 - 1);
}

unsigned int mod_16_simple(unsigned int a)
{
  return a % 16;
}

int main() { return 0; }
```

https://godbolt.org/z/Ox6pn7

```s
mod_16(unsigned int):                             # @mod_16(unsigned int)
        and     edi, 15
        mov     eax, edi
        ret
mod_16_simple(unsigned int):                     # @mod_16_simple(unsigned int)
        and     edi, 15
        mov     eax, edi
        ret
main:                                   # @main
        xor     eax, eax
        ret
```

x86-64 clang 6.0.0으로 `-O3` 옵션으로 컴파일하면 두 함수의 어셈은 똑같다.

## 결론

괜히 이상한 코드 최적화 하지 말자.
그런건 위대한 컴파일러님이 다 해주실거다.
코드 최적화는 컴파일러님한테 맡기고 너는 로직만 짜면 된다.

[blog-use-casting-as-mod-operator]: {{< ref use-casting-as-mod-operator >}}
