---
title: 타입스크립트와 순환 의존성
subtitle: "TypeError: class_X is not a constructor"
tags: [typescript, circular dependency, circular reference]
slug: typescript-and-circular-dependency
author: if1live
date: 2019-12-28
url: /posts/typescript-and-circular-dependency
---

## 타입스크립트와 순환 의존성

circular dependency(순환 의존성, circular reference, 순환 참조)는 대부분의 언어에서 발생시킬 수 있는 문제이다.
C 에서도 circular dependency 문제가 있다.
C 에서는 전방선언 같은 수단을 쓰지 않으면 컴파일이 되지 않기 때문에 문제를 쉽게 찾을수 있다.

타입스크립트 역시 순환 의존성 문제가 발생할 수 있다.
하지만 타입스크립트에서의 순환 의존성 문제는 C와는 조금 다르다.

## undefined

순환 의존성을 구현한 간단한 예제이다.

const-a.ts

```ts
import { b } from './const-b';
console.log({ b });

export const a: number = b + 1;
```

const-b.ts

```ts
import { a } from './const-a';
console.log({ a })

export const b: number = a + 1;
```

index.ts

```ts
import { a } from './const-a';

console.log(`result: ${a}`);
```

```
{ a: undefined }
{ b: NaN }
result: NaN
```

`const-a.ts`는 `const-b.ts`가 필요하고 `const-b.ts`는 `const-a.ts`가 필요하다.
순환 참조가 발생했을때 import한 값은 `undefined`가 된다.
의도하지 않게 undefined가 튀어나왔으니 결과가 이상해진다.

C에서 순환 의존성 문제가 발생하면 컴파일이 실패한다. 문제를 즉시 찾을 수 있다.
자바스크립트에서 순환 의존성 문제가 발생하면 런타임에서 터진다.
스크립트 언어는 런타임에서 터지는게 일상이니까 이상한게 아니다.

골치 아픈건 타입스크립트에서의 순환 의존성이다.
컴파일은 통과했으니 잘 돌아갈거라고 생각한 코드지만 런타임에 터지는 것이다.

## 순환 의존성과 class: 문제 있는 예제

class-x.ts


```ts
import { Y } from './class-y';

export class X {
  public static sample() { return new Y(); }
}
```

class-y.ts


```ts
import { X } from './class-x';

console.log({ X });
const x = new X();

export class Y {
  public static sample() { return x; }
}
```

index.ts
```ts
import { X } from './class-x';
console.log(X);
```

실행 결과

```
{ X: undefined }
<my-path>\class-y.js:4
var x = new class_x_1.X();
        ^

TypeError: class_x_1.X is not a constructor
    at Object.<anonymous> (<my-path>\class-y.js:4:9)
    at Module._compile (internal/modules/cjs/loader.js:778:30)
    at Object.Module._extensions..js (internal/modules/cjs/loader.js:789:10)
    at Module.load (internal/modules/cjs/loader.js:653:32)
    at tryModuleLoad (internal/modules/cjs/loader.js:593:12)
    at Function.Module._load (internal/modules/cjs/loader.js:585:3)
    at Module.require (internal/modules/cjs/loader.js:692:17)
    at require (internal/modules/cjs/helpers.js:25:18)
    at Object.<anonymous> (<my-path>\class-x.js:3:17)
    at Module._compile (internal/modules/cjs/loader.js:778:30)
```

`TypeError: XXXX is not a constructor`. 내가 몇 번 밟아본 버그이다.
에러 메세지에 undefined가 들어있으면 쉽게 깨달을수 있는데 현실은 그렇지 않다.
클래스의 생성자가 이상하다는 에러 메세지를 보고 클래스를 찍어보면 undefined가 나온다.
순환 의존성 때문에 초기화가 안된 상태에서 객체를 생성하니까 문제가 생기는 것이다.

## 순환 의존성과 class: 문제 없는 예제

class-a.ts

```ts
import { B } from './class-b';
console.log(`import B: ${B}`);

export class A {
  public sample() {
    console.log(`function B: ${B}`);
    return new B();
  }
}
```

class-b.ts

```ts
import { A } from './class-a';
console.log(`import A: ${A}`);

export class B {
  public sample() {
    console.log(`function A: ${A}`);
    return new A();
  }
}
```

index.ts
```ts
import { A } from './class-a';
import { B } from './class-b';

console.log({ A });
console.log({ B });

const a = new A();
const b = new B();

b.sample();
a.sample();
```

실행 결과

```
import A: undefined
import B: function B() {
    }
{ A: [Function: A] }
{ B: [Function: B] }
function A: function A() {
    }
function B: function B() {
    }
```

import한 직후에 A가 undefined로 뜨는 것은 문제가 있는 예제와 마찬가지이다.
하지만 함수 안에서 객체를 생성할 시점에는 A가 더이상 undefined가 아니다.

컴파일된 결과물 `class-b.js`를 보면 이해할 수 있다.

```js
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var class_a_1 = require("./class-a");
console.log("import A: " + class_a_1.A);
var B = /** @class */ (function () {
    function B() {
    }
    B.prototype.sample = function () {
        console.log("function A: " + class_a_1.A);
        return new class_a_1.A();
    };
    return B;
}());
exports.B = B;
```

`class_a_1.A`를 변수에 할당해두고 쓰지 않는다.
`class_a_1`가 초기화되기 전에 쓰면 `class_a_1.A`가 undefined가 되는 것이고 그렇지 않으면 올바른 값이 되는 것이다.


## 순환 의존성은 피할수 있는가?

[typeorm][repo-typeorm]이라는 ORM을 사용 중이다.
멀쩡한 ORM 답게 [many-to-one](https://github.com/typeorm/typeorm/blob/master/docs/many-to-one-one-to-many-relations.md)을 지원한다.
many-to-one 예제 코드를 보면 이런게 튀어나온다.

Photo.ts

```ts
import { Entity, PrimaryGeneratedColumn, ManyToOne } from 'typeorm';
import { User } from './User';

@Entity()
export class Photo {
    @PrimaryGeneratedColumn()
    id: number;

    @ManyToOne(type => User, user => user.photos)
    user: User; 
}
```

User.ts

```ts
import { Entity, PrimaryGeneratedColumn,OneToMany } from 'typeorm';
import { Photo } from './Photo';

@Entity()
export class User {
    @PrimaryGeneratedColumn()
    id: number;

    @OneToMany(type => Photo, photo => photo.user)
    photos: Photo[];
}
```

`Photo.user`는 `User` 타입으로 되어있으며 `User.photos`는 `Photo[]` 타입으로 되어있다.
`User`와 `Photo`는 서로 의존하고 있다.
순환 의존성을 없애려면 둘을 같은 파일에 넣으면 된다.
샘플은 Entity별로 독립된 파일로 만들고 싶었는지 그런 선택을 하지 않았다.

설계를 잘 하고 마음에 들지 않는 코드를 참을 수 있다면 순환 의존성은 피할 수 있다.
나는 참을성이 없어서 순환 의존성을 피하지 못했다.
각각의 Entity는 자신만의 파일을 갖게 만들고 싶었다.
0개의 순환 의존성은 나한테는 불가능한 목표이다.

## 순환 의존성 찾기: lint

처음에 사용한 도구는 [tslint-no-circular-imports][repo-tslint-no-circular-imports]이다.
순환 의존성은 잘 찾아낸다. 하지만 무시하고 싶은 순환 의존성만 걸러낼 수 있는 옵션이 없더라.

방치해도 되는 상태의 경고가 남아있는 상태로 개발을 진행하면 나중에 어떤 문제가 발생하는가?
워닝이 일상이 되면 아무도 워닝을 주목하지 않는다.
진짜 중요한 워닝이 발생했을때 무시되고 사고가 터진다.

몇번 당해봤기때문에 tslint-no-circular-imports를 사용하지 않았다.

## 순환 의존성 찾기: webpack

지금은 [circular-dependency-plugin][repo-circular-dependency-plugin]를 사용하고 있다.
webpack은 이미 사용하고 있고 tslint-no-circular-import보다 지원하는 설정이 많더라.
설정을 손봐서 typeorm entity간의 순환 참조는 무시하도록 해놨다.

```js
new CircularDependencyPlugin({
    // 대부분의 필드는 기본값 사용

    onDetected({ module: webpackModuleRecord, paths, compilation }) {
        // entity에서 x-to-x 관계를 본격적으로 쓰면 cycle를 피하기 어렵다
        // ClanEntity -> ClanMemberEntity -> ClanEntity
        // 그래서 entity간의 사이클은 무시
        const isEntityCycle = paths.map(x => x.endsWith('Entity.ts')).reduce((a, b) => a && b, true);
        if (isEntityCycle) { return; }

        compilation.errors.push(new Error(paths.join(' -> ')))
    },
}),
```

[repo-tslint-no-circular-imports]: https://github.com/bcherny/tslint-no-circular-imports#readme
[repo-circular-dependency-plugin]: https://github.com/aackerman/circular-dependency-plugin
[repo-typeorm]: https://github.com/typeorm/typeorm
