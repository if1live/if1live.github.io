---
title: localhost를 위한 serverless-standalone 
subtitle: localhost에서 AWS Lambda 흉내내기
tags: [serverless]
slug: serverless-standalone-for-localhost
author: if1live
date: 2023-12-23T00:00:00+09:00
url: /posts/serverless-standalone-for-localhost
---

## serverless-standalone?

이전에 [serverless-offline 없이 AWS Lambda 함수를 로컬에서 실행하는 방법][post-concept]이라는 글을 썻다. serverless-standalone은 위의 개념을 구현한 프로젝트이다.

* github: https://github.com/if1live/serverless-standalone
* npm: https://www.npmjs.com/package/serverless-standalone

## 배경

### 실패한 API KEY 관리

serverless-offline 쓰다 답답해서 serverless-local 이라는걸 개발했었다.
serverless-local은 회사에서 만든거라 소스가 공개되어있지 않다.
오픈소스로 공개할수도 있겠지만 저장소에 때려박은 API Key를 초기화할 생각하니 끔찍하더라.
새로운 저장소에서 새롭게 시작하고 싶었다.

### npm

serverless-local을 오랫동안 쓰다보니까 serverless-offline을 답답해서 못쓰겠더라.
node.js 이외의 런타임으로 AWS Lambda을 사용할 생각이 없으니까 serverless-offline의 장점도 없고.
개인용 플젝을 할때마다 serverless-local이 생각나는데 회사 코드를 그래도 가져오는건 좀 그렇고.
다른 플젝에서 갖다쓰기 쉽도록 npm에 공개된 패키지로 만들고 싶었다.

### aws-sdk: 2 vs 3
* serverless-local은 2021년 4월에 개발
* [2021년 4월에는 AWS Lambda는 Node.js 14가 최신 버전][aws-nodejs-14]
* [AWS Lambda nodejs14.x에는 aws-sdk 2.1374.0가 내장된다][aws-runtime]
* [AWS Lambda는 람다 함수의 패키지 크기가 cold start 시간에 영향을 준다](https://mikhail.io/serverless/coldstarts/aws/)고 알려져있다.
* [aws-sdk-v2의 패키지 크기는 대략 40MB](https://docs.aws.amazon.com/sdk-for-javascript/v3/developer-guide/welcome.html)

위의 정보를 조합하면 serverless-local 만들던 시절에는 aws-sdk-v2에 익숙해지는게 합리적인 선택이었다.
패키지 크기를 줄이기 위해서 런타임에 내장된 aws-sdk-v2를 써야했다. 
aws-sdk-v2에 익숙해지니까 serverless-local도 자연스럽게 aws-sdk-v2로 만들게 되었다.

그리고 지금은 2023년이다. [AWS Lambda 런타임][aws-runtime]으로 18, 20이 추가되었다.
런타임에 내장된 AWS SDK 버전 역시 바뀌었다.

* nodejs18.x: 3.362.0
* nodejs20.x: 3.362.0

새로운 시대에 맞춰서 aws-sdk-v3 사용법을 익혀야겠더라.

## 목표

* `npm install serverless-standalone`
  * 다른 프로젝트에서 쉽게 갖다쓸 수 있어야한다.
* aws-sdk-v3
  * aws-sdk-v3은 어떻게 쓰나 직접 확인하고 싶다.
* E2E 테스트
  * SQS 핸들러를 흉내내고 싶다면 메세지 넣었을때 핸들러가 호출되는 과정까지 볼 수 있어야 한다.
    * [e2e_sqs.ts](https://github.com/if1live/serverless-standalone/blob/bf21f89/e2e/e2e_sqs.ts)
  * 기능별로 e2e 테스트를 작성했다.
* 지원하는 핸들러 목록
  * 필수: httpApi, websocket
    * serverless-offline에서도 되는 기능
    * 내가 람다에서 사용하는 핵심 기능이라서 반드시 작동해야 한다.
  * 권장: schedule, sqs
    * 여기까지 있어으면 람다로 웬만한건 다 할 수 있겠더라.
  * 선택: IoT
    * API Gateway WebSocket API의 한계를 느껴서 IoT mqtt를 검토하고 있어서.
* `@aws-sdk/client-lambda`를 통해서 serverless-standalone에 등록된 핸들러를 호출할 수 있어야 한다.
  * aws-sdk로 람다를 호출할수 있으면 재밌는걸 할수 있는데 그건 나중에 다룬다.
  * serverless-offline에서도 되는 기능이라 유지하고 싶다.


## 사용 방법

README.md 를 작성했지만 개발 과정에서 낡아버렸다.
serverless-standalone을 사용하는 프로젝트가 좋은 README가 되어버렸다.
[karin][karin-github]에서 serverless-standalone을 쓴다.
[dev.ts][karin-example]

```ts
const definitions: FunctionDefinition[] = [
  {
    name: `karin-example-${settings.STAGE}-httpMain`,
    handler: http_main.dispatch,
    events: [
      { httpApi: { route: "ANY /" } },
      { httpApi: { route: "ANY /{pathname+}" } },
    ],
  },
  {
    name: `karin-example-${settings.STAGE}-sqsMain`,
    handler: sqs_main.dispatch,
    events: [
      {
        sqs: {
          queueName: `karin-example-${settings.STAGE}`,
          batchSize: 1,
          enabled: true,
        },
      },
    ],
  },
  {
    name: `karin-example-${settings.STAGE}-websocketConnection`,
    handler: ws_main.handle_connection,
    events: [
      { websocket: { route: "$connect" } },
      { websocket: { route: "$disconnect" } },
    ],
  },
  {
    name: `karin-example-${settings.STAGE}-websocketDefault`,
    handler: ws_main.handle_default,
    events: [{ websocket: { route: "$default" } }],
  },
];

const options = {
  httpApi: { port: 3000 },
  websocket: { port: 3001 },
  lambda: { port: 3002 },
  sqs: { url: "http://127.0.0.1:9324" },
};

const inst = standalone({
  ...options,
  functions: definitions,
});
await inst.start();
```

핸들러 정의는 `serverless.yml`와 비슷한 형태로 만들었다.
`serverless-offline`와 마찬가지로 httpApi, websocket, lambda의 포트를 설정해야 한다.
sqs를 사용하면 elasticmq의 주소를 설정하고, iot를 사용하면 mqtt의 주소를 설정해야한다.


[post-concept]: {{< ref emulate-aws-lambda-function-locally-without-serverless-offline >}}
[karin-example]: https://github.com/if1live/karin/blob/9a1a7f73da240a2038d0d6bc670d0f43c1c280dd/packages/examples/src/dev.ts
[karin-github]: https://github.com/if1live/karin
[aws-nodejs-14]: https://aws.amazon.com/ko/about-aws/whats-new/2021/02/aws-lambda-now-supports-node-js-14/
[aws-runtime]: https://docs.aws.amazon.com/ko_kr/lambda/latest/dg/lambda-nodejs.html
