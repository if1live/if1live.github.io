---
title: callbackWaitsForEmptyEventLoop와 AWS Lambda의 함정
subtitle: 즐거운 serverless의 세계
tags: [aws lambda, serverless]
slug: trap-callback-waits-for-empty-event-loop
author: if1live
date: 2020-10-30
url: /posts/trap-callback-waits-for-empty-event-loop
---

## 개요

요새 typescript + express 백엔드를 짠다.
운영에 대해서는 조금도 생각하기 싫어서 AWS Lambda로 배포한다.
[serverless-http][serverless-http]라는 좋은 물건을 쓰면 express를 AWS Lambda에서 돌릴 수 있다.
express의 생산성을 유지하면서 AWS Lambda의 안정적인 운영을 챙길 수 있으니 좋은 구성이다.
하지만 개발 환경과 배포 환경이 근본적으로 달라서 여러가지 함정을 밟고 있다.
인상적이었던 함정 하나를 정리했다.

## sentry 전송 실패

그냥 express를 사용하는 경우 [sentry][sentry] 에러 전송은 생각할 필요없다.
서버가 데몬처럼 돌아가니까 에러 리포트가 잘 보내진다.
하지만 AWS Lambda는 데몬이 아니다.
[옛날 sentry AWS Lambda 연동 문서][doc-sentry-serverless]를 보면 명시적으로 보낸다.

```ts
function sentryHandler(lambdaHandler) {
  return async event => {
    try {
      return await lambdaHandler(event);
    } catch (error) {
      Sentry.captureException(error);
      await Sentry.flush(2000);
      return error;
    }
  };
}
```

sentry에는 에러 리포팅만 있는게 아니다.
`Sentry.captureMessage()`는 에러가 아닌 일반 메세지를 보내는 목적으로 쓰인다.
`Sentry.flush()`는 핸들러의 성공, 실패와 관계없이 항상 호출되는게 합리적이다.
try...catch...finally라는 좋은 문법을 쓰면 성공, 실패와 관계없이 항상 호출되는 코드를 짤 수 있다.

```ts
try {
  return handle();

} catch (error) {
  Sentry.captureException(error);
  throw error;

} finally {
  await Sentry.flush(2000);
}
```

하지만 위의 코드를 AWS Lambda 위에 올리니 돌아가지 않았다!
에러가 발생했음에도 sentry에는 아무것도 도착하지 않았다.
finally를 지우고 `Sentry.flush()`를 try, catch에 따로 넣으면 에러 리포팅이 작동했다.

```ts
try {
  const resp = handle();
  await Sentry.flush(2000);
  return resp;

} catch (error) {
  Sentry.captureException(error);
  await Sentry.flush(2000);
  throw error;
}
```

finally가 실행되지 않을때가 있다니 이게 무슨 개소리야?

## 간단한 예제

[AWS Lambda console](https://console.aws.amazon.com/lambda/home)에서 돌릴 수 있는 간단한 코드를 짰다.
try...catch...finally 문법을 사용했다.
비동기 핸들러를 사용했고 finally 안에서 비동기 함수를 호출했다.

```js
const delay = (millis) => new Promise(resolve => setTimeout(resolve, millis));

exports.handler = async (event, context) => {
  try {
    return { statusCode: 200, body: 'a' };
  } catch (e) {
    return { statusCode: 200, body: 'b' };

  } finally {
    console.log('finally 1');
    await delay(1);
    console.log('finally 2');
  }
};
```

코드를 실행시키면 아래와 같은 응답을 볼 수 있다.
```
Response:
{
  "statusCode": 200,
  "body": "a"
}

Request ID:
"d9da2098-c9e6-437d-9fd2-315576f153b9"

Function logs:
START RequestId: d9da2098-c9e6-437d-9fd2-315576f153b9 Version: $LATEST
2020-10-30T13:07:15.582Z	d9da2098-c9e6-437d-9fd2-315576f153b9	INFO	try
2020-10-30T13:07:15.600Z	d9da2098-c9e6-437d-9fd2-315576f153b9	INFO	finally 1
2020-10-30T13:07:15.602Z	d9da2098-c9e6-437d-9fd2-315576f153b9	INFO	finally 2
END RequestId: d9da2098-c9e6-437d-9fd2-315576f153b9
REPORT RequestId: d9da2098-c9e6-437d-9fd2-315576f153b9	Duration: 39.39 ms	Billed Duration: 100 ms	Memory Size: 128 MB	Max Memory Used: 64 MB	Init Duration: 125.59 ms	
```

콘솔에 찍은 `try`, `finally 1`, `finally 2`가 보인다.

## context.callbackWaitsForEmptyEventLoop

백엔드에 데이터베이스가 빠질리 없다. AWS Lambda 내부에서 mysql, redis 같은걸 쓴다고 치자.
그러면 람다 핸들러에서 응답을 보냈음에도 함수가 끝나지 않고 timeout 당하는걸 보게된다.
흔한 문제라서 **AWS Lambda + timeout**을 검색하면 해결책을 바로 찾을 수 있다.

https://stackoverflow.com/questions/42605093/aws-lambda-rds-connection-timeout
`context.callbackWaitsForEmptyEventLoop = false`을 설정하면 된다.

[AWS Lambda 컨텍스트 객체(Node.js)](https://docs.aws.amazon.com/ko_kr/lambda/latest/dg/nodejs-context.html)
> callbackWaitsForEmptyEventLoop – Node.js 이벤트 루프가 빌 때까지 대기하는 대신, 콜백이 실행될 때 즉시 응답을 보내려면 false로 설정합니다. 이것이 false인 경우, 대기 중인 이벤트는 다음 번 호출 중에 계속 실행됩니다.

핸들러 생긴건 `async (event, context) => {...}`인데 왜 callback이 튀어나오는건가?

[AWS Lambda 함수 핸들러(Node.js)](https://docs.aws.amazon.com/ko_kr/lambda/latest/dg/nodejs-handler.html#nodejs-handler-sync)에 답이 있다.

> 세 번째 인수 callback은 응답을 전송하기 위해 비동기 이외의 핸들러에서 호출할 수 있는 함수입니다. 콜백 함수는 두 개의 인수, Error 및 응답을 사용합니다. 호출하면 Lambda는 이벤트 루프가 비워질 때까지 기다린 다음 응답이나 오류를 호출자에게 반환합니다. 응답 객체는 JSON.stringify와 호환되어야 합니다.

또한 함수 핸들러 문서는 `context.callbackWaitsForEmptyEventLoop`를 `false`로 설정해야되는 이유를 자세히 설명했다.

> 비동기 이외의 핸들러의 경우, 이벤트 루프가 비어 있거나 함수 제한 시간을 초과할 때까지 함수 실행이 계속됩니다. 응답은 모든 이벤트 루프 작업이 완료될 때까지 호출자에게 전송되지 않습니다. 함수 제한 시간을 초과하면, 대신 오류가 반환됩니다. context.callbackWaitsForEmptyEventLoop를 false로 설정하여 즉시 응답을 전송하도록 런타임을 구성할 수 있습니다.


## 변경된 예제

`context.callbackWaitsForEmptyEventLoop = false`로 설정하고 `callback`을 수동으로 호출해보자.
callback 사용 방법은 [API Gateway에서 Lambda 프록시 통합 설정](https://docs.aws.amazon.com/ko_kr/apigateway/latest/developerguide/set-up-lambda-proxy-integrations.html)를 적당히 베끼면 된다.

> Node.js에서 Lambda 함수의 응답을 반환하려면 다음과 같은 명령어를 사용할 수 있습니다.
> 성공 결과를 반환하려면 callback(null, {"statusCode": 200, "body": "results"})을 호출합니다.

```js
const delay = (millis) => new Promise(resolve => setTimeout(resolve, millis));

exports.handler = async (event, context, callback) => {
  context.callbackWaitsForEmptyEventLoop = false;

  try {
    callback(null, { statusCode: 200, body: 'a' });
  } catch (e) {
    return { statusCode: 200, body: 'b' };

  } finally {
    console.log('finally 1');
    await delay(1);
    console.log('finally 2');
  }
};
```

```
Response:
{
  "statusCode": 200,
  "body": "a"
}

Request ID:
"e6262512-05c0-4d38-98f9-ab9f79c381d6"

Function logs:
START RequestId: e6262512-05c0-4d38-98f9-ab9f79c381d6 Version: $LATEST
2020-10-30T13:07:27.997Z	e6262512-05c0-4d38-98f9-ab9f79c381d6	INFO	try
2020-10-30T13:07:27.999Z	e6262512-05c0-4d38-98f9-ab9f79c381d6	INFO	finally 1
END RequestId: e6262512-05c0-4d38-98f9-ab9f79c381d6
REPORT RequestId: e6262512-05c0-4d38-98f9-ab9f79c381d6	Duration: 3.82 ms	Billed Duration: 100 ms	Memory Size: 128 MB	Max Memory Used: 65 MB	Init Duration: 136.45 ms
```

콘솔에 찍은 `try`, `finally 1`는 보이지만 `finally 2`는 없다.
응답을 콜백으로 보내니까 finally 실행 도중 async/await를 만나고 람다가 꺼졌다.

## 문제 상황

### sentry 전송 실패

```ts
try {
  return handle();

} catch (error) {
  Sentry.captureException(error);
  throw error;
  
} finally {
  await Sentry.flush(2000);
}
```

처음 나왔던 문제의 코드를 다시 보자.
finally가 실행되지 않은게 아니라 finally 내부가 실행되지마자 `async/await`라서 람다가 꺼진거였다.

### response cache

HTTP API 요청마다 request id를 붙이고 응답 결과를 redis에 저장하는걸 구현하려고 했다.
같은 request id로 요청을 다시 보내면 캐싱된 응답을 던져줘서 같은 로직이 여러번 실행되는걸 피할수 있다.
express에서 응답 보내고 핸들러 호출하는 식으로 구현하려고 했다.

```ts
// https://stackoverflow.com/a/38057516
res.on('finish', () => { /* do stuff here */ });
```

물론 실패했다. finish 핸들러는 한박자 늦게 실행되었다.
A 요청을 보내고 A 응답을 받을때 핸들러가 돌지 않았다.
이어서 B 요청을 보내면 A 요청의 핸들러가 실행되었다.
이것 역시 `context.callbackWaitsForEmptyEventLoop = false`에 의해 발생하는 현상이다.

[serverless-http]: https://github.com/dougmoscrop/serverless-http
[sentry]: https://sentry.io/welcome/
[doc-sentry-serverless]: https://github.com/getsentry/sentry-docs/blob/1146223d0912b60538b0142728cb73be59ebff81/src/collections/_documentation/platforms/node/serverless.md
