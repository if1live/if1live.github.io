---
title: 타입스크립트로 API 명세 작성하고 서버/클라 stub 생성하기
subtitle: Mapped Types와 Type inference in conditional types
tags: [typescript, api specification]
slug: stub-from-typescript-api-specification
author: if1live
date: 2020-10-29
url: /posts/stub-from-typescript-api-specification
---

## 개요

얼마전에 백엔드/프론트엔드로 구성된 운영툴을 짰다.
백엔드는 typescript + express로 굴러간다.
프론트엔드는 typescript + react로 굴러간다.

양쪽을 같은 언어로 구현했으니까 RPC를 싸게 만들 방법이 있지 않을까 생각해서 나온 결과물이다.
요즘 세상에 RPC를 새로 만드는건 바퀴의 재발명같아 보인다.
하지만 다른 RPC를 붙여서 프로젝트를 복잡하게 만들고 싶지 않았다.

타입스크립트로 API 명세를 작성하고 이를 사용해서 HTTP API 서버/클라 구현하는 기법을 정리했다.

## specification

간단한 예제니까 API 명세에는 최소한의 정보만 넣자.
HTTP method, URL, 응답과 출력만 명시한다.

```ts
interface Specification<TReq, TResp> {
  method: 'get' | 'post';
  endpoint: string;
}
```

예제로 간단한 카운터를 만들자.
`POST /inc`로 카운터를 증가시킬 수 있다.
`POST /reset`으로 카운터를 초기화 할 수 있다.

```ts
interface CounterModel {
  id: string;
  value: number;
}

interface Input { id: string; }

const inc: Specification<Input, CounterModel> = {
  method: 'post',
  endpoint: '/inc',
};

const reset: Specification<Input, CounterModel> = {
  method: 'post',
  endpoint: '/reset',
};
```

`inc`, `reset`이 따로 굴러다니면 관리하기 어려우니까 묶자.

```ts
const counterSpec = {
  inc,
  reset,
};
```

`counterSpec`을 사용해서 서버와 클라이언트를 구현한다.

## server

### stub

`counterSpec`을 사용해서 서버에서 사용할 컨트롤러를 만들자.
컨트롤러가 어떻게 생겨야하는지 인터페이스를 생성해야 한다.

[Type inference in conditional types][ts-type-inference]를 사용해서 컨트롤러 함수가 어떻게 생겨야하는지 정의한다.
카운터는 간단한 예제니까 `(body: Req) => Resp` 으로 충분할 것이다.

```ts
type ControllerFunction<T> = T extends Specification<infer Req, infer Resp>
  ? (body: Req) => Resp
  : never;
```

`ControllerFunction`은 API 명세 1개에 대해서만 동작한다.
`counterSpec`는 2개의 API를 포함하고 있다.
[Mapped types][ts-mapped-types]를 사용하면 노가다 없이 여러 API 명세에 맞춰 인터페이스를 찍어낼 수 있다.

```ts
type Controller<T> = { [P in keyof T]: ControllerFunction<T[P]> };
type CounterStub = Controller<typeof counterSpec>;
```

인터페이스를 채워서 컨트롤러를 구현한다.

```ts
class CounterController implements CounterStub {
  private readonly map = new Map<string, number>();

  public inc: CounterStub['inc'] = body => {
    const { id } = body;
    const value = (this.map.get(id) ?? 0) + 1;
    this.map.set(id, value);
    return { id, value };
  }

  public reset: CounterStub['reset'] = body => {
    const { id } = body;
    this.map.set(id, 0);
    return { id, value: 0 };
  }
}
```

### express

컨트롤러로 정의된 핸들러를 express로 연결해야 HTTP 요청으로 사용할 수 있다.
손으로 일일히 입력해서 등록하는건 귀찮으니 API 명세를 우려먹자.

API 명세 1개와 핸들러 함수 1개를 엮어서 express에 등록하는 함수 `registerSpec`를 만든다.

```ts
function registerSpec<Req, Resp>(
  app: express.Application,
  spec: Specification<Req, Resp>,
  handler: ControllerFunction<Specification<Req, Resp>>,
) {
  const { method, endpoint } = spec;
  app[method](endpoint, async (req, res) => {
    const payload = { ...req.query, ...req.body };
    const resp = await handler(payload);
    res.json(resp);
  });
}
```

여러개의 API 명세를 사용해서 `registerSpec`를 호출하는 함수를 만든다.
`any` 사용하지 않는 우아한 방법을 찾지 못했다. (타입시스템은 잘 몰라서 그게 가능한건지 모르겠다)
API 명세와 컨트롤러는 같은 이름의 속성을 갖고있는게 컴파일 타임에 보장된다.
적당히 돌아가니까 생각하는걸 포기하고 이렇게 구현했다.

```ts
function registerController<T>(
  app: express.Application,
  spec: T,
  controller: Controller<T>,
) {
  for (const x of Object.keys(spec)) {
    const key = x as keyof typeof controller;
    const fn = controller[key].bind(controller);
    registerSpec(app, spec[key] as any, fn);
  }
}
```

## client

서버와 똑같이 Type inference in conditional types와 Mapped types를 사용한다.
HTTP 요청을 사용하니까 비동기 함수여야한다. 그래서 `(body: Req) => Promise<Resp>`를 썻다.

```ts
type ClientFunction<T> = T extends Specification<infer Req, infer Resp>
  ? (body: Req) => Promise<Resp>
  : never;
```

```ts
type Client<T> = { [P in keyof T]: ClientFunction<T[P]> };
```

API 명세를 받아서 node-fetch로 요청을 보내는 고차 함수를 만들었다.

```ts
class BaseClient {
  constructor(protected readonly host: string) { }

  protected handle<Req, Resp>(
    spec: Specification<Req, Resp>,
  ): ClientFunction<Specification<Req, Resp>> {
    return async (req) => {
      const { method, endpoint } = spec;
      const url = `${this.host}${endpoint}`;

      const resp = await fetch(url, {
        method: method,
        body: JSON.stringify(req),
        headers: { 'Content-Type': 'application/json' },
      });
      return await resp.json();
    };
  }
}
```

API 명세별로 고차 함수의 리턴을 연결했다.

```ts
class CounterClient extends BaseClient implements Client<typeof counterSpec> {
  public inc = this.handle(counterSpec.inc);
  public reset = this.handle(counterSpec.reset);
}
```

## demo

서버와 클라를 뒤섞어서 간단한 데모를 구현했다.

```ts
const PORT = 3000;
app.listen(PORT, async () => {
  console.log(`listen 127.0.0.1:${PORT}`);

  const client = new CounterClient(`http://127.0.0.1:${PORT}`);
  const id = 'foo';
  console.log('inc', await client.inc({ id }));
  console.log('inc', await client.inc({ id }));
  console.log('inc', await client.reset({ id }));
  console.log('inc', await client.inc({ id }));
  process.exit();
});
```

```
listen 127.0.0.1:3000
inc { id: 'foo', value: 1 }
inc { id: 'foo', value: 2 }
inc { id: 'foo', value: 0 }
inc { id: 'foo', value: 1 }
```

[source](https://github.com/if1live/libsora.so/blob/master/content/typescript/stub-from-typescript-api-specification/src/index.ts)

## 도전과제?

예제를 단순하게 만들기 위해 POST만 사용했다.
다른 Method도 지원하게 개선할 수 있을 것이다.
POST로 json body 사용하는거 말고 query string, path variable을 쓰도록 확장할 수 있다.

예제를 단순하게 만드려고 `express.Application`에 직접 연결했다.
`express.Router`를 사용하면 확장 가능한 형태로 만들 수 있다.

[yup][repo-yup]같은 라이브러리를 사용하면 js 객체가 미리 정의된 스키마에 맞는지 검증할 수 있다.
API 명세에 yup schema를 넣어서 `id`를 최소 2글자, 최대 10글자로 제한할 수 있다.
서버/클라가 같은 언어니까 API 명세에서 정의된 yup schema를 서버/클라에서 재탕할 수 있다.
클라에서는 요청 보내기전에 입력 검증해서 에러 팝업을 띄울수 있을 것이다.
서버에서는 요청을 검증해서 400 BadRequest로 처리할 수 있다.

API 명세를 추상화하면 어떤 형태가 될까?
하나의 X에 서로 다른 규칙을 적용해서 A, B, C, ... 를 찍어낸다.
C에서의 [X-Macro][wiki-x-macro]가 생각나더라.

[ts-type-inference]: https://www.typescriptlang.org/docs/handbook/advanced-types.html#type-inference-in-conditional-types
[ts-mapped-types]: https://www.typescriptlang.org/docs/handbook/advanced-types.html#mapped-types
[repo-yup]: https://github.com/jquense/yup
[wiki-x-macro]: https://en.wikipedia.org/wiki/X_Macro
