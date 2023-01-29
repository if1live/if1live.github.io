---
title: JWT를 사용해서 server-to-server API 없애는 안티패턴
subtitle: 인증, 로그인과 관계없는 JWT 이야기
tags: [anti-pattern, jwt]
slug: anti-pattern-using-jwt-to-remove-server-to-server-api
author: if1live
date: 2022-08-22
url: /posts/anti-pattern-using-jwt-to-remove-server-to-server-api
---

오늘 SNS에서 JWT, OAuth, access token, refresh token에 대해서 이야기하는걸 봤다.
나는 보안에 대해서는 무식해서 OAuth, access token, refresh token 로는 아무 이야기도 할 수 없다. 하지만 JWT로는 어느정도 떠들 수 있다.

[JWT][jwt-io]를 구글에서 검색하면 대부분이 인증, 보안, 로그인, ... 과 관련된 글이 나온다.
인증에만 집중해서 JWT의 진짜 가치를 보지 못하는 현실이 안타까워서 글을 써본다.

???

## JWT(JSON Web Tokens)의 특징

[JWT][jwt-io]의 소개를 인용한다.

> JSON Web Tokens are an open, industry standard RFC 7519 method for representing claims securely between two parties.

JWT는 스펙이 공개되어 있으며 간단하다. 웬만한 언어로는 JWT 구현체가 있다.
JWT와 비슷하면서 JWT보다 좋다고 하는게 있는데 언어에 따라서는 구현체가 없을 수 있다.
예를 들어, [Biscuit](https://www.biscuitsec.org/)라는 물건은 [hex.pm](https://hex.pm/)에서 검색하면 아무것도 없다.
비즈니스 로직 구현하기도 바쁜데 토큰 라이브러리부터 내가 직접 구현해야 되나?

JWT는 토큰의 내용을 뜯어서 어떤 JSON이 들어있나 확인하기 쉽다.
토큰을 읽는건(decode) JWT의 두번째 부분을 base64 디코딩하면 되니까 누구나 할 수 있다.
(https://jwt.io/ 에 토큰을 복붙해도 내용물을 볼 수 있다)

JWT는 토큰을 변조할 수 없다.
토큰의 내용물을 고쳐도 비밀키를 모르면 똑같은 토큰을 만들 수 없다.
비밀키가 유출되었으면 이미 망한거니까 신경쓰지 않아도 된다.

## 시나리오: server-to-server API

실제 상황에서 있을지도 모르는 시나리오를 만들어봤다.
처음에는 일반적인 방법(?)으로 설계해본다.
다음에는 JWT를 사용해서 놀라운 형태(?)로 바꿔본다.

* 프로젝트 구성
	* 유저 서비스
		* 유저 DB를 관리 (예: 닉네임, 레벨, ...)
	* 파티 서비스
		* 파티에 참가한 유저끼리 채팅, 게임을 할 수 있다.
		* 파티원들끼리는 닉네임, 레벨이 보여야 한다.
		* 파티에 참가할때 자신의 정보를 넣어줘야한다.
* 목표: 파티 참가 버튼을 누르면 파티에 참가한다.

### 설계A: DB 공유

```mermaid
sequenceDiagram
	client->>+party: join_party(room_id, player_id)
	party->>+db: get_player(player_id)
	db->>-party: player_data
	party->>-client: ok
```

1. 클라가 파티 서버로 요청
2. 파티 서버는 공용DB에 접근해서 플레이어 정보를 얻는다
3. 파티 방에 참가하는 로직을 수행
4. 성공 응답

공유DB를 쓰면 귀찮은 문제를 대부분 해결할 수 있다.
하지만 실제 상황에서는 공유 DB때문에 귀찮은 문제가 생길 수 도 있다.
유저 서비스의 요구사항이 변경되서 DB스키마가 변경되었을때 파티 서비스가 고장날지 모른다.

### 설계B: server-to-server API

```mermaid
sequenceDiagram
	client->>+party: join_party(room_id, player_id)
	party->>+user_service: get_player(player_id)
	user_service->>-party: player_data	
	party->>-client: ok
```

1. 클라가 파티 서버로 요청
2. 파티 서버가 유저 서비스로 요청
3. 유저 서비스가 DB로 쿼리를 날려서 플레이어 정보를 얻는다.
4. 얻은 정보가 적절히 파티 서버에 전달된다
5. 성공 응답

DB스키마가 수정되더라도 유저 서비스 API가 유지되면 파티 서비스는 동작한다.

설계A의 경우는 문제가 생겼을때 파티 서비스 로그만 열여봐도 충분했다.

설계B의 경우는 두개의 서비스중 하나라도 고장나면 전체가 고장난다.
둘중 하나가 고장나도 전체가 고장나는건 어쩔수 없다.
근데 클라 입장에서는 파티 서비스가 고장난건지 유저 서비스가 고장난건지 알 수 없다. 문제가 생겼을때 어디에서 고장났는지 파악하기 쉬우면 디버깅하기도 쉬울 것이다.

### 설계C: JWT

파티 참가에 사용될 정보(ex: 닉네임, 레벨, ...)가 프로필 화면에서도 볼 수 있는 정보라고 가정하자. 플레이어가 알아도 되는 정보라면 숨길 필요가 없다.

플레이어가 임의로 데이터를 바꾸는건 안된다.

디버깅하기 어렵다는게 설계B의 단점이었다.

어????? JWT 쓰면 뭔가 될거같은데?????

```mermaid
sequenceDiagram
	client->>+user_service: get_player(player_id)
	user_service->>-client: jwt
	client->>+party: join_party(room_id, jwt)
	party->>-client: ok
```

1. 클라가 유저 서비스로 요청해서 jwt로 인코딩된 정보를 얻는다
2. 클라가 이전에 얻은 jwt를 파티 서비스로 전달한다
3. 파티 서비스는 jwt를 뜯어서 정보를 얻는다
4. 성공 응답

유저 서비스가 고장나면 JWT를 얻을 수 없다.
파티 서비스가 고장나면 join_party가 실패할 것이다.
클라 로그를 열어서 어느 시점에 문제가 생겼는지 확인하면 어떤 서비스에 문제가 있는지 알 수 있다.
파티 서비스로 보낸 요청이 터졌을때 JWT를 뜯으면 어떤 입력에서 터지는지 쉽게 확인할 수 있다.

JWT를 사용해서 서버의 복잡한 구현을 통신 시퀀스의 복잡함으로 바꿨다.

## anti-pattern

JWT를 응용할수 있는 시나리오를 준비한거지 모든 상황에서 이걸 쓸 순 없다.
이 설계를 사용하면 새로운 문제가 발생한다.

1. 요구사항 분석을 통해서 클라에 노출되어도 상관없는 정보인지 확인해야한다.
2. 레이턴시가 길어진다.
	* 서버가 같은 리전에 있다면 server-to-server API는 빠르다.
	* server-to-server API를 클라가 대행하는 구조라서 실행 시간이 길어진다.
3. 트랜잭션이 필요한 경우 문제가 복잡해진다.
	* 설계A는 클라의 API 요청 한번으로 끝난다
	* 설계C는 클라가 두번의 API 요청을 보내야한다.
	* 근데 설계B라고 트랜잭션이 간단한건 아니다. server-to-server API 성공후에 로직 버그가 있다면?

은총알은 없다.

[jwt-io]: https://jwt.io/
