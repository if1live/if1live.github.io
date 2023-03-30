---
title: serverless-offline 없이 AWS Lambda 함수를 로컬에서 실행하는 방법
subtitle: 나만의 serverless-offline 만들기
tags: [serverless]
slug: emulate-aws-lambda-function-locally-without-serverless-offline
author: if1live
date: 2023-03-30T20:00:00+09:00
url: /posts/emulate-aws-lambda-function-locally-without-serverless-offline
---

## serverless-offline: 장점

[Serverless Offline][github-serverless-offline]는 AWS Lambda에 배포하지 않고 함수를 로컬호스트에서 테스트해볼수 있는 좋은 툴이다.

serverless offline 문서에 의하면 Node.js, Python, Ruby, Go, Java을 테스트할 수 있다.
https://github.com/dherault/serverless-offline/blob/v12.0.4/src/config/supportedRuntimes.js
(serverless-offline v12.0.4 기준으로 닷넷은 지원하지 않는다. 불쌍한 닷넷...)

API Gateway의 REST API, HTTP API, WebSocket API를 구현해놔서 간단한 웹서비스 정도면 로컬에서 돌릴 수 있다.

[serverless framework plugins][serverless-plugins] 목록을 뒤져보면 serverless offline에 붙여쓸 수 있는게 많다.
내 경우에는 [serverless-offline-sqs][npm-serverless-offline-sqs]를 유용하게 썻다.

## serverless-offline: 단점

### 다양한 언어를 지원

serverless-offline은 자바스크립트로 구현되어있다.
자바스크립트로 구현된 물건이 파이썬 함수를 호출할 수 있다고? 어떻게?

```js
this.#handlerProcess = spawn(
  pythonExecutable,
  [
    '-u',
    join(import.meta.url, 'invoke.py'),
    relative(cwd(), handlerPath),
  handlerName,
  ],
  {
    env: assign(process.env, this.#env),
    shell: true,
  },
)
```
https://github.com/dherault/serverless-offline/blob/v12.0.4/src/lambda/handler-runner/python-runner/PythonRunner.js#L41

답은 `spawn` 이다. serverless-offline에서 파이썬을 직접 실행한다.

파이썬 핸들러만 spawn일까?
자바스크립트로 구현된 물건이 Java, Go, Ruby를 호출할수 있다고? 어떻게?

```js
// https://github.com/dherault/serverless-offline/blob/v12.0.4/src/lambda/handler-runner/java-runner/JavaRunner.js#L4
import { invokeJavaLocal } from 'java-invoke-local'

// https://github.com/dherault/serverless-offline/blob/v12.0.4/src/lambda/handler-runner/ruby-runner/RubyRunner.js#LL6
import { execa } from 'execa'

// https://github.com/dherault/serverless-offline/blob/v12.0.4/src/lambda/handler-runner/go-runner/GoRunner.js#L6
import { execa } from 'execa'
```

[java-invoke-local][npm-java-invoke-local], [execa][npm-execa] 의 코드를 열어보지 않아도 구조를 상상할 수 있을 것이다.
Node.js의 [child_process][nodejs-child_process]에서 자유롭지 못할 것이다.

간단한 설정으로 serverless-offline를 사용하는 경우, serverless-offline를 거치는 모든 람다 함수 호출은 느리다.
사실상 cold start로 동작하게 된다.
함수 하나의 동작만 테스트할때는 문제가 없지만 람다에 얽힌 애플리케이션을 로컬호스트에서 테스트할때는 적절하지 않다.
로컬호스트에서 서버가 실행되는데도 프론트엔드에서 버튼 하나 누를때마다 몇초씩 걸리면 답답해서 못해먹는다.

### reload

[nodemon][npm-nodemon] 같은 도구로 서버를 실행하고 코드를 고치면 코드가 바뀌었을때 서버가 다시 뜬다.
코드 고치는게 실시간으로 반영되면 개발할때 편한다.
serverless-offline으로 서버를 띄운 다음에 코드 고치는게 실시간으로 반영되면 개발할때 편할거같다.

하지만 그게 과거에는 간단하게 되지 않았다.
코드를 고쳐도 서버는 옛날 코드로 동작했다. 직접 Ctrl-C로 끄고 다시 켜야했다.
serverless-offline의 reloadHandler 옵션을 쓰면 되지만 모든 요청이 느려져서 다른 문제가 생겼다.
요새는 [serverless-offline-watcher][npm-serverless-offline-watcher]도 있다는데 내가 쓰던 시절에는 없었다.

## 나만의 serverless-offline

어차피 Node.js만 쓸거고 serverless-offline의 모든 기능이 필요한건 아니다.
http 요청을 받으면 `APIGatewayProxyHandlerV2`가 호출되고, SQS 메세지 넣으면 `SQSHandler`가 호출되는거로 충분하다.
그러면 내가 필요한 부분만 짜면 되는거 아닌가?

AWS Lambda 함수를 로컬호스트에서 실행할때 필요한 것은 무엇인가?

1. 이벤트를 어떤식으로 받을 수 있어야 한다.
2. 외부에서 받은 이벤트를 AWS Lambda의 이벤트로 변환한다.
3. AWS Lambda 이벤트로 내가 원하는 핸들러를 호출한다.
4. 핸들러의 결과를 외부로 잘 연결한다.

해볼만한데?

### 예제: HTTP API

1. 이벤트 받기: Node.js 내장 라이브러리를 사용해서 간단한 웹서버를 만든다.
2. 이벤트 변환: `http.IncomingMessage`를 `APIGatewayProxyEventV2`로 적절히 변환한다.
3. 핸들러 호출: `APIGatewayProxyEventV2`로 `APIGatewayProxyHandlerV2`를 호출한다.
4. 응답: `APIGatewayProxyResultV2`를 `http.OutcomingMessage`로 변환해서 리턴하면 나머지는 Node.js 내장 라이브러리가 해줄거다.

`serverless.yml`을 파싱해서 URL을 원하는대로 연결하려면 머리 아플 수 있다.
serverless-http 같은걸 쓰고 express와 엮으면 `httpApi: *`를 쓸 수 있어서 편한다.

### 예제: SQS

1. 이벤트 받기
    1. 로컬호스트에 [elasticmq][github-elasticmq]를 띄운다. SQS 대용
    2. elasticmq에서 메세지를 폴링하는 루프를 적절히 짠다.
2. 이벤트 변환: 메세지를 `SQSEvent`로 적절히 변환한다.
3. 핸들러 호출: `SQSEvent`로 `SQSHandler`를 호출한다.
4. 응답: `SQSHandler`는 응답이 없어서 안해도 된다.

SQS 롱폴링 정도가 추가되는데 적절히 하면 된다.

### 예제: WebSocket API

[ws][npm-ws] 같은거로 웹소켓 서버를 짜면 간단하게 끝날거같지만 웹소켓의 경우는 사실상 하나 더 짜야한다.
[백엔드 서비스에서 @connections 명령 사용][aws-websocket-api] 규격에 맞는 http 서버도 끼워넣어야한다.
API Gateway WebSocket API은 HTTP 요청을 통해서 웹소켓을 제어한다.
웹소켓으로 메세지를 보낼때, 웹소켓 연결을 끊을때, 웹소켓 정보를 받을때 쓴다.

## 마무리

HTTP API, WebSocket API, SQS, CloudWatch scheduled events 구현해서 잘 쓰고있다.
HTTP 요청 받을수 있고, 웹소켓으로 실시간 통신하고, 메세지큐로 비동기 작업하고, Cron으로 주기적 작업 처리할수 있으면 웹서비스 만드는데 필수 도구는 다 있는거 아닐까?

람다 핸들러가 로컬호스트에서 그럴싸하게 돌아가니까 기능 검증은 다 해볼 수 있다.
로컬 환경과 람다 환경이 달라서 모든 문제를 재현할 수 있는건 아니지만 이건 서버리스라서 어쩔 수 없다.
(예: 패키징 과정에 문제 생기면 로컬에서는 잘 도는데 람다에서는 터짐)

글을 쓸때 실제로 돌아가는 코드를 넣고 싶었지만 귀찮아서 넣지 않았다.
나는 2년 전에 설명된 내용을 구현해서 serverless-offline에서 탈출했지만 코드는 내꺼가 아니다.
고작 글 하나 쓰겠다고 이미 짜본 코드 또 짤라니 귀찮더라...

이 글은 사실상 2년전의 serverless-offline 경험으로 작성되었다.
2년 사이에 serverless-offline이 내가 생각하는 것보다 훨씬 좋아졌을지 모른다.
(2022년에도 잠깐 만져본 경험으로는 크게 바뀌지 않았지만)
또는 serverless-offline의 대안이 충분히 많아졌을지 모른다.
하지만 새로운걸 검색해서 삽질하긴 귀찮고 2년동안 쓴 검증된 코드를 이제와서 버리긴 싫고.
잘 돌아가니까 쓰던거 쓰게 된다.

[github-serverless-offline]: https://github.com/dherault/serverless-offline
[serverless-plugins]: https://www.serverless.com/plugins
[npm-serverless-offline-sqs]: https://www.npmjs.com/package/serverless-offline-sqs
[npm-execa]: https://www.npmjs.com/package/execa
[npm-java-invoke-local]: https://www.npmjs.com/package/java-invoke-local
[npm-nodemon]: https://www.npmjs.com/package/nodemon
[npm-ws]: https://www.npmjs.com/package/ws
[npm-serverless-offline-watcher]: https://www.npmjs.com/package/serverless-offline-watcher
[nodejs-child_process]: https://nodejs.org/api/child_process.html
[github-elasticmq]: https://github.com/softwaremill/elasticmq
[aws-websocket-api]: https://docs.aws.amazon.com/ko_kr/apigateway/latest/developerguide/apigateway-how-to-call-websocket-api-connections.html
