---
title: 프리티어 서비스가 잠들지 않게 하기, 그리고 discord의 가능성
subtitle: ayane 개발 포스트모템
tags: [serverless, free tier, discord]
slug: ayane-post-mortem
author: if1live
date: 2023-12-24T00:00:00+09:00
url: /posts/ayane-post-mortem
---

[github](!https://github.com/if1live/ayane)
![ayane](ayane-pv.jpg)

## 목표

### free tier의 대가: sleeping

[Supabase](https://supabase.com/), [PlanetScale](https://planetscale.com/)은 무료 서비스를 제공하는 좋은 물건이다. 하지만 무료에는 대가가 있다.

> Free projects are paused after 1 week of inactivity.
> https://supabase.com/pricing

> When a database on the free Hobby plan is inactive for 7 days, the database will be enter sleep mode.
> https://planetscale.com/docs/concepts/database-sleeping

무료 유저의 경우, 요청이 들어가지 않으면 잠들어버린다.
아무 요청이나 지속적으로 서비스가 잠들지 않도록 하는게 필요하다.

옛날 옛적 heroku에서 무료 서비스를 제공하던 시절, 무료 서버는 30분동안 요청이 없으면 잠들었다.
서버가 잠들지 않게 하려고 여러가지 방법을 사용했다.
[8 No Brainer Ways to Keep Your Heroku App Awake](https://genicsblog.com/gouravkhunger/8-ways-to-keep-your-heroku-app-awake)
그 시절에 하던 기법을 다시 꺼낸 것이다.

### 명시적 비용: 0

supabase, planetscale 같은 서비스를 무료로 쓰려고 비용을 지불하는건 말이 되지 않는다.
그럴 돈이 있었으면 supabase, planetscale을 유료로 쓰고말지.
무료 서비스를 잠들지 않게 하는 서비스는 명시적 비용 없이 작동해야 한다.

### 죽지 않는 서비스

supabase에서는 2개의 free organization를 만들 수 있다.
planetscale에서는 1개의 무료 데이터베이스를 만들 수 있다.
사이드 프로젝트마다 독립된 데이터베이스를 할당하면 비용이 늘어난다.
비용을 0으로 유지하려면 많은 사이드 프로젝트가 하나의 DB를 공유해야 된다.
DB가 잠들지 않게 만드는 서비스가 죽어서 DB가 잠들어버리면 DB를 사용하는 모든 서비스가 죽어버린다.
무료 서비스를 잠들지 않게 하는 서비스는 안정적으로 작동해서 웬만해서는 죽으면 안된다.

### 크래시 리포트가 오지 않으면 크래시 리포트 보내는 곳이 크래시

지금까지 수많은 프로젝트를 진행하면서 똑같았던게 있다.
크래시 리포트가 오지 않으면 크래시가 없을 정도로 잘 짠게 아니다.
크래시 리포트를 보내는 지점이 고장나서 크래시 리포트가 오지 않는거다.
실제로는 서비스가 죽어서 리포트를 못보내는건데 문제가 없어서 리포트가 안오는거로 착각하면 안된다.

일반적인 웹서버는 요청-응답 구조로 이루어진다.
서버가 죽으면 요청을 보내도 응답이 제대로 오지 않는다.
서버가 죽었다는걸 쉽게 알 수 있다.

스케줄같이 뒤에서 돌아가는 물건이라면?
이런 물건은 죽어도 죽었다는게 잘 보이지 않다.
서버가 죽고나서 한참 뒤에나 뜬금없는 문제가 생기고 문제를 파보다가 서버가 죽었다는건 발견할지 모른다.

이런 문제를 피하고 싶어서 매일 퇴근 시간에 리포트를 보내게 만들고 싶다.
리포트를 받으면 퇴근할 시간, 리포트가 안오면 집가서 디버깅할 시간

내가 만든 서비스가 충분히 안정적이라고 해도 외부 서비스에 얽힌 물건은 언제 터질지 모른다.
supabase, planetscale에서 무료 티어 정잭을 바꾼다면 내 서비스는 고장날 수 밖에 없다.
고장난걸 알 수 있어야 고칠 수 있다.

## 선택과 구현

* 주기적으로 호출되어야 한다.
  * [AWS Schedule](https://www.serverless.com/framework/docs/providers/aws/events/schedule)
  * 람다 함수를 하루에 한번 호출
* 명시적 비용은 0이어야 한다.
  * 하루에 한번 호출되는 수준으로 AWS Lambda에서 비용이 발생하지 않는다.
* 죽지 않아야 한다
  * AWS Lambda로는 내가 원하는 수준의 안정성이 나온다.
* 서비스에 문제가 생긴걸 받아볼 수 있어야 한다.
  * 디스코드를 알림 수단으로 쓴다
  * 직접 구현하는 것보다 디스코드가 안정적이고 게다가 공짜

ayane에는 기술적으로 대단한건 없다.
serverless framework로 배포되는 람다 함수이다.
serverless.yml에 schedule 설정해서 매일 특정 시간에 람다 함수가 호출된다.
람다 함수에서 supabase, planetscale 같은 곳으로 요청을 보내서 서비스가 잠들지 않게 한다.
실행 결과를 디스코드로 보내서 문제가 있는지 확인할 수 있다.

## discord의 다양한 활용

옛날이었으면 free push notification api로 검색해서 적당한거 하나 썼을거다.
[Pushbullet](https://www.pushbullet.com/), [Pushover](https://pushover.net/)를 써본 적도 있고.
하지만 이번에는 디스코드를 선택했다.

개인용 디스코드 서버를 파서 메세지를 보내니까 푸시 알림이 잘 오더라.
개인용 push notification 대용으로 괜찮은데?

게다가 어떤 푸시를 날렸는지도 디스코드 채널로 기록이 잘 남는다.
이걸 응용하면 discord 채널을 DB로 쓸 수 있을거같은데?

디스코드를 붙이다보니 다양한 목적으로 응용할 수 있겠더라.
찾아보니 나랑 비슷한 생각을 하는 사람들이 많았다.

* Use discord server as databases
  * https://github.com/aiotter/discord_db
* Using Discord as your CDN
  * https://dev.to/grubba/using-discord-as-your-cdn-o4j

다음 플젝에서는 discord를 어떻게 활용해볼까?
