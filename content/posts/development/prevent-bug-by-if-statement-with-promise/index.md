---
title: 타입스크립트에서 if문 안에 promise가 들어가는 문제 피하기
subtitle: lint + naming convention
tags: [typescript, lint]
slug: prevent-bug-by-if-statement-with-promise
author: if1live
date: 2018-10-13
url: /posts/prevent-bug-by-if-statement-with-promise
---

## async function + if

옛날 옛적 미개한 자바스크립트에는 콜백뿐이었다.
나중에 promise가 추가되서 좋아졌다.
더 나중에 async/await가 추가되서 더 좋아졌다.
async/await 좋다고 열심히 쓰다가 이번주에 아래와 같은 함정을 밟았다.

```ts
export const isReady = async () => {
  const timeout = new Promise((resolve) => {
    setTimeout(resolve, 100)
  });
  await timeout;
  return false;
};
```

`common.ts`에 있는 `isReady()`는 async 함수이다.
예제 코드니까 적당한 시간 뒤에 false를 반환하도록 구현했다.

```ts
import { isReady } from './common';

const main = async () => {
  if (isReady()) {
    console.log('ready');
  } else {
    console.log('not ready');
  }
};
main();
```

`isReady()`는 `common.ts`의 외부에서 사용된다.

```bash
$ npx ts-node bug.ts
ready
```

코드를 돌리면 `ready`가 출력된다.
async/await의 리턴타입은 Promise이다.
그리고 Promise는 if문 안에서 true로 취급된다.

## fix bug

버그를 고치는 방법은 간단하다.
`isReady()`를 호출할때 await를 붙여주면된다.

```ts
import { isReady } from './common';

const main = async () => {
  const ready = await isReady();
  if (ready) {
    console.log('ready');
  } else {
    console.log('not ready');
  }
};
main();
```

```bash
$ npx ts-node fixed.ts
not ready
```

위의 문제는 원인도 간단하고 해결하는 방법도 쉽다.
하지만 자주 밟을 가능성이 있다.
프로젝트의 규모가 커지면 함수의 리턴타입을 전부 기억하기 어려워진다.
`isXXX`같은 함수를 보고 boolean 리턴한다고 생각하는게 보통이다.
`Promise<boolean>`을 리턴한다고 생각하지 않을 수 있다.

## 버그 방지

버그를 고친다는건 이미 문제가 생겼다는 뜻이다.
더 좋은것은 처음부터 문제가 생기지 않도록 하는 것이다.

### lint

lint의 수준을 올려서 if문 안에 boolean만 들어갈수 있도록 하자.
그러면 if문 안에 Promise가 들어갈 경우 lint에서 경고할거다.

tslint에는 [strict-boolean-expression][tslint-rule]라는 규칙이 있다.
tslint.json에 `"strict-boolean-expressions": true`를 추가해서 이를 활성화 시켜보자.

> $ npx tslint --project tsconfig.json -t verbose
>
> ERROR: (strict-boolean-expressions) D:/blog/libsora.so/src/libsora.so/content/development/prevent-bug-by-if-and-promise/bug.ts[4, 7]: This type is not allowed in the 'if' condition because it is always truthy. Only booleans are allowed.
>
>
>
>


### naming convention

[Node.js File System][node-fs] 를 뒤져보면 아래같이 생긴 함수가 많이 보인다.

* `fs.writeFile(file, data[, options], callback)`
* `fs.writeFileSync(file, data[, options])`

nodejs에는 같은 기능을 동기로 구현한 함수도 있고 비동기로 구현한 함수도 있다.
동기로 구현된 경우 비동기 함수 이름 뒤에 `Sync`가 붙는다.

헝가리안 표기법의 연장선으로 생각해서 코딩을 하자.
함수 이름에 적당한 prefix/postfix를 붙여서 promise를 리턴한다고 명시하는 것이다.
그러면 함수의 선언을 보지 않고도 리턴 타입을 알 수 있을것이다.

```typescript
if(await isReadyAsync()) {
    ...
}
```

## 내가 선택한 길

* tslint에서 strict-boolean-expression rule를 활성화
* CI 스크립트에 tslint 추가. lint를 통과하지 못하면 CI에서 빌드가 실패한다.
* 함수 이름에 async붙이는건 이미 구현한게 많아서 귀찮다보니 생략



[tslint-rule]: https://palantir.github.io/tslint/rules/strict-boolean-expressions/
[node-fs]: https://nodejs.org/api/fs.html
