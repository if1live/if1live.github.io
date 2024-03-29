---
title: Jest 탈출기 - 타입스크립트 + Mocha
subtitle: ts-node, tsc
tags: [testing, jest, mocha]
slug: escape-from-jest-mocha-with-typescript
author: if1live
date: 2019-07-03T01:00:00+09:00
url: /posts/escape-from-jest-mocha-with-typescript
---

## 개요

[Jest 탈출기 - Jest는 느리다]({{< ref escape-from-jest-jest-is-slow >}}), 
[Jest 탈출기 - Mocha를 쓰자]({{< ref escape-from-jest-use-mocha >}})에서는 자바스크립트 기준으로 예제를 작성했다.
하지만 실제로 내가 쓰는 언어는 타입스크립트이다.

이 글에서는 타입스크립트에서 mocha 굴리는 방법을 다룬다.

## 목표: 타입스크립트로 작성된 테스트 돌리기

아래의 테스트는 타입스크립트로 작성되었다.
이것을 mocha로 돌리는게 목표이다.

```js
import { Sample } from '@src/sample';

test('ok', () => {
  const x = new Sample();
  expect(x.foo()).toBe(1);
});
```

`@src`는 `tsconfig.json`에 설정한 paths이다.
[타입스크립트에서 절대경로 쓰기]({{< ref use-absolute-path-in-typescript >}})에서 다뤘다.
여기에서는 다루지 않는다.

`test()`를 mocha에서도 쓸 수 있게 만드는 방법은 [Jest 탈출기 - Mocha를 쓰자]({{< ref escape-from-jest-use-mocha >}})에서 다뤘다.
여기에서는 다루지 않는다.

## ts-node + typescript

[ts-node][npm-ts-node] 설명에서도 mocha 사용법이 나온다.
적당히 붙여넣고 tsconfig-paths만 맞춰주면 잘 돌아간다.

```sh
mocha \
    -r node_modules/ts-node/register/transpile-only \
    -r node_modules/tsconfig-paths/register \
    -r bootload \
    src/__tests__/sample.test.ts
```


## tsc + mocha + javascript

ts-node + mocha를 쓰면 jest보다 빠르게 테스트 돌아간다.
하지만 더 빠르게 만드는게 가능할거같다.
어려운 문제를 해체해서 쉬운 문제 여러 개로 만들면 된다.
타입스크립트를 자바스크립트로 컴파일하는 과정와 mocha 돌리는 과정을 분리한다.

`tsc` 를 실행하면 타입스크립트를 자바스크립트로 컴파일 할 수 있다.
`--skipLibCheck` 옵션을 쓰면 더 효율적으로 컴파일 할 수 있다.

```
$ time npx tsc

real    0m8.804s
user    0m0.015s
sys     0m0.150s


$ time npx tsc --skipLibCheck

real    0m4.665s
user    0m0.015s
sys     0m0.121s
```

이제 컴파일된 자바스크립트를 mocha로 굴려야한다.
mocha를 그냥 쓰면 안돌아간다. `@src`는 자바스크립트가 처리하지 못한다.
webpack을 잘 쓰면 될거같은데 webpack 출력물을 그대로 mocha에 넣어도 굴러갈지 모르겠더라.
삽질하기 귀찮아서 쉬운 방법을 선택했다. 
bable을 쓰자.

아래와 같은 `.bablerc`를 넣어주면 자바스크립트에서 `@src`를 처리할 수 있다.

```json
{
  "plugins": [
    [
      "module-resolver",
      {
        "root": [
          "./dist"
        ],
        "alias": {
          "@src": "./dist/"
        }
      }
    ]
  ]
}
```

`-r babel-core/register`을 넣어주면 mocha에서 babel을 사용한다.

```sh
mocha \
    -r babel-core/register \
    -r bootload \
    dist/__tests__/sample.test.js
```

## 성능

tsc + mocha 조합으로 테스트 돌리는게 ts-node + mocha보다 빠르다.
ts-node는 간단하게 쓰기 좋은거지 성능 생각하면서 쓸 물건이 아닌가보다.

내 경우 타입스크립트 컴파일 할라고 `tsc -w --skipLibCheck`를 켜둔다.
간단한 수정의 경우는 1초안에 컴파일이 끝난다.
mocha가 전체 테스트하는데는 4-5초 정도 걸린다.

ts-node는 10초 이상 걸린다.
체감상 2-3배는 느린거같다.

## ts-node에 의미가 있는가?

ts-node + mocha 조합은 느리지만 그렇다고 쓸모없는건 아니다.
테스트 실패시 콜스택이 멀쩡하게 나온다.

tsc + mocha 조합은 타입스크립트 파일을 테스트에 쓰는게 아니다.
컴파일된 자바스크립트 파일을 테스트하는 것이다.
그래서 테스트 실패시 콜스택에는 자바스크립트 파일의 좌표가 나온다.

```txt
at Test.<anonymous> (<hidden>/src/server/__tests__/app.test.js:20:26)
at Test.assert (node_modules\supertest\lib\test.js:181:6)
at Server.localAssert (node_modules\supertest\lib\test.js:131:12)
at Server.EventEmitter.emit (domain.js:442:20)
at emitCloseNT (net.js:1618:8)
at process._tickCallback (internal/process/next_tick.js:63:19)
```

ts-node + mocha 조합에서는 타입스크립트 파일의 좌표가 제대로 나온다.

```txt
at Test.<anonymous> (src\server\__tests__\app.test.ts:19:18)
at Test.assert (node_modules\supertest\lib\test.js:181:6)
at Server.localAssert (node_modules\supertest\lib\test.js:131:12)
at Server.EventEmitter.emit (domain.js:442:20)
at emitCloseNT (net.js:1618:8)
at process._tickCallback (internal/process/next_tick.js:63:19)
```

## 결론

이것이 Jest 탈출기의 마지막 글이다.

jest 탈출 이전에는 유닛테스트 한번 돌리는데 1분이 걸려서 테스트 이터레이션을 자주 돌리지 못했다.
watch 걸어두는건 생각도 못했다.
테스트 결과를 보는데 1분이 걸리니 watch를 왜 띄우지?
그 시간에 코딩을 하고말지.

tsc + mocha + babel로 탈출한 이후에는 watch를 켜고 작업한다.
`tsc -w --skipLibCheck`는 매우 빠르게 돌아간다. 간단한 수정은 1초 안에 끝난다.
mocha로 테스트 전체 돌아가는데도 5초면 충분하다.
저장하고 5-6초면 전체 테스트가 끝난다. 그것도 통합 테스트가!

이제 TDD 하는 기분을 낼 수 있다.

[npm-ts-node]: https://www.npmjs.com/package/ts-node
