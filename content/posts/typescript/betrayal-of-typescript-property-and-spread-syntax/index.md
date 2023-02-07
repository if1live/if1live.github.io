---
title: 타입스크립트의 배신 - property와 spread syntax
subtitle: class + property + spread = FAIL
tags: [typescript, javascript, class, property, spread syntax]
slug: betrayal-of-typescript-property-and-spread-syntax
author: if1live
date: 2023-02-07
url: /posts/betrayal-of-typescript-property-and-spread-syntax
---

## 준비물

### property
자바스크립트의 [class][mdn-class]로 객체의 getter, setter 프로퍼티를 만들수 있다.

```js
class Rectangle {
  constructor(height, width) {
    this.height = height;
    this.width = width;
  }

  // Getter
  get area() {
    return this.calcArea();
  }
}
```

### spread syntax

자바스크립트에는 [전개 구문, spread syntax][mdn-spread-syntax]가 있다.
spread syntax로 객체를 쉽게 복제할 수 있다.

```js
let objClone = { ...obj };
```

## 버그

컴파일이 잘 되는 간단한 타입스크립트 코드를 작성했다.

```ts
interface X {
    v: number;
}

class Y implements X {
    public get v() { return 1; }
}

const a: X = new Y();
const b: X = { ...a };
const c: X = { v: a.v };

console.log('a', a.v);
console.log('b', b.v);
console.log('c', c.v);
```

인터페이스 X와 이를 구현한 Y를 준비한다.
인터페이스 X에는 `v` 필드가 정의되어있다.

`a`는 클래스에서 만들어진 객체이다.
`b`는 `a`에 spread syntax를 적용해서 만들어진 새로운 객체이다.
`c`는 spread syntax 없이 X에 정의된 필드 각각을 손으로 연결해서 만든 객체이다.
`a`, `b`, `c`는 인터페이스 X를 구현했으니까 `v` 로 접근할 수 있다.

예상되는 출력은 다음과 같다.

```
[LOG]: "a",  1 
[LOG]: "b",  1
[LOG]: "c",  1 
```

예상한 출력과 실제 결과가 같았으면 글을 쓰지 않았을거다.
실행 결과는 다음과 같다.

```
[LOG]: "a",  1 
[LOG]: "b",  undefined 
[LOG]: "c",  1 
```

https://www.typescriptlang.org/play?target=9&module=0#code/JYOwLgpgTgZghgYwgAgBrIN4Chm+QNwC5kQBXAWwCNoBuLAXyywQBs4Bnd5ATWWHIAOLCOQjgu6bHmQDSlFsATIA5hDAEAFAEpMyKGtJQQyAIw1kjRswD2IdurjF0AXhIQA7j210Et+8konZFcMZAA6CLgLHz91BCCQgmI4MPxopl87a2EwlmtlDQByOEKAGmQU-C0YrJy8gsLKMoDU6ptaiFz8ooRmhFaaIA

`b.v`가 `undefined`가 되었다.
컴파일에는 문제가 없었지만 런타임에는 의도한 결과가 나오지 않는다.

더 자세하게 뜯어보기 위해 객체 자체도 로그로 찍어보자.

```ts
interface X {
    v: number;
}

class Y implements X {
    public get v() { return 1; }
}

const a: X = new Y();
const b: X = { ...a };
const c: X = { v: a.v };

console.log('a', a);
console.log('a.v', a.v);

console.log('b', b);
console.log('b.v', b.v);

console.log('c', c);
console.log('c.v', c.v);
```

https://www.typescriptlang.org/play?target=9&module=0#code/JYOwLgpgTgZghgYwgAgBrIN4Chm+QNwC5kQBXAWwCNoBuLAXyywQBs4Bnd5ATWWHIAOLCOQjgu6bHmQDSlFsATIA5hDAEAFAEpMyKGtJQQyAIw1kjRswD2IdurjF0AXhIQA7j210Et+8konZFcMZAA6CLgLHz91BCCQgmI4MPxopl87a2EwlmtlDQByOEKAGmQ4LRisnLyC4tSyitSqjL9siFz8osomylbM9g6u+spG8rH8VpsazrqihCaEAfba7sKEceRNqbogA

```
[LOG]: "a",  Y: {} 
[LOG]: "a.v",  1 
[LOG]: "b",  {} 
[LOG]: "b.v",  undefined 
[LOG]: "c",  {
  "v": 1
} 
[LOG]: "c.v",  1
```

`a`는 클래스에서 만들어진 객체라서 특별하다. property인 `v`가 콘솔 로그에는 보이지 않지만 `v`로 접근할 수 있다.
`b`는 비어있는 자바스크립트 객체이다. spread syntax로 만들었기떄문에 클래스 Y와는 관계가 없다.
`c`는 직접 만든 자바스크립트 객체이다. 원한대로 잘 돌아간다.

## summary

타입스크립트 컴파일을 통과해도 런타임에서 터질 수 있다.

일반 자바스크립트 객체에만 spread syntax를 적용하자.
클래스로 만들어진 객체에와 spread syntax를 같이 쓰면 프로퍼티가 사고칠 수 있다.

[mdn-spread-syntax]: https://developer.mozilla.org/ko/docs/Web/JavaScript/Reference/Operators/Spread_syntax

[mdn-class]: https://developer.mozilla.org/ko/docs/Web/JavaScript/Reference/Classes#%ED%94%84%EB%A1%9C%ED%86%A0%ED%83%80%EC%9E%85_%EB%A9%94%EC%84%9C%EB%93%9C
