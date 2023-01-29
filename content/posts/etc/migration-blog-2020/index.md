---
title: 2020년식으로 블로그 빌드 고치기
subtitle: dep -> Go Modules, travis-ci -> GitHub Actions
tags: [migration, blog]
slug: migration-blog-2020
author: if1live
date: 2020-11-14
url: /posts/migration-blog-2020
---

## dep -> Go Modules

golang의 패키지 관리자는 golang이 만들어진 이후 몇 년동안 개판이었다. 
얼마나 개판이었는지는 golang [PackageManagementTools](https://github.com/golang/go/wiki/PackageManagementTools)의 스크롤을 보면 된다.

하지만 golang의 패키지 관리자가 개판인 시절은 끝이다. golang에서 직접 칼을 뽑았다!
저장소 좌표부터 [github.com/golang/dep][dep-repo]이다.
언어에서 공식적으로 넣은 패키지 관리자라면 당연히 갈아타야지!

2018년 3월 9일에 블로그 빌드 시스템에서 `go get`으로 패키지 설치하던걸 `dep`로 교체했다.
[commit](https://github.com/if1live/if1live.github.io/commit/20c3ce281c2341c993e947b9640aa36d0361b3ad)

... 그리고 dep는 죽었다.
dep는 golang 1.9, 1.10까지만 살았다.
golang 1.11부터는 [Go Modules](https://golang.org/ref/mod)로 교체되었다.
dep로 바꾼걸 또 바꾸라고? 모든게 고장나서 터지지 전까지 미뤄야지.

## travis-ci -> GitHub Actions

GitHub Pages는 초기부터 [Jekyll][jekyll-repo]를 지원했다.
공식 문서로 [Setting up a GitHub Pages site with Jekyll](https://docs.github.com/en/free-pro-team@latest/github/working-with-github-pages/setting-up-a-github-pages-site-with-jekyll)도 있다.
Jekyll을 쓰면 편하게 블로그를 만들 수 있었겠지만 당시의 나는 그런 선택을 하지 않았다.

1. 기술적 자유. 깃헙에서 지원해준다는 소리는 지원 범위 밖에 있는 기능은 못 쓴다는 뜻이다.
2. 파이썬으로 밥벌어먹던 시기라서 파이썬 기반을 고르고 싶었다.

그래서 Jekyll 대신 [Pelican][pelican-repo]을 선택했다.

깃헙 내장 기능을 쓰지 않기로 했으니 CI도 직접 선택해야 한다.
CI는 [travis-ci](https://travis-ci.org/)를 골랐다.
오픈소스는 무료이고 다른 프로젝트의 README를 통해서 자주 본 CI였다.

1. markdown으로 글을 써서 푸시
2. travis-ci에서 pelican으로 빌드해서 HTML을 생성
3. 생성된 HTML을 다시 저장소에 푸시
3. GitHub Pages를 통해서 웹에 보임

2014년 1월 27일에 `.travis.yml`을 넣은걸 시작으로 위의 물건이 잘 돌아가게 만드는데 시간을 썼다.
[commit](https://github.com/if1live/if1live.github.io/commit/d6fdb579b04cea455919dc33e2d26b7f04275b93)

그리고 시간은 6년 넘게 지났다. 세상은 바뀌었다.

travis-ci를 무료로 쓰니까 푸시한게 작업큐에 들어가고 처리될때까지 오래 걸리더라.
몇 년 전까지는 5분안에 처리되었던거같은데 요새는 30분씩 걸리더라.
내가 글을 쓴걸 웹에서 보려면 30분씩 걸린다는거다.

게다가 2020년이 되니까 GitHub Actions이라는 이름으로 CI 기능이 추가되었다.
그리고 GitHub Action은 큐에 들어간게 빨리 처리되더라.

## GitHub Actions + Go Modules

처음에는 travis-ci만 GitHub Actions로 교체하려고 했다.
travis-ci용 스크립트를 github actions 규격으로 바꾸면 될줄 알았는데 현실은 그렇지 않았다.
CI가 달라지니까 `GOPATH`에서 문제가 생겼다

travis-ci는 언어만 golang으로 설정하면 GOPATH에 맞춰서 git을 잘 받고 돌린다.
GOPATH는 `/home/travis/gopath`로 설정된다.
git 클론 받는 경로는 `/home/travis/gopath/src/github.com/<user_name>/<repo_name>`이다.
`GOPATH/src` 아래에 저장소가 있어서 dep가 잘 돌아간다.
[log](https://travis-ci.org/github/if1live/libsora.so/jobs/742462085)

GitHub Actions에서는 최소한 2개의 Action을 쓰게 된다.

첫번째 Action인 [actions/checkout](https://github.com/actions/checkout)가 git 저장소를 받는다.
특별한 설정이 없으면 `$GITHUB_WORKSPACE`에 클론 받는다.
그리고 `$GITHUB_WORKSPACE`의 기본값은 `/home/runner/work/my-repo-name/my-repo-name`이다.
https://docs.github.com/en/free-pro-team@latest/actions/reference/environment-variables

두번째 Action인 [setup-go](https://github.com/actions/setup-go)가 golang을 설정했다.
GOPATH는 `/home/runner/go`로 설정된다.

기본값을 쓰면 클론받은 경로가 `GOPATH/src` 밖에 있어서 dep가 터진다.
[log](https://github.com/if1live/if1live.github.io/runs/1374760394?check_suite_focus=true)

GitHub Actions 설정값 바꾸는 삽질할바에 dep 버리고 Go Modules를 써보기로 했다.
go modules를 처음 써봤는데 이건 `GOPATH` 설정 안해도 잘 돌아가더라?
그래서 GitHub Actions, Go Modules가 동시에 적용되었다.

## 마무리

내가 안바꿔도 세상이 바뀌다보니까 내가 끌려다닐 수 밖에 없더라.
로그 뒤져보니 1년에 한번씩 빌드 시스템 손보는거같더라.
2020년도 이제 끝나가니 2022년쯤에 빌드 시스템 고장나고 손보겠네.

GitHub Actions 좋다.
남이 짠 Action 대충 끼워넣어도 잘 굴러간다.
travis-ci 쓰던 시절에는 생성된 HTML을 git으로 푸시하는 스크립트를 직접 짜서 사용했다.
[Push directory to another repository](https://github.com/marketplace/actions/push-directory-to-another-repository)로 바꿨는데 훨씬 간단하더라.

Go Modules은 제정신으로 만들어졌다.
GOPATH 설정은 golang의 진입장벽이었다. go modules로 바꾸니까 GOPATH 신경 안써도 돌아가더라.
드디어 golang이 멀쩡한 언어가 되었다. 물론 나는 안 쓸거지만.

[dep-repo]: github.com/golang/dep
[jekyll-repo]: https://github.com/jekyll/jekyll
[pelican-repo]: https://github.com/getpelican/pelican
