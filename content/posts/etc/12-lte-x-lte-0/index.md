---
title: "12 ≦ x ≦ 0, 12 ≦ x ≦ 7"
subtitle: 즐거운 C언어
tags: [c, useless]
slug: 12-lte-x-lte-0
author: if1live
date: 2018-07-30
url: /posts/12-lte-x-lte-0
---

SNS을 하다 재밌는 짤을 봤다.

![12 ≦ x ≦ 0, 12 ≦ x ≦ 7](DjRKOnGWwAEUtyu.jpg)

말도 안되는 것처럼 보이지만 즐거운 C의 세계에서는 가능하다.

## 12 ≦ x ≦ 0

[연산자 우선순위](https://msdn.microsoft.com/ko-kr/library/2bxt6kc4.aspx)를 보자.

* `<`, `>`, `<=`, `>=` : 왼쪽에서 오른쪽

읽기 쉽게 괄호를 치자.
`12 <= x <= 0` 는 `(12 <= x) <= 0` 와 같다.
C의 세계에서는 true/false가 1/0이나 마찬가지니까 `(1 or 0) <= 0` 라고 쓸수있다.
`12 <= x`가 false인 x를 찾으면 된다. x는 12보다 작다.

## 12 ≦ x ≦ 7

`12 <= x <= 7` 에 괄호를 치면 `(12 <= x) <= 7` 이다.
true/false는 1/0이 들어가니까 `(1 or 0) <= 7` 이다.
1과 0 모두 7보다는 작으니 x가 뭐가 되든 성립한다.

## source code

https://ideone.com/F7HW2U

```cpp
#include <cstdio>
#include <cassert>

bool check(int x) {
  auto a = (12 <= x <= 0);
  auto b = (12 <= x <= 7);
  return a && b;
}

int main(void) {
  // invalid
  assert(check(12) == false);

  // valid
  assert(check(11));
  assert(check(10));
  assert(check(0));
  assert(check(1));

  return 0;
}
```

## 연습 문제

* C++ 연산자 오버로딩을 이용해서 `12 ≦ x ≦ 0, 12 ≦ x ≦ 7`가 성립하는 x를 구현하시오.

