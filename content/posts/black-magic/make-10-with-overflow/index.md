---
title: 오늘은 3, 1, 4, 2로 10을 만들어 봅시다!
subtitle: 오버플로우는 버그가 아닙니다. 기능입니다
tags: [overflow, black magic]
slug: make-10-with-overflow
author: if1live
date: 2018-08-06
url: /posts/make-10-with-overflow
---

## 개요
2017년에 트위터에 [#10을만들자](https://twitter.com/hashtag/10%EC%9D%84%EB%A7%8C%EB%93%A4%EC%9E%90?src=hash) 라는 해시태그가 유행한적 있다.

이런 느낌의 트윗이다.

> 오늘은 3, 1, 4, 2로 10을 만들어 봅시다!
> 존재하는 모든 연산을 허용합니다. 단, 숫자 붙이기(ex. 1과 5를 붙여 15를 만드는 것 등)는 허용하지 않습니다. 각 숫자는 한 번씩만 사용합시다.
> 0722 #10을만들자
> https://twitter.com/MathQ_kr/status/888413492465639425

며칠동안 지속된 해시였고 며칠동안 재밌는 풀이를 만들기 위해서 고민했었다.
나의 풀이법 중에 가장 기억에 남는걸 뒤늦게라도 정리해보았다.

## 오늘은 3, 1, 4, 2로 10을 만들어 봅시다!

https://twitter.com/if1live/status/888422747612692480

```c
void main(){
  char a = 3-'1'-'2'*4;
  printf("%d", a);
}
```

```bash
$ clang bar.c 2> /dev/null && ./a.out
10
```


## 오늘은 1, 5, 3, 8로 10을 만들어 봅시다!

> 오늘은 1, 5, 3, 8로 10을 만들어 봅시다!
> 존재하는 모든 연산을 허용합니다. 단, 숫자 붙이기(ex. 1과 5를 붙여 15를 만드는 것 등)는 허용하지 않습니다. 각 숫자는 한 번씩만 사용합시다.
> 0726 #10을만들자1
> https://twitter.com/MathQ_kr/status/890094934631600130


https://twitter.com/if1live/status/890447928916983808

```c
void main() {
  char a = 1 + 8 - '3' * 5;
  printf("%d", a);
}
```

```bash
$ clang foo.c 2> /dev/null && ./a.out
10
```

## 풀이

### overflow


> In computer programming,
> an integer overflow occurs when an arithmetic operation attempts
> to create a numeric value that is outside of the range
> that can be represented with a given number of bits
> – either larger than the maximum or lower than the minimum representable value.
> https://en.wikipedia.org/wiki/Integer_overflow

![overflow](overflow.gif)

오버플로우는 상식적인 내용이니까 설명하지 않는다.
풀이에서 마음에 들었던건 버그의 상징 중 하나인 오버플로우를 기능으로 접근했다는 점이다.

어떻게 문제를 풀지 정했으면 답은 끼워맞추면 된다.
문제풀이는 방향을 잡는게 어려운거지 답을 찾는게 어려운게 아니다.

char의 표현 범위는 -128~127이다.
오버플로우는 256을 주기로 반복된다.

결국 `a % 256 == b % 256` 인 a, b를 char에 넣으면 둘은 똑같다.
`10 == 266 % 256 == -246 % 256 == ...` 니까
10을 출력하는 문제는 266, -246... 중 하나를 출력하는 문제로 바꿔서 생각할수 있다.

https://ideone.com/vWHyM1

```c
#include <stdio.h>
void main(void) {
  printf("%d %d %d\n", (char)10, (char)(10+256), (char)(10-256));
}
```

```
10 10 10
```


### ASCII

3, 1, 4, 2 만으로는 266, -246을 만들기 어렵더라.
더 큰 숫자가 있으면 좋겠더라.
그래서 아스키코드를 썻다.
치트키같지만 모든 연산을 허용한댔으니 갖다쓰자.
0~9는 아스키코드로 48~57에 맵핑되니 숫자의 크기도 적절하다.

| dec | ASCII |
|----|--------|
| 48 | 0 |
| 49 | 1 |
| 50 | 2 |
| 51 | 3 |
| 52 | 4 |
| 53 | 5 |
| 54 | 6 |
| 55 | 7 |
| 56 | 8 |
| 57 | 9 |

### solution

준비가 끝났으니 명상의 시간을 가진다.
숫자를 적당히 뒤섞고 배열하면 누구든 답을 하나쯤은 찾아낼수 있다.

1. `3 - '1' - '2' * 4`
2. `3 - 49 - 50 * 4`
3. `-246`
4. `-246 % 256`
5. `10`

## 결론

* 오버플로우는 버그가 아니라 기능이다.
    * ???
* 발상의 전환을 통해 새로운 관점을 얻을 수 있다. 
