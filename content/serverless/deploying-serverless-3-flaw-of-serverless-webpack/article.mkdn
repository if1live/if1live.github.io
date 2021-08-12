---
title: serverless 배포하기 (3) serverless-webpack의 결함
tags: [serverless]
slug: deploying-serverless-3-flaw-of-serverless-webpack
author: if1live
date: 2021-08-12
url: /posts/deploying-serverless-3-flaw-of-serverless-webpack
---

[이전글](/posts/deploying-serverless-2-serverless-webpack-advance)에서는 serverless-webpack 기반으로 배포 패키지 크기를 줄여봤다.
3부에서는 serverless-webpack의 결함을 다룬다.
작성일 기준으로 최신 버전은 5.5.1 이다.
serverless-webpack의 버전이 올라가면 글의 내용과 달라질 수 있다.

## 패키징이 오래 걸린다

`npx sls webpack`를 실행한후 기다리면 `.webpack/`, `.serverless/` 디렉토리가 생긴다.
`.serverless/` 안에는 배포 패키지 `xxx.zip`가 들어있다.
배포 패키지를 생성하는 시간이 빠르면 배포 과정도 빠를 것이다.
근데 `npx sls webpack`는 생각보다 매우 오래 걸린다.
예제는 몇줄도 안되는 타입스크립트 코드니까 웹팩 빌드는 매우 빠를 것이다.
의존성 걸린 패키지는 이미 로컬 `node_modules/`에 들어있다.

대체 왜 느린거지?

### 중복된 복사

웹팩 빌드 결과물을 뒤져보면 두개의 `node_modules/` 가 새로 생긴다.
수십MB의 파일, 수만개의 파일이 새로 생겼으니 패키징 속도가 빠를 수 없다.

근데 왜 `node_modules`가 두개나 생긴거지? 디렉토리 내부를 비교해보면 재밌다.
이전글에서 배포 패키지 크기를 줄이려고 `npx rimraf "node_modules/@types"`를 썻다.

* `.webpack/dependencies/node_modules`
	* `@types` 있음
* `.webpack/services/node_modules`
	* `@types` 없음

결과물을 보고 serverless-webpack의 코드를 보지 않고도 동작을 상상할 수 있다.

1. 생성: `.webpack/dependencies/node_modules`
2. 복사: `.webpack/dependencies/node_modules` -> `.webpack/services/node_modules`
3. 삭제: `.webpack/services/node_modules/@types`
4. zip: `.webpack/services` -> `.webpack/xxx.zip`

근데 node_modules는 대체 왜 만들어진거지?

### npm install

serverless-webpack은 npm, yarn을 지원한다.
코드를 뒤져보면 각각의 패키지 매니저에 대응하는 install 이 있다.

[npm][packager-npm]
```js
static install(cwd) {
	const command = /^win/.test(process.platform) ? 'npm.cmd' : 'npm';
	const args = ['install'];

	return Utils.spawnProcess(command, args, { cwd }).return();
}
```

[yarn][packager-yarn]
```js
static install(cwd, packagerOptions) {
	const command = /^win/.test(process.platform) ? 'yarn.cmd' : 'yarn';
	const args = [ 'install', '--non-interactive' ];
	...
	return Utils.spawnProcess(command, args, { cwd }).return();
}
```

두 install 함수 모두 `Utils.spawnProcess()`를 쓴다.
이 함수는 진짜로 process를 spawn한다.
`.webpack/dependencies/node_modules`는 `npm install`에 의해서 만들어진 것이다.
npm install이 실행되고 인터넷에서 패키지를 받을테니 패키징 속도가 느린건 어쩔 수 없다.

https://github.com/serverless-heaven/serverless-webpack/blob/v5.5.1/lib/utils.js#L73
```js
function spawnProcess(command, args, options) {
	return new BbPromise((resolve, reject) => {
		const child = childProcess.spawn(command, args, options);
```

npm install 하려면 package.json이 필요한데 이건 어떻게 만들어진거지?

## 새로운 package.json

serverless-webpack이 얼마나 똑똑한지 보여주려고 멍청한 짓을 해봤다.
`p-limit`은 package.json에는 있지만 handler.ts에는 없다.
package.json에는 의존성이 정의되어 있지만 실제로는 사용되지 않는 패키지이다.
`aws-xray-sdk-core`는 package.json에는 없지만 handler.ts에는 있다.
`dynamo-types`는 `aws-xray-sdk-core`에 의존하고 있어서 package.json에 넣지 않아도 쓸 수 있다.

package.json
```
"dependencies": {
	"aws-sdk": "^2.958.0",
	"dynamo-types": "^2.10.1",
	"lodash": "^4.17.21",
	"mysql": "^2.18.1",
	"p-limit": "^3.1.0",
	"typeorm": "^0.2.36"
},
"devDependencies": {
	"@types/aws-lambda": "^8.10.71",
	"@types/lodash": "^4.14.171",
	"@types/node": "^14.14.25",
	....
```

src/handler.ts
```ts
import 'aws-sdk';
import 'dynamo-types';
import 'lodash';
import 'typeorm';
import 'mysql';

import 'aws-xray-sdk-core';
```

`npx sls webpack`을 돌리면 아래와 같은 로그를 볼 수 있다.

```
Serverless: WARNING: Could not determine version of module aws-xray-sdk-core
Serverless: Excluding external modules: aws-sdk@^2.958.0
Serverless: Package lock found - Using locked versions
Serverless: Packing external modules: dynamo-types@^2.10.1, lodash@^4.17.21, typeorm@^0.2.36, mysql@^2.18.1, aws-xray-sdk-core
Serverless: WARNING: Could not determine version of module aws-xray-sdk-core
Serverless: Copying existing artifacts...
```

.webpack/dependencies/package.json 가 생성된다.

```json
{
  "name": "webpack-flaw",
  "version": "1.0.0",
  "description": "Packaged externals for webpack-flaw",
  "private": true,
  "scripts": {
    "script0": "npx rimraf \"node_modules/{aws-sdk,figlet}\"",
    "script1": "npx rimraf \"node_modules/@types\""
  },
  "dependencies": {
    "dynamo-types": "^2.10.1",
    "lodash": "^4.17.21",
    "typeorm": "^0.2.36",
    "mysql": "^2.18.1",
    "aws-xray-sdk-core": ""
  }
}
```

package.json를 보면 로그를 해석할 수 있다.

```
Serverless: WARNING: Could not determine version of module aws-xray-sdk-core
```
handler.ts에서 `aws-xray-sdk-core`를 쓰니까 의존성에 끼워넣는다.
하지만 버전이 뭔지 모르니까 공백으로 들어간다. `"aws-xray-sdk-core": ""`.

```
Serverless: Excluding external modules: aws-sdk@^2.958.0
```
이전글에서 `serverless.yml` 편집을 통해서 `aws-sdk`를 제외하는 방법을 다뤘다.
그래서 생성된 package.json에는 `aws-sdk`가 없다.

```
Serverless: Package lock found - Using locked versions
Serverless: Packing external modules: dynamo-types@^2.10.1, lodash@^4.17.21, typeorm@^0.2.36, mysql@^2.18.1, aws-xray-sdk-core
```
원본 package.json에서 버전을 찾을수 있는건 그대로 새로운 package.json에도 포함된다.

serverless-webpack이 잘 만들어져있어서 코드에서 사용되지 않은 `p-limit`을 지운걸 확인할 수 있다.

### Phantom dependencies

npm의 경우 직접 설치한적 없지만 의존성에 의해서 설치된 패키지를 코드에서 쓸 수 있다.
이를 [Phantom dependencies][rush-phantom-dependencies]라고 부른다.

package.json에서는 `aws-xray-sdk-core`를 설치한적 없다.
`dynamo-types`가 설치될때 `aws-xray-sdk-core`가 같이 설치되서 쓸 수 있는거다.
나쁜 코드지만 아무튼 돌아간다. 하지만 나쁜 코드에는 이유가 있다.

오늘의 `aws-xray-sdk-core`의 최신버전이 `2.3.6`라고 가정하자.
오늘 `npx sls webpack`를 돌리면 `"aws-xray-sdk-core": ""`에 의해서 오늘의 최신버전이 깔린다.
`aws-xray-sdk-core@2.3.6`이 깔린다.
원본 `package-lock.json`도 오늘의 최신 버전인 `2.3.6`으로 기록될 것이다.

몇달뒤에 `aws-xray-sdk-core`의 버전이 많이 올라가서 `3.3.3`이 되었다고 치자.
미래에 `npx sls webpack`를 돌리면 `"aws-xray-sdk-core": ""`에 의해서 미래의 최신버전이 깔린다.
`aws-xray-sdk-core@3.3.3`이 깔린다.

`aws-xray-sdk-core`가 대격변 패치를 너무 많이 해서 `2.x`와 `3.x`의 호환성이 깨졌다고 치자.
`2.x`에 있던 함수가 `3.x`에서 사라질수도 있는거 아니겠는가?

내 컴퓨터에서 몇달뒤에 `npm install`을 돌려도 package-lock.json에 의해서 `2.3.6`가 설치될 것이다.
근데 serverless-webpack을 거쳐서 배포하면 오늘은 `2.3.6`가 깔리는데 몇달뒤에는 `3.3.3`이 깔린다.
코드 한줄 안고치고 배포만 다시 돌렸는데 서비스가 망가졌다!

> // (1.a.2) Copy package-lock.json if it exists, to prevent unwanted upgrades
> https://github.com/serverless-heaven/serverless-webpack/blob/v5.5.1/lib/packExternalModules.js#L291

serverless-webpack도 패키지가 강제로 업데이트 당하는 문제를 피하려고 노력을 했다.
phantom dependencies를 막지 못하는건 패키지 매니저의 잘못이니까 serverless-webpack의 문제는 아니다.

## 시대에 뒤쳐진 패키지 관리자

node.js 패키지 관리자는 계속 발전하고 있다.
패키지 관리자가 개선될수록 install 속도도 빨라질 것이다.
새로운 개념의 패키지 관리자가 등장해서 install 속도가 매우 빨라지기도 한다.

하지만 serverless-webpack에는 공짜 점심이 없다.
serverless-webpack은 npm과 yarn 1만 지원한다.
yarn 버전도 3.0.1까지 올라갔고 pnpm 같이 좋은 패키지 관리자도 만들어졌는데 쓸 수 없다.

## 어떻게 하면 문제를 해결할 수 있는가?

문제가 뭔지 알았으니 serverless-webpack을 뜯어고치면 된다.
...를 시도하려고 했으나 serverless-webpack 코드 열어보고 접었다.
나는 대단한 사람이 아니라서 serverless-webpack 못고치겠더라.

패키지를 인터넷에서 새로 받는짓을 안하면 빨라질것이다.
pnpm의 경우 받은 패키지를 로컬에 저장해둔다.
나중에 같은 버전을 install하면 인터넷에서 받는 대신에 로컬에 저장된것을 사용하니까 빠르다.

Phantom dependencies가 생기지 않도록 코딩을 잘 하면 된다.
이건 건강한 몸을 유지하기 위해서 좋은걸 먹고 운동을 열심히 하라는 소리와 똑같다.
잘 짜는것보다 원본 package.json, package lock file을 그대로 사용하는게 쉬울거같더라.

## next?

serverless-webpack은 못쓰겠는데 직접 serverless-webpack같은거 만들자니 귀찮더라.
이럴때는 무식한 방법을 쓰면 된다. 컨테이너로 포장해서 도커로 돌리면 된다.
[지금][aws-lambda-container]은 람다에서 컨테이너 이미지를 돌릴 수 있다.
4부는 컨테이너 이미지를 사용해서 람다 배포하는 삽질이 될 것이다.

[packager-npm]: https://github.com/serverless-heaven/serverless-webpack/blob/v5.5.1/lib/packagers/npm.js#L110
[packager-yarn]: https://github.com/serverless-heaven/serverless-webpack/blob/v5.5.1/lib/packagers/yarn.js#L135
[rush-phantom-dependencies]: https://rushjs.io/pages/advanced/phantom_deps/
[aws-lambda-container]: https://aws.amazon.com/ko/blogs/korea/new-for-aws-lambda-container-image-support/
