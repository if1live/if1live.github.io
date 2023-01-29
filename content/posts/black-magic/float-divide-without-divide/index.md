---
title: 나눗셈 연산을 곱셈으로 프로그래밍 할 수 있음?
subtitle: 즐거운 부동소수의 세계
tags: [floating-point]
slug: float-divide-without-divide
author: if1live
date: 2018-08-05
url: /posts/float-divide-without-divide
---

## 개요

몇달전에 SNS를 하다가 재밌는 글을 봤다.

> 나눗셈 연산을 곱셈으로 프로그래밍 할 수 있음?
> 
> 나누기/ 연산자를 안쓰고


정수에 대해서는 어떻게 구현할지 잘 모르겠지만 부동소수에서 간단한 방법이 보이더라.
[Fast inverse square root](https://en.wikipedia.org/wiki/Fast_inverse_square_root)를 쓰면 될거같다.

고속 역 제곱근은 $\frac{1}{\sqrt{x}}$를 구하는 함수이다.
이것을 제곱하면 $\frac{1}{x}$가 된다.
`/x`나 `*(1/x)`나 똑같은거니까 문제는 다 푼것과 마찬가지다.

코드를 공유할때는 [ideone](http://ideone.com)같은게 편하더라.
그래서 코드를 넣고 돌렸다. 그리고 재밌는 결과가 나오기 시작하는데....

## code

```c
#include <stdio.h>
#include <math.h>

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float Q_rsqrt( float number )
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

void print(float n) {
  float result_1 = 1 / n;
  float result_2 = Q_rsqrt(n) * Q_rsqrt(n);
  printf("%f %f\n", result_1, result_2);
}

int main() {
  print(1000.0f);
  print(100.0f);
  print(10.0f);
  print(1.0f);
  print(0.1f);
  print(0.01f);
  return 0;
}
```


앞의 계산 결과는 `1 / x`를 수행한 것이다.
뒤의 계산 결과는 fast inverse square root를 제곱해서 `1 / x`를 구한것이다.

### ideone + gcc 6.3

https://ideone.com/8GRJrX

```
0.001000 -0.000997
0.010000 -0.009969
0.100000 -0.099658
1.000000 -0.996617
10.000000 -9.968114
100.000000 99.650749
```

재밌는 결과의 시작이다.

`Q_rsqrt()` 함수는 static 변수가 없으니 내부에서 상태가 있진 않다.
항상 같은 계산 결과를 리턴할 것이다.
복소수 i를 다루고 있진 않으니까 같은 수를 제곱해서 음수가 되지 않을것이다.

그런데 그런일이 실제로 일어났습니다.

???

### ideone + clang 4.0

https://ideone.com/n8krf8

```
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

gcc에서 충격받고 clang으로도 돌려봤다.
clang에서는 제곱 결과가 양수로 잘 나온다.

???

## optimize

이유는 모르겠지만 실행 결과가 다르다.
컴파일러에 따라서 실행 결과가 바뀌는건가?
컴파일러 버그인가?

이런걸 생각해보기전에 최적화 옵션을 건드려보자.
최적화 옵션을 건드린다고 실행 결과가 바뀔거같진 않지만 현실은 그렇지 않다는걸 몇번 당해봤으니까.

### gcc + optimize

```
% gcc --version
gcc (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

```
% gcc main.c && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

기본 컴파일 옵션에서는 gcc도 양수로 잘 나온다.
옵션을 쓰지 않으면 최적화 기본값인 `-O0`으로 돌아간다.

```
% gcc main.c -O1 && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

`-O1` 에서도 양수로 잘 나온다.


```
% gcc main.c -O2 && ./a.out
0.001000 -0.000997
0.010000 -0.009969
0.100000 -0.099658
1.000000 -0.996617
10.000000 -9.968114
100.000000 99.650749
```

`-O2` 에서는 음수가 보인다.

```
% gcc main.c -O3 && ./a.out
0.001000 -0.000997
0.010000 -0.009969
0.100000 -0.099658
1.000000 -0.996617
10.000000 -9.968114
100.000000 99.650749
```

`-O3` 에서도 음수가 보인다.

컴파일러 최적화때문에 gcc에서는 제곱의 결과로 음수가 나온거같다.

### clang + optimize

```
% clang --version
clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
```

```
% clang main.c && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

```
% clang main.c -O1 && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

```
% clang main.c -O2 && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

```
% clang main.c -O3 && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

clang에서는 최적화 옵션을 건드린다고 결과가 바뀌지 않는다.

## 임시변수

예전에 [100 * 0.7 = ?](/posts/amazing-floating-point-100x07/)라는 글을 쓴적이 있다.
임시변수를 사용하면 부동소수 계산이 바뀌는걸 그때 봤었다.
임시변수를 쓰도록 구현을 바꿔보았다.

```c
#include <stdio.h>
#include <math.h>

float Q_rsqrt( float number )
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

void print(float n) {
  float result_1 = 1 / n;

  float x = Q_rsqrt(n);
  float result_2 = x * x;
  printf("%f %f\n", result_1, result_2);
}

int main() {
  print(1000.0f);
  print(100.0f);
  print(10.0f);
  print(1.0f);
  print(0.1f);
  print(0.01f);
  return 0;
}
```

https://ideone.com/3ZkwP7

```
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

ideone + gcc에서 잘 나온다.

```
% gcc -O3 main_with_tmpval.c && ./a.out
0.001000 0.000997
0.010000 0.009969
0.100000 0.099658
1.000000 0.996617
10.000000 9.968114
100.000000 99.650749
```

gcc 최적화 옵션이 높을떄 음수가 나오는 문제도 해결되었다.
