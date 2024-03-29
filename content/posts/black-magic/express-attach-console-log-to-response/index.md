---
title: express에서 콘솔로 찍은 로그를 HTTP 응답에 붙이기
subtitle: continuation local storage
tags: [express, logging, monkey patch, cls-hooked]
slug: express-attach-console-log-to-response
author: if1live
date: 2020-07-11T00:00:00+09:00
url: /posts/express-attach-console-log-to-response
---

## HTTP 응답에 콘솔 로그 붙인 이유

간단한 express 서버가 있다고 치자.

```js
const express = require('express');
const delay = require('delay');

const app = express();

app.get('/', async (req, res) => {
  const data = await execute(req.query || {});
  res.json(data);
});

async function execute(input) {
  const id = input.id;
  console.info(`before delay: ${id}`);
  await delay(100);
  console.warn(`after delay: ${id}`);
  return { id };
}

const port = 3000;
app.listen(port, () => console.log(`listen: 127.0.0.1:${port}`));
```

`http://127.0.0.1:3000/?id=1`로 접속하면 아래와 같은 JSON을 응답으로 받는다.

```json
{
  "id":"1"
}
```

동시에 콘솔에는 아래와 같은 로그가 찍힐 것이다.

```
before delay: 123
after delay: 123
```

로컬 환경에서는 내 컴퓨터에 서버를 띄워두고 내 컴퓨터에서 접속하니까 응답과 로그를 동시에 보기 쉽다.
하지만 로컬 환경을 탈출하는 순간부터는 로그 보는게 쉽지 않다.

127.0.0.1이 아닌 곳에서 docker같은 것으로 돌아가고 있을테니까 로그를 보기 위해서 터미널이 띄워야 한다.
aws lambda로 굴러가는 경우에는 cloudwatch에 로그가 쌓이는데 실시간 로그가 아니다.
API 호출하고 몇초에서 몇십초가 지나야 cloudwatch에서 보인다.
게다가 서버를 나 혼자 쓴다는 보장도 없다.
지금 호출한 API에서 발생한 로그 이외의 다른 로그가 뒤섞여있다.
내가 로그를 뒤지는 동안에도 로그는 더 쌓일 것이다.

REST API에서 발생한 로그를 HTTP 응답에 붙여서 던지면 편하지 않을까?
API 응답과 콘솔 로그를 합쳐서 아래와 같은 형태로 만들어보자.

```json
{
  "id":"1",
  "_logs":[
    "[INFO] before delay: 1",
    "[WARN] after delay: 1"
  ]
}
```

## cls-hooked

django 쓸때는 [django-threadlocals](https://pypi.org/project/django-threadlocals/)같은거 사용해서 원하는 기능을 만들었다.
node.js에서는 다른 방법을 써야한다.
thread-local storage 대신 continuation-local storage로 찾으면 몇개 나온다.
[cls-hooked][repo-cls-hooked]를 사용하기로 했다.

최신 node.js에서는 [AsyncLocalStorage](https://nodejs.org/api/async_hooks.html#async_hooks_class_asynclocalstorage)라는게 있다더라.
[aws lambda][docs-aws-lambda]는 아직 node 12.x까지만 된다.
node.js 12.17에서는 AsyncLocalStorage를 쓸 수 있지만 aws lambda에서의 버전은 12.17보다 높을 수도 있고 낮을 수도 있다.
괜히 12.x라고 적어둔게 아니더라.

cls-hooked는 남들이 짜둔 코드를 보고 익혔다.
[cls-rtracer 1.x][repo-cls-rtracer]와 [express-http-context][repo-express-http-context]를 베끼면 원하는걸 만들 수 있겠더라.

## 의식의 흐름

콘솔 로그를 쌓아둘 함수를 만든다.
[express-http-context][repo-express-http-context]를 베끼면 된다.

```js
const NSID = 'writer';
const KEY_LOGS = 'logs';

function set(key, val) {
  const ns = getNamespace(NSID);
  if (ns && ns.active) { ns.set(key, val); }
}

function get(key) {
  const ns = getNamespace(NSID);
  return (ns && ns.active) ? ns.get(key) : undefined;
}
```

express middleware를 적당히 만들어서 context를 생성한다.
middleware 이후에 나오는 코드에서 CLS를 쓸 수 있다
[cls-rtracer 1.x][repo-cls-rtracer]를 베끼면 된다.

```js
app.use((req, res, next) => {
  const ns = getNamespace(NSID) || createNamespace(NSID);
  ns.bindEmitter(req);
  ns.bindEmitter(res);

  ns.run(() => next());
});
```

express에서는 `res.json()`을 두 번 호출할 수 없다.
로그를 끼워넣은 후 진짜 `res.json()`를 호출하는 함수로 교체한다.

```js
const default_json = res.json.bind(res);
res.json = (data) => default_json({ ...data, _logs: get(KEY_LOGS) });
```

콘솔 로그 함수를 새거로 바꾼다.
CLS로 로그를 쌓고 콘솔에도 로그를 찍는다.

```js
const propertyNames = ['info', 'warn', 'error']
for (const property of propertyNames) {
  const default_fn = console[property].bind(console);
  console[property] = (message) => {
    const logs = get(KEY_LOGS) || [];
    logs.push(`[${property.toUpperCase()}] ${message}`);
    set(KEY_LOGS, logs);

    default_fn(message);
  };
}
```

전부 이어붙이면 아래와 같은 결과를 볼 수 있다.

```
$ curl -s "http://127.0.0.1:3000/?id=1" -w '\n' & curl -s "http://127.0.0.1:3000/?id=2" -w '\n' &
[1] 6454
[2] 6455

{"id":"1","_logs":["[INFO] before delay: 1","[WARN] after delay: 1"]}
{"id":"2","_logs":["[INFO] before delay: 2","[WARN] after delay: 2"]}
```

## 응용

삽질을 더 하면 `console.info()`보다 더 다양한 것을 로깅할 수 있다.
[몽키 패치를 사용해서 ioredis 로깅][ioredis-logging-with-monkey-patch]하는 방법을 정리한건 빅-픽처의 일부였다.

API에서 발생한 SQL, redis, fetch도 로그로 남기면 언젠가 쓸모있을 것이다.
typeorm logger에 CLS 로깅을 붙여서 `repository.save()`가 쿼리를 생각보다 많이 발생시킨다는걸 확인했다.
[typeorm으로 삽질한 글][typeorm-entity-proxy-for-save]은 CLS 로깅 덕분에 나온 것이다.

## code

```js
/**
 * test
 * curl "http://127.0.0.1:3000/?id=1" & curl "http://127.0.0.1:3000/?id=2"
 */

const express = require('express');
const delay = require('delay');
const { getNamespace, createNamespace } = require('cls-hooked');

const NSID = 'writer';
const KEY_LOGS = 'logs';

function set(key, val) {
  const ns = getNamespace(NSID);
  if (ns && ns.active) { ns.set(key, val); }
}

function get(key) {
  const ns = getNamespace(NSID);
  return (ns && ns.active) ? ns.get(key) : undefined;
}

function monkeypatch_console() {
  const propertyNames = ['info', 'warn', 'error']
  for (const property of propertyNames) {
    const default_fn = console[property].bind(console);
    console[property] = (message) => {
      const logs = get(KEY_LOGS) || [];
      logs.push(`[${property.toUpperCase()}] ${message}`);
      set(KEY_LOGS, logs);

      default_fn(message);
    };
  }
}
monkeypatch_console();

const app = express();

app.use((req, res, next) => {
  const default_json = res.json.bind(res);
  res.json = (data) => default_json({ ...data, _logs: get(KEY_LOGS) });

  const ns = getNamespace(NSID) || createNamespace(NSID);
  ns.bindEmitter(req);
  ns.bindEmitter(res);

  ns.run(() => next());
});

app.get('/', async (req, res) => {
  const data = await execute(req.query || {});
  res.json(data);
});

async function execute(input) {
  const id = input.id;
  console.info(`before delay: ${id}`);
  await delay(100);
  console.warn(`after delay: ${id}`);
  return { id };
}

const port = 3000;
app.listen(port, () => console.log(`listen: 127.0.0.1:${port}`));
```

[repo-cls-hooked]: https://www.npmjs.com/package/cls-hooked
[repo-cls-rtracer]: https://github.com/puzpuzpuz/cls-rtracer/blob/1.4.1/index.js#L18
[repo-express-http-context]: https://github.com/skonves/express-http-context/blob/v1.2.3/index.js

[docs-aws-lambda]: https://docs.aws.amazon.com/lambda/latest/dg/lambda-runtimes.html

[ioredis-logging-with-monkey-patch]: {{< ref ioredis-logging-with-monkey-patch >}}
[typeorm-entity-proxy-for-save]: {{< ref typeorm-entity-proxy-for-save >}}
