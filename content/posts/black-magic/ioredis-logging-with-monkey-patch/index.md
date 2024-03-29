---
title: ioredis에 로깅 끼워넣기
subtitle: 라이브러리 코드 수정 없이 라이브러리 개조하기
tags: [ioredis, monkey patch]
slug: ioredis-logging-with-monkey-patch
author: if1live
date: 2020-07-09
url: /posts/ioredis-logging-with-monkey-patch
---

## 로깅의 필요성

요새 작업하고 있는 프로젝트에서 [typeorm][github-typeorm]과 [ioredis][github-ioredis]를 쓰고 있다.
기능이 검증된 코드를 짜기 위해서 유닛 테스트를 도배하고 있다.
하지만 유닛 테스트로는 성능을 확인할 순 없다.
redis MSET 한번으로 될 일을 SET 100번 호출하고 있더라도 유닛테스트는 통과할 것이다.

최적화라는 빅-픽쳐를 위해 서버가 redis, rdbms로 요청 보낼때 로그를 남기기로 했다.
지정된 규격으로 로그를 찍으면 CloudWatch Logs를 통해서 elasticsearch에 저장되도록 만들고 싶다.

## typeorm: custom logger

typeorm은 로깅 갈아끼우는걸 기본적으로 지원한다.
[custom logger][typeorm-logging]를 쓰면 된다고 한다.
문서에서 시키는대로 하면 잘 돌아간다.

```ts
export class SimpleConsoleLogger implements Logger {
  logQuery(query: string, parameters?: any[], queryRunner?: QueryRunner) {
    logger.info({ty: 'typeorm', query});
  }
  ...
}
```

## ioredis (v4.17.3): debug

ioredis는 typeorm만큼 자유롭지 않다.
ioredis는 [debug][npm-debug]를 쓴다.
https://github.com/luin/ioredis/blob/v4.17.3/lib/utils/debug.ts#L1

환경변수에 `DEBUG=ioredis:*`를 넣으면 로그를 볼 수 있다.
![ioredis logging](ioredis-debug.png)

ioredis 코드에 정의된 디버그 로그가 찍히기 때문에 마음대로 커스터마이징 할 수 없다.
https://github.com/luin/ioredis/blob/v4.17.3/lib/redis/index.ts#L704

이럴때는 ioredis는 fork해서 custom logger를 구현하면 된다.
잘 구현해서 풀리퀘를 보내면 ioredis에서 머지해줄테고 많은 사람들이 이득을 본다.
이래서 오픈소스가 좋은 것이다.

하지만 나는 게으르다.
풀리퀘를 만드는 것도 귀찮고 이슈에서 영어로 토론하기도 귀찮다.
ioredis 안고치고 로깅 기능을 추가하기로 했다.

## monkey patch

ioredis를 보니까 redis 명령은 `sendCommand`를 거쳐가더라.
https://github.com/luin/ioredis/blob/v4.17.3/lib/redis/index.ts#L650

`sendCommand`를 고쳐서 로깅을 추가하면 원하는 형태로 만들수 있지 않을까?
[monkey patch][wiki-monkey-patch]라는 좋은 기법이 있다.

```js
const default_sendCommand = redis.sendCommand.bind(redis);
  redis.sendCommand = async (...params) => {
    ...
    default_sendCommand(...params);
    ...
  }
}
```

![ioredis logging](ioredis-logging.png)

왼쪽은 터미널에서 실행한 결과이고 오른쪽은 `redis-cli monitor`이다.
몽키 패치가 잘 돌아간다.
C, C++ 같은 언어였으면 훨씬 복잡했을텐데 자바스크립트라서 간단하게 문제를 해결했다.

```javascript
const Redis = require('ioredis');

function makeRedis() {
  const redis = new Redis({ lazyConnect: true });

  const default_sendCommand = redis.sendCommand.bind(redis);
  redis.sendCommand = async (...params) => {
    const command = params[0];
    const { name, args } = command;

    const start = Date.now();
    const result = await default_sendCommand(...params);
    const end = Date.now()

    const elapsed = end - start;
    console.log(`redis: ${name} ${args.join(' ')} [${elapsed}ms]`);

    return result;
  };

  return redis;
}

async function main() {
  const redis = makeRedis();
  await redis.connect();

  await redis.mset('foo', 1, 'bar', 2);
  console.log(await redis.get('foo'));
  await redis.del('foo', 'bar');
  console.log(await redis.get('bar'));
}

main().then(process.exit).catch(console.error);
```

[npm-debug]: https://www.npmjs.com/package/debug
[github-typeorm]: https://github.com/typeorm/typeorm
[github-ioredis]: https://github.com/luin/ioredis
[typeorm-logging]: https://typeorm.io/#/logging/using-custom-logger
[wiki-monkey-patch]: https://en.wikipedia.org/wiki/Monkey_patch
