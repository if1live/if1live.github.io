---
title: lodash 탈출기 - remeda의 틈을 막아라
subtitle: just - dependency-free JavaScript utilities
tags: [typescript, lodash, just]
slug: escape-from-lodash-just
author: if1live
date: 2023-06-17T00:02:00+09:00
url: /posts/escape-from-lodash-just
---

## 개요

이전 글에서 lodash를 remeda로 교체한 이야기를 했다.
그리고 remeda가 lodash를 100% 대체할 수 없다는 것도 언급했다.
이번 글에서는 remeda로 대체하지 못한 영역을 막을 라이브러리를 다룬다.

## 요구사항

`uniqBy`, `sumBy`, `intersection`, `pipe`, ... 많은 함수가 이미 remeda에 있다.
remeda 정도 되는 크기의 라이브러리 X를 또 쓰고 싶진 않다.
`X.uniqBy`를 써야할지 `R.uniqBy`를 써야할지 고민하는건 말이 안된다.
remeda에 없는 함수만 선택적으로 쓰고 싶다.

remeda는 `_.shuffle`, `_.random`에 대응되는 함수가 없다.
(한달전만 해도 `_.isEmpty`, `_.sample`도 없었는데 추가되었더라)
lodash의 다양한 함수를 많이 썻다면 빈곳이 더 많을지도?

간단한 함수는 직접 구현해도 되지만 left-pad를 보고 배우자.
내가 짠 코드에서 버그가 생기면 내 책임이다.
고작 11줄밖에 안되는 함수라고 해도 검증된 오픈소스를 갖다쓰자.

![dependency](dependency.jpg)

## 후보: [just][github-just]

> A library of dependency-free JavaScript utilities that do just one thing.

내가 원하는 수준의 라이브러리를 한번에 찾아냈다.
just는 함수 하나가 패키지 하나로 구성되어있다.

```ts
// pnpm install just-shuffle
import shuffle from 'just-shuffle';
shuffle([1, 2, 3]);
```

## 적용

`import shuffle from 'just-shuffle'` 같은 형태로 매번 import해서 쓰고싶진 않았다.
named export를 해서 자동 완성이 잘되게 하고 싶었다.
적당한 파일을 만들고 내가 사용하려는 just 함수만 묶어서 다시 export했다.

```ts
// just.ts
import randomIntFn from 'just-random-integer';
import shuffleFn from 'just-shuffle';

export const randomInt = randomIntFn;
export const shuffle = shuffleFn;

// usage
export * as J from './just';
export { randomInt, shuffle } from './just';
```

`import * as R from 'remeda'` 랑 비슷한 느낌으로 쓸 수 있게 해봤다.

## 마무리

3편의 글로 lodash 탈출하는 이야기를 해봤다.

첫번째 글에선 lodash가 없던 시절의 옛날 이야기와 lodash에서 탈출하고 싶은 이야기를 다뤘다.
두번째 글에선 lodash의 대안으로 여러가지 라이브러리를 찾아보고 remeda를 선택했다.
세번째 글에선 remeda의 빈틈을 메꿀 just를 써봤다.

이 정도 정보면 누구나 큰 문제 없이 lodash에서 탈출할 수 있을 것이다.

Good Luck!

![Drowning High Five](highfive.jpg)

## 연결
* [lodash 탈출기 - 왜 lodash에서 벗어나는가?]({{< ref escape-from-lodash-why-i-escape >}})
* [lodash 탈출기 - lodash의 대안과 결정]({{< ref escape-from-lodash-remeda >}})
* [lodash 탈출기 - remeda의 틈을 막아라]({{< ref escape-from-lodash-just >}})

[github-just]: https://github.com/angus-c/just
