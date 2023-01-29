---
title: serverless 배포하기 (5) 최소한의 설정파일로 타입스크립트 함수 배포하기
tags: [serverless]
slug: deploying-serverless-5-zero-configuration
author: if1live
date: 2022-08-20T23:00:00+09:00
url: /posts/deploying-serverless-5-zero-configuration
---

[이전글](/posts/deploying-serverless-4-pnpm-with-script)에서는 빌드, 패키징 과정을 직접 제어해서 serverless plugin을 사용하지 않고 람다로 배포해보았다.
패키징 과정을 직접 제어할수 있으면 많은걸 할 수 있지만 이것은 모든 상황에서 필요한 기능이 아니다.
간단한 람다 함수 배포에서는 쉽고 간편한 방법을 사용하고 싶다.
쉽고 간편한 방법을 쓰다가 문제가 생겼을때만 직접 제어하는게 맞는 접근법이다.
이번 글에서는 최소한의 설정파일로 타입스크립트 함수를 람다에 배포해본다.

## zero-configuration(?)

설정파일 하나 없이 타입스크립트 프로젝트를 람다로 배포하는건 사실 불가능하다.
타입스크립트 프로젝트와 람다를 정의하는데 필요한 최소한의 파일을 뽑아보자.

* tsconfig.json
	* 타입스크립트 컴파일에 필요한 설정파일
	* `tsconfig.json`이 없으면 tsc는 타입스크립트를 어떻게 생긴 자바스크립트로 뽑아야되는지 모른다.
* package.json
	* 의존성을 정의. 외부 패키지를 하나도 안쓰고 의미있는 작업을 하는건 어렵다. (node-fetch조차 안쓸수 없으니)
	* `package.json`에 `"type": "module"`를 넣어서 node.js가 ESM으로 작동하게 만들 수 있다.
* serverless.yml
	* 람다 함수와 이벤트, ... 를 정의
	* `serverless.yml`이 없으면 serverless framework는 무엇을 어떻게 배포해야 되는지 모른다.

`tsconfig.json`, `package.json`는 타입스크립트 프로젝트를 정의하는 최소한의 설정파일이다.
`serverless.yml`만을 사용해서 빌드, 패키징, 배포를 정의하자.

## 빌드

이전까지는 webpack을 사용해서 타입스크립트를 자바스크립트로 번들링했다.
근데 2022년에도 `webconfig.config.js`를 만들어가며 webpack을 꼭 써야할까?
deno의 경우 [deno bundle](https://deno.land/manual/tools/bundler) 명령어를 사용해서 타입스크립트 프로젝트를 자바스크립트 파일 1개로 묶을 수 있다.
node.js에서 `deno bundle`와 비슷한걸 사용해서 자바스크립트 파일 1개로 패키지까지 묶을수 있으면 배포가 편하다.
(참고: ESM 프로젝트의 경우 package.json도 생긴다. package.json에 ESM 플래그가 포함된다)

[ncc][repo-ncc]를 쓰면 webpack이 처리하는 문제를 더 간단하게 해결할수 있다.

> Compile a Node.js project into a single file. Supports TypeScript, binary addons, dynamic requires.

`ncc build ./handler.ts -o output` 로 타입스크립트 핸들러 함수와 의존성 패키지를 하나의 자바스크립트 파일로 묶을수 있다.

## 패키징

deno 프로젝트를 serverless를 사용해서 람다로 배포하려고 찾아보다가 [deno-lambda][repo-deno-lambda]를 발견했다.
deno-lambda의 예제가 [serverless-scriptable-plugin][repo-serverless-scriptable-plugin]를 사용하더라.

`serverless-scriptable-plugin`를 사용해서 빌드 아티팩트 생성 직전에 `ncc`로 빌드를 하면 뭔가 될거같더라.

```yaml
service: hello-zero-configuration

frameworkVersion: '3'

provider:
  name: aws
  runtime: nodejs16.x
  region: ap-northeast-1

package:
  patterns:
    - "!**"
    - "!node_modules/**"
    - output/*.js
    - output/*.json

functions:
  hello:
    handler: output/index.hello
    events:
      - httpApi: '*'

plugins:
  - serverless-scriptable-plugin

custom:
  scriptHooks:
    before:package:createDeploymentArtifacts: ncc build ./handler.ts -o output
```

```yaml
plugins:
  - serverless-scriptable-plugin

custom:
  scriptHooks:
    before:package:createDeploymentArtifacts: ncc build ./handler.ts -o output
```

`serverless-scriptable-plugin` README를 참고해서 ncc로 빌드를 뽑아서 `output/` 디렉토리에 넣었다.

```yaml
package:
  patterns:
    - "!**"
    - "!node_modules/**"
    - output/*.js
    - output/*.json
```

serverless가 패키징할때 `output/` 디렉토리만 갖다 쓰게 제한했다.
`serverless-scriptable-plugin` README와는 형태는 조금 다른데 알아서 잘 수정해서 쓰면 된다.
(프로젝트에 따라서는 typeorm의 ormconfig.json과 같이 별도의 설정파일도 패키징에 포함되어야 할 수 있다)

```yaml
functions:
  hello:
    handler: output/index.hello
```

`ncc`로 빌드를 뽑으면 출력 파일명이 `index.js`이다.
출력 경로를 `output/`로 잡았으니 `output/index.js`의 함수 `hello`를 핸들러로 연결했다.

## 배포

```bash
pnpm sls deploy
```
serverless deploy 명령어 한줄이면 된다!

상황에 따라 별도의 빌드 스크립트가 필요해져도 `serverless.yml`만 고치면 된다.
serverless-scriptable-plugin는 cli 명령을 그대로 실행하니까 적당히 잘 넣으면 된다.
내 경우, `before:package:createDeploymentArtifacts: pnpm run build`로 쓰고 있다.
복잡해질수 있는 ncc 빌드와 serverless 패키징을 분리해서 생각하니까 좋더라.

## next?

2022년에 타입스크립트 프로젝트를 람다로 배포하려면 어떻게 해야되는가?
[ncc][repo-ncc]와 [serverless-scriptable-plugin][repo-serverless-scriptable-plugin]를 같이 쓰면 된다.

지난 몇년 동안은 정답을 몰라서 삽질을 했었다.
너무 그럴싸한 이름에 낚여서 [serverless-plugin-typescript][repo-serverless-plugin-typescript] 쓰다 던지고.
몇년전에 `sls create --template aws-nodejs-typescript`에서 쓰던 [serverless-webpack][repo-serverless-webpack]에도 낚이고.
플러그인 안쓰고 스크립트로 제어했다가 너무 복잡해져서 다음 프로젝트에서는 안쓸거같고.
지금 도착한건 ncc와 serverless-scriptable-plugin의 조합인데 이정도면 웬만한건 될거같다.

ncc가 개인이 심심풀이로 만드는 프로젝트라면 언제 유지보수가 끊겨도 이상하진 않을것이다.
다행히도 ncc github 저장소 이름은 `vercel/ncc`이다.
vercel은 serverless function으로 장사하는 회사니까 ncc의 가치를 알고 있을테고 쉽게 던지지 않을거라 믿는다.


지금까지 삽질한게 2022년의 답이라면 2023년에는 다른 답이 생길 수 있다.
2022년 이후에 serverless framework를 사용해서 타입스크립트 프로젝트를 람다로 배포하는 방법에는 3가지 답이 있을 것이다.

### ncc + serverless-scriptable-plugin

serverless 만져본지 4년? 5년?쯤 되었는데 지금까지 본 것 중에서 제일 멀쩡하다.
이 조합이면 몇 년 더 쓸수 있을거같다.
vercel이 ncc를 던지기 전까지는 쓸수 있을거같다.

### serverless-esbuild

2022년 8월 20일에 `sls create --template aws-nodejs-typescript`로 프로젝트 만들면 [serverless-esbuild][repo-serverless-esbuild]를 쓴다.
serverless에 내장된 템플릿이니 자기들이 생각해도 멀쩡해보이는걸 사용하지 않았을까?

몇년전에는 aws-nodejs-typescript 템플릿이 `serverless-webpack`을 썻다.
serverless 내장된 템플릿이 `serverless-webpack`를 쓰니까 이게 답이라고 믿고 썼다가 몇년을 삽질했더라.
`serverless-esbuild`에 당해보지 않아서 잘 모르지만 `serverles-webpack`보다는 멀쩡하니까 갈아탔을거라고 믿는다.

### deno
`deno bundle`는 deno에 내장되어있으니 node.js에 내장되지 않은 `ncc`보다 신뢰할 수 있을 것이다.
아직은 aws lambda에서 deno 런타임을 공식적으로 지원하지 않는다.
[deno-lambda][repo-deno-lambda]를 layer를 사용해서 deno를 배포할수 있지만 레이어가 모든 리전에 있는것도 아니고 arm64까지 제대로 돌아간다는 확신도 없다.
하지만 us-east-1 리전에서 x86_64로 돌아가는걸 직접 확인해봤다.

node.js를 반드시 써야하는 상황이 아니라면 deno도 미래의 선택지로 남겨두고 싶다.
내가 관심있는건 npm의 수많은 패키지인거지 node.js 자체인건 아니니까.

[repo-serverless-scriptable-plugin]: https://github.com/weixu365/serverless-scriptable-plugin
[repo-serverless-esbuild]: https://github.com/floydspace/serverless-esbuild
[repo-serverless-webpack]: https://github.com/serverless-heaven/serverless-webpack
[repo-serverless-plugin-typescript]: https://github.com/serverless/serverless-plugin-typescript
[repo-ncc]: https://github.com/vercel/ncc
[repo-deno-lambda]: https://github.com/hayd/deno-lambda/tree/master/example-serverless
