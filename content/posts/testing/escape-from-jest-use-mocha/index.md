---
title: Jest 탈출기 - Mocha를 쓰자
subtitle: Jest/Mocha 동시에 지원하기
tags: [testing, jest]
slug: escape-from-jest-use-mocha
author: if1live
date: 2019-07-02T01:00:00+09:00
url: /posts/escape-from-jest-use-mocha
---

## 개요

이전에 [Jest 탈출기 - Jest는 느리다]({{< ref escape-from-jest-jest-is-slow >}})를 썼다.
이를 통해 jest를 탈출해서 mocha로 넘어가기로 마음먹었다.

jest는 느린거 빼곤 문제가 없다.
CI에서는 성능은 심각한 문제가 아니니 jest를 써도 된다.
CI에서는 jest의 장점인 테스트 간의 고립을 유지하고 싶다.
개발 환경에서는 mocha의 장점인 빠른 이터레이션을 얻고 싶다.

근데 이미 테스트 코드를 jest 기준으로 작성했다.
jest와 mocha는 미묘하게 달라서 그냥은 안돌아간다.
그래서 jest 기준으로 작성된 테스트 코드가 mocha로도 잘 돌아가도록 mocha를 손댔다.

## 목포: jest 테스트 코드를 mocha로 돌리기

```js
let counter = 0;

function print(message) {
  counter += 1;
  console.log(`${message} : ${counter}`);
}

beforeAll(() => print('before all - global'));
afterAll(() => print('after all - global'));
beforeEach(() => print('before each - global'));
afterEach(() => print('after each - global'));

describe('foo', () => {
  beforeAll(() => print('before all - local'));
  afterAll(() => print('after all - local'));
  beforeEach(() => print('before each - local'));
  afterEach(() => print('after each - local'));

  test('ok', () => {
    expect(1).toBe(1);
    print('ok');
  });
  test('ok2', () => {
    expect(1).toBe(1);
    print('ok2');
  });
  test.skip('ok2', () => expect(1).toBe(1));
});

describe.skip('skip', () => {
  test('ok', () => expect(1).toBe(1));
});
```

```txt
before all - global : 1
before all - local : 2
before each - global : 3
before each - local : 4
ok : 5
after each - local : 6
after each - global : 7
before each - global : 8
before each - local : 9
ok2 : 10
after each - local : 11
after each - global : 12
after all - local : 13
after all - global : 14
```

테스트를 jest로 돌리면 위와 비슷한 출력을 얻는다.
mocha로 비슷한 출력을 뽑아내는게 목표이다.

## jest와 mocha의 차이

나는 jest의 고급기능까지 전부 쓰지 않는다.
내가 쓰는 범위에서의 jest와 mocha의 차이는 이거뿐이었다.

* `beforeAll()`, `afterAll()`은 jest에는 있지만 mocha에는 없다.
* `test()`, `it()` 둘다 jest에서는 쓸 수 있지만 mocha에는 `it()`만 있다.

## mocha에서 돌리기

`--require`, `-r` 옵션을 통해서 mocha에 자바스크립트를 꽂을 수 있다.
이를 통해 jest에 있지만 mocha에는 없는 함수를 꽂아주면 mocha에서도 jest용 테스트 코드를 돌릴 수 있을 것이다.
나보다 jest의 고급기능을 많이 쓰는 사람도 잔머리를 잘 굴리면 우회법을 찾을 수 있을 것이다.

```js
var expect = require('expect');
global.expect = expect;

global.test = (name, fn) => global.it(name, fn);
global.test.skip = (name, fn) => global.it.skip(name, fn);

global.beforeAll = (name, fn) => global.before(name, fn);
global.afterAll = (name, fn) => global.after(name, fn);
```

`mocha -r bootload sample.test.js`

```txt
before all - global : 1
  foo
before all - local : 2
before each - global : 3
before each - local : 4
ok : 5
    √ ok
after each - local : 6
after each - global : 7
before each - global : 8
before each - local : 9
ok2 : 10
    √ ok2
after each - local : 11
after each - global : 12
    - ok2
after all - local : 13

  skip
    - ok

after all - global : 14
```
