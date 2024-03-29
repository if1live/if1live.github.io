---
title: yup로 입력 검증을 덜 귀찮게 처리하자
subtitle: 선언형 입력 검증 라이브러리
tags: [library, javascript, typescript]
slug: yup-declarative-input-validation
author: if1live
date: 2019-01-19
url: /posts/yup-declarative-input-validation
---


## 개요
프로그래머가 러스트, C#, 타입스크립트같은 타입 있는 언어로 코드를 짜면 컴파일러에 의해서 타입이 보장되니 런타임에서 문제가 생기지 않는다.
미개한 스크립트 언어들만이 런타임에서 타입으로 문제가 생긴다.
그러니까 우리는 정적 언어를 써야한다.

![그렇게 생각했던 시기가](e0080266_51714c9299ca8.jpg)

물론 세상은 그렇게 굴러가지 않는다.
프로그래머가 코드를 완벽하게 통제하고 있을수는 있어도 외부에서의 입력까지 완벽하게 통제할수는 없다.
특히 인터넷에 연결된 서버의 경우는 밖에서 어떤 입력이 들어올지 알 수 없다.
그렇기 때문에 입력 검증은 필요하다. 

## 선언형 입력 검증

입력 검증은 뻔한 부분이 많다.
이런 패턴이 흔히 등장한다.

* 비어있을 수 없다.
* 들어갈 수 있는 값은 A/B/C 중 하나
* 이메일 필드
* 숫자 필드
* 최대 255 글자
* ...

입력 검증을 직접 구현하는건 너무 귀찮고 선언적으로 때울 방법 어디 없나?
이런 식으로 돌아가는 느낌으로.

1. validator를 생성한다. 이때 각각의 필드별로 어떤 속성이 가능한지 명시한다.
    * name: 문자열 타입, 최대 16글자
    * age: 숫자 필드, 최소 18
    * 대충 이런 느낌으로 필드별로 정의
2. 검증하고 싶은 값을 묶어서 객체로 만든다.
3. 생성된 객체를 validator에 통과시키면 성공/실패를 알 수 있다.

## yup

이전에 GraphQL에 대해서 찾아보다가 [fullstack-graphql-airbnb-clone][repo-graphql] 라는 저장소를 뜯어본 적이 있다. 
그때 [yup][repo-yup] 라는 입력 검증 라이브러리를 알게 되었다.

yup를 쓰면 아래와 같은 느낌으로 입력 검증이 가능하다.

```javascript
var yup = require('yup');

var schema = yup.object().shape({
  name: yup.string().required(),
  age: yup
    .number()
    .required()
    .positive()
    .integer(),
  email: yup.string().email(),
  website: yup.string().url(),
  createdOn: yup.date().default(function() {
    return new Date();
  }),
});

//check validity
schema
  .isValid({
    name: 'jimmy',
    age: 24,
  })
  .then(function(valid) {
    valid; // => true
  });

//you can try and type cast objects to the defined schema
schema.cast({
  name: 'jimmy',
  age: '24',
  createdOn: '2014-09-23T19:25:25Z',
});
// => { name: 'jimmy', age: 24, createdOn: Date }
```

비슷한 컨셉의 라이브러리로 [joi][repo-joi] 라는것도 있더라.
github star 기준으로는 joi가 더 많더라.
하지만 joi랑 yup랑 선언형 스키마라는 개념은 비슷하길래 나는 쓰던거 쓴다.

## 내가 적용한 코드

요새 서버를 typescript + express로 구현하고 있다.
입력은 request body에 json을 붙여서 받고있다.
하지만 body로 들어온 json이 올바른 내용인지는 검증하지 않으면 믿을 수 없다.
여기에 yup를 붙였다.

```typescript
const allowedAuthTypes = [
	'twitter',
	'facebook',
];
export const connectAuthSchema = yup.object().shape({
	auth_type: yup.string().oneOf(allowedAuthTypes).required(),
	user_id: yup.string().required(),
});

const connectAuth = async (req: express.Request, res: express.Response) => {
	await connectAuthSchema.validate(req.body, { abortEarly: true });
	const resp = await connectAuth(new MyRequest<M.ConnectAuthReq>(req));
	res.json(resp);
};
```

## 직접 써보니까

선언형 입력 검증 라이브러리를 쓰고 크게 2가지를 얻은거같다.

입력 검증과 관련된 코드는 줄어들었고 신뢰도는 올라갔다.
입력 검증을 직접 구현했다면 입력 검증이 일종의 로직이 되니까 이를 위한 테스트가 필요하다.
(테스트가 없다면 내가 짠 검증이 제대로 검증하는지 어떻게 믿을 수 있나?)
그리고 직접 입력 검증을 구현했다면 yup schema보다는 코드가 길었을것이다.
yup 덕분에 직접 로직을 작성한 것은 거의 없으니 테스트를 덜 신경쓰게 되었다.

클라이언트 개발자에 입력값이 무엇인지 알려주기 쉬워졌다.
입력 검증이 선언형 코드로 구성되니 무엇을 넣을수 있는지 한눈에 보인다.
그래서 API 문서를 안쓰기로 했다. (???)

[repo-graphql]: https://github.com/benawad/fullstack-graphql-airbnb-clone
[repo-yup]: https://github.com/jquense/yup
[repo-joi]: https://github.com/hapijs/joi
