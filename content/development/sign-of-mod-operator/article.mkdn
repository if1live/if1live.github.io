---
title: "-4 % 3 = ?"
subtitle: 2 vs -1
tags: [mod]
slug: sign-of-mod-operator
author: if1live
date: 2018-08-16
url: /posts/sign-of-mod-operator
---

`%` 연산자를 응용해보는 글을 쓰기전에 % 연산자에 대해서 간단하기 글을 써보기로 했다.
`%` 는 나머지 연산자로`4 % 3` 을 계산하면 1이 나온다는건 다들 알고있을거다.

그렇다면 `-4 % 3`, `4 % -3`의 결과는?


## python

```
Python 3.7.0 (v3.7.0:1bf9cc5093, Jun 27 2018, 04:06:47) [MSC v.1914 32 bit (Intel)] on win32
Type "copyright", "credits" or "license()" for more information.
>>> -4 % 3
2
>>> 4 % -3
-2
```

* -4 % 3 = 2
* 4 % -3 = -2

## javascript

```
> -4 % 3
< -1
> 4 % -3
< 1
```

* -4 % 3 = -1
* 4 % -3 = 1

## note

파이썬과 자바스크립트에서 결과가 다르다.
wikipedia의 [Modulo operation][wiki-mod]를 보면 이유가 정리되어있다.
파이썬 % 연산자의 결과는 Divisor(제수)의 부호를 따라간다.
그래서 `3`, `-3`의 부호를 따라가서 `2`, `-2`가 나온다.
자바스크립트 % 연산자의 결과는 Dividend(피제수)의 부호를 따라간다.
그래서 `-4`, `4`의 부호를 따라가서 `-1`, `1`가 나온다.

위키피디아에 정리된 mod 연산 결과의 부호를 보면 개판을 구경할 수 있다.

* Dividend
* Divisor
* implementation defined
* Nearest to zero
* Nonnegative always
* Undefined

내 경우는 주력 언어가 딱히 없다.
매번 사용하는 언어가 바뀐다.
게다가 동시에 여러 언어를 사용한다.
현재 진행하고 있는 프로젝트만 하더라도 파이썬과 자바스크립트를 같이 쓴다.
똑같이 생긴 코드가 언어에 따라서 동작이 바뀐다면 개발하다 헷갈려서 버그가 생길 수 있다.
그래서 나는 % 연산자같이 언어에 따라서 동작이 바뀌는 기능은 거의 쓰지 않는다.

아니면 동작이 똑같도록 만들거나.

mod 연산 결과의 부호가 개판인것처럼 보이지만 적어도 하나는 보장된다.
Dividend, Divisor 가 모두 양수이면 결과는 음수가 되지 않는다.
적절히 짜서 % 연산자에 음수가 들어갈 일이 없도록 할때도 있다.

아니면 % 계산 결과가 음수인지 확인해보고 음수이면 Divisor를 더해서 양수로 만들어주거나.

[wiki-mod]: https://en.wikipedia.org/wiki/Modulo_operation
