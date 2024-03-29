---
title: Jest 탈출기 - Jest는 느리다
subtitle: jest는 왜 mocha보다 느린가?
tags: [testing, jest, mocha]
slug: escape-from-jest-jest-is-slow
author: if1live
date: 2019-07-01T01:00:00+09:00
url: /posts/escape-from-jest-jest-is-slow
---

## 개요

요새 작업하는 프로젝트에서 jest를 사용해서 유닛테스트를 돌린다.
프로젝트가 진행될수록 테스트가 점점 느려지더니 이제 유닛 테스트 한번 돌리는데 1분이 걸린다.
라이젠 붙은 좋은 컴퓨터에서 CI 돌리는데 여기에서도 30초나 걸린다.

근데 같은 테스트 코드를 mocha로 돌리면 몇초 안걸리더라?
jest는 mocha와 달리 병렬적으로 테스트가 돌아가니 훨씬 빨라야 하지 않나?
근데 몇배가 느리다고?

왜 jest가 mocha보다 느리게 테스트가 굴러가는지 뜯어봤다.

## 속도 비교

```js
const a1 = Date.now();
var express = require('express');
const a2 = Date.now();
var express = require('express');
const a3 = Date.now();

const pid = process.pid;
console.log(`first load: ${(a2 - a1) / 1000} pid=${pid}`);
console.log(`second load: ${(a3 - a2) / 1000} pid=${pid}`);

const expect = require('expect');

it(__filename, () => expect(1).toBe(1));
```

위의 테스트 코드를 복사해서 3개의 테스트 파일로 만들었다.
3개의 테스트 스위트를 통해 다음의 요소를 확인하고 싶었다.

1. express를 처음으로 import하는데 걸리는 시간
2. express를 두번째 import하는데 걸리는 시간
3. 테스트 프로세스의 pid

## 실행 결과

### mocha

```txt
first load: 0.124 pid=18380
second load: 0 pid=18380
first load: 0 pid=18380
second load: 0 pid=18380
first load: 0 pid=18380
second load: 0 pid=18380
```

* 모든 테스트의 pid는 동일하다.
* 첫번째 import에서만 시간이 소모된다.
* 일단 import를 했으면 그 이후의 import는 캐싱된다.

### jest

`--runInBand` 옵션을 넣고 돌리면 jest는 하나의 worker로 돌아간다.
아래와 비슷한 결과를 얻을수 있다.

```txt
PASS test/a.test.js
  ● Console

    console.log test/a.test.js:8
      first load: 0.216 pid=15604
    console.log test/a.test.js:9
      second load: 0 pid=15604

PASS test/c.test.js
  ● Console

    console.log test/c.test.js:8
      first load: 0.021 pid=15604
    console.log test/c.test.js:9
      second load: 0 pid=15604

PASS test/b.test.js
  ● Console

    console.log test/b.test.js:8
      first load: 0.021 pid=15604
    console.log test/b.test.js:9
      second load: 0 pid=15604
```

* 모든 테스트의 pid는 동일하다. `--runInBand`가 없으면 pid는 여러개가 된다.
* 각각의 테스트 스위트에서 처음으로 import할때 시간이 소모된다.
* 두번째 import는 시간이 걸리지 않는다. 캐싱된 물건을 쓰나보다.

### mocha vs jest

mocha는 한번 import하면 그 이후에는 캐싱된 결과물을 사용한다.
일반적인 node 프로그램과 동일한 방식이다.

jest는 각각의 테스트 스위트가 고립된 상태로 돌아간다.
같은 프로세스 안에서 돌아가더라도 테스트 스위트가 다르면 import를 다시 한다.

## 왜 둘은 다른가?

jest 문서를 제대로 읽어보면 설명을 찾을 수 있을지 모른다.
하지만 나는 영어만 읽으면 정신이 피곤해지니 반대로 접근했다.
문서에서 vm으로 검색해서 관련된 [항목](https://jestjs.io/docs/en/configuration#extraglobals-array-string)을 발견할 수 있었다.

> Test files run inside a vm, which slows calls to global context properties (e.g. Math).
> With this option you can specify extra properties to be defined inside the vm for faster lookups.

[VM (Executing JavaScript)][node-vm]을 쓴다니!
각각의 테스트 스위트를 독립적으로 돌리기 위해서 vm을 사용한 jest의 접근법이 이해가 된다.
각각의 vm이 독립적으로 돌아갈테니 import를 매번 하는 것도 알겠더라.

## Jest 탈출기

내가 요새 작업하는 물건은 express 기반 서버이다.
[supertest][github-supertest]를 이용해서 대부분의 API에 통합 테스트(integration testing)를 적용해놨다.
아래와 같은 테스트가 각각의 API endpoint 별로 작성되어 있다.

```ts
import { makeApp } from '@server/app';

const app = makeApp();

describe('GET /user', () => {
  test('ok', done => {
    request(app)
      .get('/user')
      .expect(200, done);
  });
});
```

순서대로 생각하면 jest가 느린 이유가 잘 보인다.

1. 작업중인 프로젝트는 express 기반 서버
2. `makeApp()`은 사실상 `main()` 역할
3. `makeApp()`을 import하는 과정을 따라가면 프로젝트 전체를 import하게 된다.
4. 각각의 테스트 스위트마다 프로젝트 전체를 import한다.
5. 프로젝트가 커지면 `makeApp()` import하는 시간이 점점 더 느려진다.
6. 테스트 스위트가 많아질수록 테스트 돌아가는 시간이 길어진다.

반면 mocha의 경우는 각각의 테스트는 하나의 프로그램 안에서 돌아간다.
import를 매번하지 않으니까 훨씬 빠르다.
그래서 나는 jest를 버리고 mocha로 갈아타기로 했다.
테스트간의 독립성보다 빠른 이터레이션이 나한테는 훨씬 중요하다.

[node-vm]: https://nodejs.org/api/vm.html
[github-supertest]: https://github.com/visionmedia/supertest
