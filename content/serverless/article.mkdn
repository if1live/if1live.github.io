---
title: serverless framework plugins
subtitle: 내가 쓰는 플러그인과 이유
tags: [serverless]
slug: serverless-framework-plugins
author: if1live
date: 2020-01-01
url: /posts/serverless-framework-plugins
---

## 개요
serverless framework은 aws lambda 같은 서버리스 플랫폼에 배포할때 유용한 도구이다.
하지만 serverless framework를 그대로 쓰는건 불편하다.
다행히도 serverless에는 플러그인이 많이 있고 적절한걸 붙이면 많이 편해진다.

내가 사용 중인 serverless 플러그인을 정리해봤다.
사용법은 각각의 README를 참고하면 되기때문에 다루지 않았다.

## [serverless-webpack][serverless-webpack]

나는 타입스크립트를 사용한다.
serverless에서 타입스크립트 사용하는 방법 찾다가 [serverless-plugin-typescript][serverless-plugin-typescript]를 찾아서 사용했다.
serverless-plugin-typescript는 소개 첫줄부터 zero-config 를 강조하더라.
플러그인만 설치해도 타입스크립트를 쓸 수 있더라.

나중에 더 배우고나니까 serverless가 타입스크립트를 기본적으로 지원하더라.
`serverless create --template aws-nodejs-typescript --name xxx` 로 프로젝트를 생성시킬 수 있다.
템플릿으로 생성한 프로젝트는 serverless-webpack을 사용하거더라.

지금은 serverless-plugin-typescript를 걷어내고 serverless-webpack를 쓰고있다.

1. webpack은 빌드 과정을 손댈 수 있다. 설정 손대지 않고 프로덕션까지 가는건 쉽지 않다.
2. webpack은 플러그인이 많다. webpack 생태계에 묻어가면 편하다.

## [serverless-dotenv-plugin][serverless-dotenv-plugin]

express 쓰던 시절부터 [dotenv][dotenv]는 열심히 썼다.
serverless 환경에서도 비슷한게 있어서 갖다썻다.

## [serverless-domain-manager][serverless-domain-manager]

aws lambda로 서비스를 배포하면 도메인이 할당된다.
`https://xxxxxxxxxx.execute-api.ap-northeast-2.amazonaws.com/`
서비스를 지웠다가 다시 배포하면 새로운 도메인이 할당된다.
문제 생겨서 서비스 지웠다 다시 만들면 바뀌며 생긴게 이쁘지 않은 도메인을 프로덕션에서 사용하고 싶진 않았다.

api gateway, route53을 손봐서 사용자 지정 도메인을 연결할수 있다.
하지만 서비스가 한두개도 아니고 스테이지도 한두개가 아니라서 일일히 연결하는건 사람 손으로 할 짓이 아닌거같더라.
도메인 연결을 자동화 시키려고 플러그인을 설치했다.

## [serverless-layers][serverless-layers]

[AWS SAM을 이용한 모놀리식 서버리스 어플리케이션 운영하기](https://www.slideshare.net/changhoonhyun/aws-sam)
나도 마이크로서비스의 길을 포기하고 모놀리식(monolithic) 아키텍처로 구현하고 있다.

저장소 하나에 모든 구현을 때려박고 있으니까 package.json이 점점 거대해지고 있다.
typeorm, mysql, express, ... 의존성이 늘어나고 배포 용량도 커지고 있다.
나중에는 배포 한번 할때마다 15MB~20MB가 배포되더라.

내가 짠 코드는 끽해야 100~200kb 밖에 안되고 나머지는 전부 라이브러리이다.
라이브러리는 자주 바뀌지 않는 부분이니까 배포에서 이를 분리하고 싶었다.

serverless-layers를 도입해서 문제를 해결했다.
package.json의 내용이 바뀌었을때는 배포 용량이 20MB이지만 평소에는 1MB보다 훨씬 작다.

## [serverless-offline][serverless-offline]

serverless로 처음 만든 함수는 REST API였다.
근데 순정 serverless에는 웹서버를 띄울 수 있는 기능이 없다.
내가 만든 REST API를 테스트 하기 위해서는 aws lambda에 배포해야 된다.
로컬에서 테스트 하려면 invoke local을 사용해야 되는데 이것은 매우 불편하다.

serverless-offline이 붙으면 모든게 달라진다.
serverless framework를 로컬에 웹서버로 띄울 수 있다.
개발 과정이 훨씬 편해진다.

가끔 serverless-offline의 동작과 실제 aws lambda의 동작이 달라서 엿먹을때가 있지만 serverless-offline이 없는 것보다는 훨씬 좋다.

## [serverless-offline-sns][serverless-offline-sns]

localstack을 이용하면 SNS(Simple Notification Service) 를 로컬에 띄워서 테스트할 수 있다.
하지만 나는 docker같은 외부 의존성 없이 SNS 관련 기능을 구현하고 싶었다.
serverless-offline이 있으니까 SNS도 offline 구현체가 있지 않을까 싶어 뒤져보니까 있더라.

## [serverless-offline-sqs][serverless-offline-sqs]

비동기로 작업을 처리한후 SNS로 알림을 보내야하는 기능을 얼마전에 구현했다.
aws lambda를 무식하게 사용하면 이렇게 구현할 수 있다.

1. aws lambda 시작
2. 비동기 명령 보내기
3. x초 대기... 명령이 처리되었는가?
4. 처리되지 않았으면 다시 대기...x초 대기후 완료되었는지 확인
5. 완료되었으면 SNS로 이벤트 보내기
6. aws lambda 종료

이는 무식한 구현답게 비용 측면에서 비효율적이다.
람다는 사용 시간 자체가 비용이다.
결과가 끝났는지 확인하기 위해서 람다가 계속 살아있어야한다.
결과가 끝날때까지 기다리는 루프를 다른 식으로 구현하고 싶었다.

SQS(Simple Queue Service)의 delay를 이용해서 구현하면 아래와 같다.

1. aws lambda 시작
2. 비동기 명령 보내기
3. SQS로 x초 뒤에 처리되는 메세지 넣기
4. aws lambda 종료
5. ...(x초 대기)
6. aws lambda 시작 (sqs에 의해서 호출)
7. 처리되지 않았으면 SQS로 x초 뒤에 처리되는 메세지 넣기
8. 완료되었으면 SNS로 이벤트 보내기
9. aws lambda 종료

대기하는 동안은 돌아가는 aws lambda 함수가 없으니까 과금도 없다.
SQS 역시 SNS처럼 로컬 구현이 있나 뒤져보니까 있더라.

## 부록: aws-serverless-express
[aws-serverless-express][aws-serverless-express]는 serverless framework 플러그인은 아니지만 유용하게 쓰고 있다.
aws-serverless-express 관련된 글을 많이 있어서 직접 다루진 않는다.
어차피 남들도 나와 비슷한 이유로 쓰고 있을테니까.

https://devstarsj.github.io/cloud/2017/08/06/aws-serverless-express/

[serverless-framework]: https://serverless.com/
[serverless-plugins]: https://serverless.com/plugins/
[dotenv]: https://github.com/motdotla/dotenv
[serverless-domain-manager]: https://github.com/amplify-education/serverless-domain-manager
[serverless-dotenv-plugin]: https://github.com/colynb/serverless-dotenv-plugin
[serverless-plugin-typescript]: https://github.com/prisma-labs/serverless-plugin-typescript
[serverless-webpack]: https://github.com/serverless-heaven/serverless-webpack
[serverless-offline]: https://github.com/dherault/serverless-offline
[serverless-offline-sns]: https://github.com/mj1618/serverless-offline-sns
[serverless-offline-sqs]: https://github.com/CoorpAcademy/serverless-plugins/tree/master/packages/serverless-offline-sqs
[serverless-layers]: https://github.com/agutoli/serverless-layers
[aws-serverless-express]: https://github.com/awslabs/aws-serverless-express
