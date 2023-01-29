---
title: 나의 백엔드 배포 환경 정책
subtitle: 어쩌다 이렇게 되었는가?
tags: [staging, stage, deployment]
slug: my-backend-deployment-environment-policy
author: if1live
date: 2020-01-02T00:00:00+09:00
url: /posts/my-backend-deployment-environment-policy
---

배포 환경([Development environment][wiki-deployment-environment])은 목적에 따라서 각각 다른 스테이지로 소프트웨어 배포하는걸 말한다.
예시로 설명하면 쉬운데 말로 쓰니까 어렵다.
개발 서버와 프로덕션 서버를 분리해서 실수로 개발 서버 말아먹어도 서비스에는 영향이 가지 않도록 만드는 것을 뜻한다.

wikipedia 문서를 참조하면 남들은 아래와 같이 환경으로 나눈다고 카더라.

* local
* development
* integration
* testing
* staging
* production

내 경우는 아래와 같이 환경을 나눴다

| stage | dotenv |
|-------|--------|
| (local) | development |
| dev | unstable |
| nightly | nightly |
| devqa | devqa |
| staging-1 | staging-1 |
| staging-2 | staging-2 |

왜 이 꼴이 되었는지 자기합리화하는 글을 써보았다.

## stage vs dotenv

serverless framework는 `--stage` 옵션을 통해서 스테이지를 설정할 수 있다.
기본 스테이지는 dev이다.

[serverless-dotenv-plugin][repo-serverless-dotenv-plugin]를 유용하게 쓰고있다.
serverless-dotenv-plugin은 `--env` 옵션을 받는다.
기본 설정파일은 `.env.development`, `.env`이다. 

둘은 기본값이 다르다.
각각 값을 설정할 수 있다.
이를 이용해서 stage, env을 따로 설정하는 이상한 짓을 했다.
(남들은 이렇게 안할거같은데)

## local
개발자 컴퓨터에서의 스테이지이다.
배포하기 전에는 스테이지에 의미가 없기 때문에 어디에도 명시되지 않은 스테이지이다.
dotenv는 `.env.development`를 사용한다.
serverless-dotenv-plugin에서의 기본값이기 때문이다.

## dev
master로 커밋할때마다 배포되는 스테이지이다.
serverless framework의 기본 스테이지인 dev를 유지했다.
[dotenv][repo-dotenv] 이름은 뭐로 할지 한참 고민했었다.
`.env.development`를 이미 local에서 쓰고있다.
`.env.dev`는 `.env.development`랑 비슷해서 실수할 가능성이 높다.

development랑 비슷하지 않으면서 개발 버전의 느낌이 나는 이름이 뭐가 있을지 고민했다.
그러다 [debian unstable](https://wiki.debian.org/DebianUnstable) 릴리즈가 떠올라서 `.env.unstable`로 정했다.

## nightly
nightly는 매일, 몇시간마다 주기적으로 배포되는 스테이지이다.
dev는 매 커밋마다 배포되니까 불안정하다.
API에 새로운 필드가 생겼다 사라졌다 URL 바뀌고 등등이 계속 발생한다.
프론트엔드 개발이 서버의 dev 스테이지에 붙어있으면 자주 깨지더라.
한박자 늦게 배포할 목적으로 스테이지를 분리했다.

잘못된(?) 결정으로 인해 프론트엔드의 dev 스테이지는 백엔드의 nightly 스테이지와 연결되었고 처음 보는 사람을 헷갈리게 만들었다.

## devqa
devqa는 내부 QA용 스테이지이다.
dev는 매 커밋마다 서버가 바뀐다.
nightly는 주기적으로 서버가 바뀐다.
QA가 진행되는 동안에는 빌드가 고정되어야하니까 스테이지를 분리했다.

스테이지 이름을 qa로 했다간 사내 QA와 외부 자원(예: 퍼블리셔)으로 돌아가는 QA가 동시에 존재할 경우 이름이 헷갈릴 수 있다.
그래서 devqa로 했다.

## staging-x
마일스톤을 찍을때마다 배포되는 스테이지이다.
안정버전인 빌드가 올라가며 외부 시연용으로도 쓰인다.

남들을 staging을 1개 쓰는데 2개가 되어버린 것에는 역사적(?) 이유가 있다.
마일스톤 마감일과 외부 시연하려고 빌드 들고간 날이 겹친 적이 있다.
마일스톤 찍었으니 스테이징 배포는 해야겠는데 기존 스테이징 환경을 건드렸다가 외부 시연이 실패하는건 말이 안된다.
그렇다고 마일스톤 마감 일정을 미루는건 패배하는 기분이었다. 이런 일은 앞으로 몇번이나 더 발생할 수 있으니까.

그래서 staging 스테이지를 둘로 나누기로 했다.
홀/짝으로 교대로 배포하고 있다.
(milestone-9는 staging-1, milestone-10은 staging-2)

staging 스테이지를 2개로 나눈 덕분에 얻은 이점도 있다.
최신 마일스톤과 직전 마일스톤이 공존한다.
동시에 돌려서 무엇이 개선되었는지 비교할 수 있다.

[wiki-deployment-environment]: https://en.wikipedia.org/wiki/Deployment_environment
[repo-serverless-dotenv-plugin]: https://github.com/colynb/serverless-dotenv-plugin
[repo-dotenv]: https://www.npmjs.com/package/dotenv
