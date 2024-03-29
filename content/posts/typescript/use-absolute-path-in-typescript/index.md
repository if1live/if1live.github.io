---
title: 타입스크립트에서 절대경로 쓰기
subtitle: typescript, absolute path, webpack, ts-node, jest
tags: [typescript, ts-node, jest, webpack, alias]
slug: use-absolute-path-in-typescript
author: if1live
date: 2019-02-10
url: /posts/use-absolute-path-in-typescript
---

## 개요

요새 타입스크립트를 쓰고있다.
100줄짜리 프로그램 짜는게 아니니 소스파일이 수십개로 나뉘어져있다.
파일에 접근하기 위해서는 디렉토리 구조도 몇단계로 내려가야 한다.
소스 파일간의 import하는 일이 많은데 타입스크립트에서는 기본적으로 상대경로 쓰더라.
그래서 import에 들어가는 경로가 깨끗하지 않다.
`../../..`가 자주 등장하더라.

```typescript
import { bar } from '../../../../bar';

export const foo = () => {
  console.log(bar());
};
```

설계에 따라서는 상대경로로 밑에서 경로를 접근하는것보다 위에서 내려가는게 빠를 수 있다.
그리고 import 경로로 깔끔하게 만들 수 있다.

```typescript
import { bar } from '@src/bar';

export const foo = () => {
  console.log(bar());
};
```

절대경로라고 표현하긴 했지만 `c:\repo\sample\src\bar\index.ts`같은 진짜 절대경로는 쓰지 않을 것이다.
진짜 절대경로는 다른 컴퓨터에서 안돌아간다.
프로젝트 디렉토리를 루트 디렉토리로 취급하는 것을 절대경로라고 표현할거다.
유사-절대경로라고 부를 수 있다.

이 글에서는 타입스크립트에서 절대경로를 쓰는 방법을 정리했다.
그리고 내가 자주 쓰는 도구인 webpack, ts-node, jest가 제대로 동작하게 설정하는 방법도 정리했다.


## 샘플 프로젝트 구조

절대경로가 잘 돌아가는지 확인하기 위한 [샘플 프로젝트][repo-sample]를 만들었다.
디렉토리 구조는 다음과 같다.

```
.
├── src
│   ├── index.ts
│   ├── foo
│   │   └── a
│   │       └── b
│   │           └── c
│   │               └── index.ts
│   └── bar
│       └── index.ts
└── test
    └── bar
        └── index.test.ts
```

src/index.ts가 프로젝트의 진입점이다.
src/index.ts는 foo/a/b/c/index.ts를 갖다쓴다.
src/foo/a/b/c/index.ts에서 src/bar/index.ts를 갖다쓴다.
유닛테스트는 별도 디렉토리로 분리했다.
테스트는 jest를 사용해서 돌릴거다.

해당 프로젝트에서는 상대경로를 쓰지 않는다.
절대경로만으로 돌아가게 만들 것이다.

## tsc

src/foo/a/b/c/index.ts 내용은 다음과 같다.
상대경로를 썻으면 import가 얼마나 길어지는지 비교할 수 있다.

```typescript
// import { bar } from '../../../../bar';
import { bar } from '@src/bar';

export const foo = () => {
  console.log(bar());
};
```

이 코드는 컴파일 되지 않는다.
타입스크립트는 `@src`가 어디인지 모르기 때문이다.
나는 `@src`가 프로젝트 디렉토리의 `/src`라고 알고있다.
타입스크립트 설정파일을 고치자.
타입스크립트 컴파일러가 `@src`가 무엇인지 알면 제대로 컴파일할 수 있다.

타입스크립트 문서에서 [Module Resolution][ts-module-resolution]를 찾아보자.
Path mapping 항목을 tsconfig.json에 적용하자.
paths와 baseUrl을 적절히 설정해주 컴파일러가 `@src`를 이해할 수 있다.

tsconfig.json

```json
{
  "compilerOptions": {
    ...
    "baseUrl": ".",
    "paths": {
      "@src/*": [
        "src/*"
      ]
    }
  }
}
```

이제 컴파일이 된다.

## node

컴파일된 코드를 node로 돌려보자.

```
$ node .\dist\src\index.js
internal/modules/cjs/loader.js:582
    throw err;
    ^

Error: Cannot find module '@src/bar'
    .....
```

안돌아간다.
컴파일만 되었을뿐이다.

tsc에 의해서 생성된 파일을 열어보면 원인을 알 수 있다.
소스를 보면 `require(@src/bar)`가 있다.
하지만 node는 `@src/bar`가 어떤 경로인지 모른다.

```javascript
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
// import { bar } from '../../../../bar';
var bar_1 = require("@src/bar");
exports.foo = function () {
    console.log(bar_1.bar());
};
```

비슷한 문제는 다들 한번씩 겪었을것이다.
C, C++ 쓰다보면 컴파일은 성공했지만 링킹(linking)이 되지 않는 문제를 당했을 것이다.
비주얼 C++ 쓰다보면 보는 LNK???? 에러.

C를 컴파일하면 object 파일이 생성된다.
여러 파일로 구성된 C 프로젝트면 object 파일도 여러개가 생성된다.
이것을 하나로 합쳐야 실행 가능한 파일이 만들어진다.

타입스크립트 소스 파일들이 컴파일되어 자바스크립트 파일이 생성되었다.
하지만 자바스크립트 파일간에는 연결이 없다.
`@src`를 node도 알아먹을수 있는 형태로 바꿔야한다.

## webpack

webpack은 모듈 번들러이다.
webpack을 쓰면 분리되어있는 자바스크립트 소스 파일을 하나로 파일로 묶어줄 수 있다.

![webpack](toptal-blog-image-1476174229140-3890202f75d94c0692549af41f9d652a.png)

생각해보면 linker와 하는 짓이 유사하다.

![linker](COMPILE.gif)

linking 하듯이 webpack을 이용해서 분리된 자바스크립트 파일을 합칠때 `@src`를 멀쩡한 이름으로 바꿀 수 있으면 프로그램이 돌아갈거다.
webpack이 `@src`를 알아듣게 만드는 방법은 남들이 간 길을 찾으니 나오더라.
[Working with TypeScript, webpack and Ionic 3](https://robferguson.org/blog/2017/11/22/working-with-typescript-webpack-and-ionic-3/) 를 참고했다.
webpack 설정파일에 alias 를 설정하면 된다더라.
나는 절대경로라고 불렀는데 webpack에서는 이것을 alias라고 부르나보다.

webpack.config.js

```javascript
...
module.exports = {
  ....
  resolve: {
    extensions: [
      '.ts',
      '.tsx',
      '.js',
      '.json',
    ],
    // -----
    'alias': {
      '@src': path.resolve(__dirname, 'src'),
    },
    // ----
  },
};
```

이제 webpack으로 뽑은 파일은 잘 실행된다.

## ts-node

[ts-node][ts-node]는 내가 좋아하는 도구다.
프로젝트의 설명을 보면 TypeScript execution and REPL for node.js 라고 되어있다.
타입스크립트는 node에서 바로 돌릴 수 없다.
tsc로 컴파일한후 생성된 자바스크립트 파일을 node로 돌려야한다.
하지만 이짓을 하는건 너무 귀찮잖아?
ts-node를 설치하면 `node xxx.js` 실행하는 느낌으로 `ts-node xxx.ts`를 실행할 수 있다.

절대경로가 들어간 타입스크립트를 ts-node로 실행하면 안돌아간다.
아래와 같은 에러가 튀어나온다.

```
$ npx ts-node src/index.ts
Cannot find module '@src/bar'
```

ts-node가 `@src`를 못 알아먹는다.
tsconfig.json 에 `@src`가 있지만 ts-node가 tsconfig.json의 내용을 전부 쓰지 않나보다.
ts-node의 문서를 뒤져보면 tsconfig-paths를 설치해서 문제를 해결할 수 있다고한다.
이제 절대경로 들어간 타입스크립트도 ts-node로 돌릴 수 있다.

```
$ npx ts-node -r tsconfig-paths/register --files src/index.ts
bar
```

ts-node를 쓸때마다 tsconfig-paths 관련 설정을 치는것은 매우 귀찮다.
package.json를 고치면 노가다가 조금 줄어든다.

package.json

```json
{
  ...
  "scripts": {
    "ts-node": "ts-node -r tsconfig-paths/register --files"
  },
  ...
}
```

```
$ npm run ts-node src/index.ts

> use-relative-path-in-typescript@1.0.0 ts-node ...
> ts-node -r tsconfig-paths/register --files ".\src\index.ts"

bar
```

## jest

나는 jest를 이용해서 유닛테스트를 돌린다.
jest는 기본적으로 자바스크립트만 지원한다.
하지만 ts-jest를 설치하면 타입스크립트도 지원한다.
ts-jest를 설치하고 jest돌릴때 타입스크립트 파일은 ts-jest를 거치도록 설정하면 된다.

package.json
```json
{
  ...
  "jest": {
    ...
    "transform": {
      "^.+\\.tsx?$": "ts-jest"
    },
    "testRegex": "(/__tests__/.*|(\\.|/)(test|spec))\\.(tsx?)$",
    ...
  }
}
```

잘 설정하면 아래와 같은 유닛테스트를 돌릴 수 있다.

```typescript
import { bar } from '../../src/bar';

test('bar', () => {
  expect(bar()).toEqual('bar');
})
```

그렇다면 절대경로를 사용한 유닛테스트도 돌아갈까?

```typescript
import { bar } from '@src/bar';

test('bar', () => {
  expect(bar()).toEqual('bar');
})
```

```
 FAIL  test/bar/index.test.ts
  ● Test suite failed to run

    Cannot find module '@src/bar' from 'index.test.ts'

    > 1 | import { bar } from '@src/bar';
        | ^
      2 |
      3 | test('bar', () => {
      4 |   expect(bar()).toEqual('bar');

      at Resolver.resolveModule (node_modules/jest-resolve/build/index.js:221:17)
      at Object.<anonymous> (test/bar/index.test.ts:1:1)
```

`@src`가 들어간 유닛테스트는 돌릴 수 없다.
jest가 타입스크립트를 위한 물건이 아니다보니 tsconfig.json을 못알아먹는다.
webpack과 마찬가지로 별도 설정이 필요하다.
webpack때와 마찬가지로 남들이 삽질한거 베끼자.
ts-jest의 [Configuration](https://kulshekhar.github.io/ts-jest/user/config/)을 참고하면 된다.
`moduleNameMapper`를 설정하면 된다.

```json
{
  ...
  "jest": {
    ...
    "moduleNameMapper": {
      "@src/(.*)": "<rootDir>/src/$1"
    }
  }
}
```


[repo-sample]: https://github.com/if1live/libsora.so/tree/master/content/typescript/use-absolute-path-in-typescript
[ts-module-resolution]: https://www.typescriptlang.org/docs/handbook/module-resolution.html
[ts-node]: https://github.com/TypeStrong/ts-node
