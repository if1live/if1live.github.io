---
title: 사람 생각하는건 비슷하다
subtitle: 나만 틀린건 아니었나보다
tags: [thinking]
slug: peoples-thoughts-are-similar
author: if1live
date: 2019-01-30
url: /posts/peoples-thoughts-are-similar
---

최근에 [클라이언트-서버 웹 앱 만들기 - 자바와 자바스크립트로 배우는 모던 웹 앱 개발][yes24-book]이라는 책을 읽었다.
도서관 돌아다니다보니 손에 잡혀서 읽어봤다.

![book cover](41548950.jpg)

8장 API 디자인에 재밌는 내용이 나오더라.

> 원래는 보기 좋은 URL도 요청 매개변수 때문에 '보기 흉한' URL이 되기도 한다.
> 어떤 상황에서는, HTTP GET에 해당하는 듯한 동작을 하면서도 계층적 데이터 구조로 정의하는 것이 더 쉬운 요청 객체를 사용해야 할 수도 있다.
> 한 예로, 많은 항목을 GET으로 조회해 가져오는데, 각 항목을 찾기 위해서는 여러 필드가 필요한 상황이 있겠다.
> 서버가 요청 내용을 파싱하는 공식적인 요건이 없으므로,
> 당장은 여러 사례에서 쓰이더라도 장기적인 솔루션으로 사용하면 안 되겠지만,
> 로이 필딩과 HTTP 명세 (http://yhoo.it/1hfR4SD) 는 이를 허용하는 것 같다.

내가 밟은 삽질이 생각나더라.

[HTTP GET 요청에 body를 붙여서 보내면 어떤 일이 벌어질까?]({{< ref http-get-request-with-body-and-http-library >}})

사람 생각하는건 다 거기서 거기인가보다.

[yes24-book]: http://www.yes24.com/24/goods/14914156?scode=032&OzSrank=1
