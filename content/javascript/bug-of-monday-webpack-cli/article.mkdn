---
title: 월요일의 버그 - webpack-cli
subtitle: 오픈소스 뒤에 있는 것은 사람이다.
tags: [bug, webpack, javascript]
slug: bug-of-monday-webpack-cli
author: if1live
date: 2019-06-29
url: /posts/bug-of-monday-webpack-cli
---

## 문제!!!

이번주 월요일(2019/06/24)에 webpack 빌드를 돌렸는데 webpack-cli 안에서 터졌다.

```
if (!e && fileOwnerId === process.getuid()) utimesSync(openCollectivePath, now, now);
TypeError: process.getuid is not a function
    at e (<hidden>\node_modules\webpack-cli\bin\cli.js:356:43)
    at FSReqWrap.oncomplete (fs.js:141:20)
```

금요일(2019/06/21)까지는 멀쩡하게 돌아가는 물건이었는데 왜???
그것도 내 코드가 아니라 왜 webpack-cli에서???

## 문제 분석

`webpack-cli/bin/cli.js`를 열어보면 원인을 쉽게 찾을 수 있다.

```js
/**
* Show a hint to donate to our Opencollective
* once a week, only on Monday
*/
const openCollectivePath = __dirname + "/opencollective.js";
const MONDAY = 1;
const SIX_DAYS = 518400000;
const now = new Date();
if (now.getDay() === MONDAY) {
    const { access, constants, statSync, utimesSync } = require("fs");
    const stat = statSync(openCollectivePath);
    const lastPrint = stat.atime;
    const fileOwnerId = stat.uid;
    const lastPrintTS = new Date(lastPrint).getTime();
    const timeSinceLastPrint = now.getTime() - lastPrintTS;
    if (timeSinceLastPrint > SIX_DAYS) {
        require(openCollectivePath);
        // On windows we need to manually update the atime
        // Updating utime requires process owner is as same as file owner
        access(openCollectivePath, constants.W_OK, e => {
            if (!e && fileOwnerId === process.getuid()) utimesSync(openCollectivePath, now, now);
        });
    }
}
```

내부에서 `process.getuid()`라는 함수를 사용한다.
Node.js [문서][docs-process-getuid]에 `process.getuid()`는 윈도우에서 쓸 수 없는 함수로 써있다.
그래서 `process.getuid`가 윈도우에서는 `undefined`로 나온다.

> The process.getuid() method returns the numeric user identity of the process. (See getuid(2).)
>
> This function is only available on POSIX platforms (i.e. not Windows or Android).

발생 조건을 정리하면 다음과 같다

* webpack-cli v3.3.4 이하
* 윈도우, node에 `process.getuid`가 없다
* 매주 월요일, 주석에도 친절하게 적혀있다.

## 문제 대응

webpack-cli v3.3.5로 올리면 된다.
월요일마다 기부 요청하는 메세지가 사라졌다.
문제 해결이라기보다는 문제 우회같은 느낌이긴하다.

[cli: remove donation prompt](https://github.com/webpack/webpack-cli/commit/a37477dd37525a497728b3076d6745928bd5b974)

나중에 찾아보니까 이슈로도 등록되어 있더라.

* [Webpack crushes when tries to print a message about donating in Windows 8.1 #962][issue-962]
* [fix: removed process.getuid method as its not supported in winos #966][pull-996]

## 결론

열정으로만은 오픈소스가 굴러갈 수 없다.
열정이 밥먹여주는건 아니니까.
밥은 돈이 있어야 먹는다.
webpack 개발자도 그걸 아니까 유저들에게 기부해달라고 했나보다.
그런 의미에서는 오픈소스라서 터진 버그라고 생각할 수 있다.
상용 프로그램이었으면 기부해달라는 코드를 짤 일이 없었을테니까.

오픈소스의 혜택을 누리는 사람이라면 개발자에게 기부해보는건 어떨까?
오픈소스 뒤에 있는 것도 당신과 같이 돈을 좋아하는 사람일테니까.

https://opencollective.com/webpack/donate/


[docs-process-getuid]: https://nodejs.org/api/process.html#process_process_getuid
[issue-962]: https://github.com/webpack/webpack-cli/issues/962
[pull-996]: https://github.com/webpack/webpack-cli/pull/966
