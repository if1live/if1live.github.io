---
title: 나도 해보는 Fizz Buzz Test
subtitle: printf의 리턴값은?
tags: [fizz-buzz]
slug: fizz-buzz-01
author: if1live
date: 2018-08-04
url: /posts/fizz-buzz-01
---

## Fizz Buzz Test?
[Fizz Buzz Test](http://wiki.c2.com/?FizzBuzzTest)는 상당히 오래된 떡밥이다.

> Write a program that prints the numbers from 1 to 100. 
> But for multiples of three print "Fizz" 
> instead of the number and for the multiples of five print "Buzz".
> For numbers which are multiples of both three and five print "FizzBuzz".

> 우리 말로 풀어서 쓰면,
> 1부터 100사이의 숫자를 프린트하는 프로그램을 작성하는데 3의 배수이면 "Fizz"를,
> 5의 배수이면 "Buzz"를,
> 둘 모두의 배수 즉 15의 배수이면 "FizzBuzz" 를 프린트하도록 하라. 

번역을 내가 하기 귀찮아서 [Barracuda MemoLog](http://bryan.wiki/260)에서 긁어왔다.

Fizz Buzz 테스트는 오래되고 쉬운 문제인 만큼 뻔한 풀이법도 많고 뻔한 해설도 많다.
하지만 나는 뻔한 길은 가지 않는다. 재밌는 풀이가 생각나서 구현해봤다.

## code

```c
#include <stdio.h>

int main(void) {
  for(int i = 1 ; i <= 100 ; i++) {
    int found = 0;
    if(!(i % 3)) { found += printf("Fizz"); }
    if(!(i % 5)) { found += printf("Buzz"); }
    if(!found) { printf("%d", i); }
    printf("\n");
  }
  return 0;
}
```

## 풀이

### printf의 리턴값

C를 해본 사람중에 `printf()` 를 모르는 사람은 없을것이다.
하지만 printf의 리턴값을 사용해본 사람은 별로 없을거다.

printf의 리턴값을 뒤지면 다음과 같은 내용이 나온다

> number of characters transmitted to the output stream 
> or negative value if an output error or an encoding error
> (for string and character conversion specifiers) occurred
> 
> https://en.cppreference.com/w/c/io/fprintf

일반적인 상황에서는 printf가 실패하진 않을테니 쓰여진 글자수라고 생각해도 된다.
변수 found를 printf의 호출 여부를 기록하는 플래그처럼 사용했다.

### FizzBuzz = Fizz + Buzz

| number |    1  |  2    |  3     |
|---------|------|-------|--------|
| 3의 배수 | Fizz |      |        |
| 5의 배수 |      | Buzz |        |
| 15의 배수| Fizz | Buzz |        |
| 나머지   |      |      | number |

Fizz, Buzz 순서로 출력할수 있도록 배치했다.
3, 5의 배수일떄는 한쪽만 걸리지만 15의 배수일떄는 양쪽을 통과하게 했다.
printf 의 리턴값을 이용해서 만든 플래그를 사용해서 숫자를 출력해야되는 경우를 처리했다.

## 오늘의 숙제

* 리턴값을 한번도 사용해본적 없는 C 표준 라이브러리 함수의 리턴값 의미 찾아보기
