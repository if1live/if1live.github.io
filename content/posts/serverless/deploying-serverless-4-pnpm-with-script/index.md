---
title: serverless 배포하기 (4) 플러그인 안쓰고 타입스크립트 함수 배포하기
tags: [serverless]
slug: deploying-serverless-4-pnpm-with-script
author: if1live
date: 2022-08-20T20:00:00+09:00
url: /posts/deploying-serverless-4-pnpm-with-script
---

[이전글](/posts/deploying-serverless-3-flaw-of-serverless-webpack)에서는 serverless-webpack 를 사용했을때 발생할 수 있는 문제를 다뤘다.
이번에는 serverless-webpack같은 플러그인을 쓰지 않고 serverless framework만으로 타입스크립트 함수를 배포한다.
손으로 serverless를 배포해보면 AWS Lambda 핸들러가 어떻게 돌아갈지 이해하고 어떤 부분을 더 개선할수 있을지 보인다.

## 빌드

타입스크립트로 간단한 핸들러를 작성하고 webpack을 사용해서 번들링한다.
요새는 webpack 잘 안쓴다던데 예제 코드를 작성한게 2021년이라서 webpack을 유지했다.
관심있는 주제는 배포라서 타입스크립트, webpack 설정파일은 따로 다루지 않았다. (이전글 참고)

`webpack-node-externals`을 사용해서 번들링된 js에 패키지가 포함되는걸 피했다.
웹팩 빌드하면 `.webpack/src/handler.js`, `.webpack/src/handler.js.map`이 생긴다.
(`.webpack`은 serverless-webpack이 쓰는 디렉토리 이름)


## pnpm

패키지 매니저로 [pnpm](https://pnpm.io/)을 사용했다.
패키지 매니저는 관심있는 주제가 아니라서 npm, yarn을 써도 된다.
pnpm을 쓰면 `node_modules`을 직접 손대는게 쉬워서 이번에는 pnpm을 썼다.

## 패키징

람다로 배포하려면 zip 파일 1개로 실행에 필요한 모든걸 우겨넣어야한다.
serverless-webpack과 같은 플러그인을 쓰면 직접 하지 않아도 되는 작업이다.
이번에는 플러그인을 쓰지 않기로 했으니 스크립트를 작성했다.

```bash
#!/bin/bash

set -exuo pipefail

DIR_TMP=artifact

function prepare {
	mkdir -p $DIR_TMP
	cp package.json pnpm-*.yaml $DIR_TMP
	cp -r .webpack/src $DIR_TMP
}

function install_package {
	pushd $DIR_TMP

	pnpm install --prod --frozen-lockfile --offline

	# 불필요한 패키지, 파일 삭제
	rm -rf node_modules/.pnpm/@types+*
	rm -rf node_modules/.pnpm/figlet@*
	rm -rf node_modules/.pnpm/aws-sdk@*

	# https://unix.stackexchange.com/a/622875
	find node_modules/.pnpm -depth -name "test" -type d -execdir rm -rf {} +
	find node_modules/.pnpm -depth -name "tests" -type d -execdir rm -rf {} +
	find node_modules/.pnpm -depth -name "__tests__" -type d -execdir rm -rf {} +

	find node_modules/.pnpm -name "*.ts" -type f -delete
	find node_modules/.pnpm -name "*.map" -type f -delete
	find node_modules/.pnpm -name "*test*" -type f -delete

	popd
}

function archive_artifact {
	pushd $DIR_TMP
	zip -r --symlinks -q ../artifact.zip node_modules src
	popd
}

prepare
install_package
archive_artifact
```

1. prepare
	* `artifact` 디렉토리 생성
	* webpack으로 번들링한 결과물을 `artifact`로 복사
	* pnpm으로 패키지 설치할때 필요한 파일을 `artifact`로 복사
2. install_package
	* `artifact` 디렉토리 안에서 pnpm으로 의존성 패키지 설치
	* 없어도 되는 파일을 `node_modules`에서 삭제해서 크기 줄임
3. archive_artifact
	* `artifact` 디렉토리를 `artifact.zip`으로 압축
	* 심볼릭 링크까지 잘 처리하기 (pnpm이 심볼릭 링크를 쓴다)

## serverless.yml

```yaml
service: hello-pnpm-with-script

frameworkVersion: '2'

package:
  artifact: artifact.zip

provider:
  name: aws
  runtime: nodejs14.x
  lambdaHashingVersion: 20201221

  memorySize: 128
  logRetentionInDays: 1
  versionFunctions: false

  region: ap-northeast-1

functions:
  hello:
    handler: src/handler.hello
    events:
      - httpApi: '*'
```

패키징을 수동으로 처리해서 serverless framework는 무엇을 배포해야되는지 모른다.
package 항목으로 artifact 파일을 지정해준다.

```yml
package:
  artifact: artifact.zip
```

람다 함수의 handler는 실행될 람다 핸들러를 뜻한다.
타입스크립트로 작성된 `src/handler.ts`의 함수 `hello`는 `artifact.zip`에 없다.
`artifact.zip`에 있는 webpack으로 번들링된 자바스크립트 핸들러의 함수를 지정해야된다.
`src/handler.js`의 함수 `hello`를 연결해야되니까 `src/handler.hello`를 넣는다.


## 배포

```bash
# typescript --(webpack)--> javascript
pnpm webpack

# javascript, node_modules/, ... --(compress)--> artifact.zip
./build-artifact.sh

# deploy
pnpm sls deploy
```

## next?

수동으로 artifact를 생성해서 배포해보니까 어떤 부분을 더 개선할수 있을지 보이더라.
다음 글에는 2022년의 방식으로 타입스크립트 함수를 람다에 배포해본다.

이번 글의 예제 코드는 2021년 8월에 작성되었다.
스크립트를 사용해서 빌드 아티팩트를 생성해서 serverless 배포하는건 과도기적 형태같아서 글을 마무리하지 1년동안 방치했다.
1년쯤 지나니까 타입스크립트 함수를 람다로 배포하는 더 우아한 방법을 알겠더라. 그래서 오랜만에 글을 써 봤다.
