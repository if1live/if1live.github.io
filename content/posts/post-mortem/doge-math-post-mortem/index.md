---
title: Doge Math 개발 포스트모템
subtitle: You do the math! You do the code!
tags: [postmortem, doge math]
slug: doge-math-post-mortem
author: if1live
date: 2015-09-17
url: /posts/doge-math-post-mortem
---
## 개요
2015/09/07 ~ 2015/09/12 동안 뻘짓 개발한 개인 플젝을 정리하기로 했다.
지금까지는 개인 프로젝트에 대해서 기록을 남기지 않았지만 이번에는 몇가지 이유로 기록을 남겨볼까한다.

1. 짜잘하게 만든게 많으니까 나중에 뭘 만들었는지도 기억이 나지 않더라 (....)
2. 옛날에 분명 삽질했던건데 기억이 안나서 또 삽질했다 (...) 참고용 링크만 있어도 두번 삽질하는 일은 없을거다.
2. 간단한걸 만들었지만 생각보다 참고한게 많았다. 적어도 "헬로월드를 짰다. 재밌었다." 수준의 포스트모템은 안나오겠더라.
3. 이런거라도 안쓰면 글을 쓸 이유가 없다 (...)

이번에 만든 것은 doge-math이다. 저장소와 라이브 데모는 다음과 같다.

* [Live Demo][doge_math_site]
* [Repository][doge_math_repo]

## 왜 만들었는가?
> 포탈1은 2007년에, 포탈2는 2011년에 출시됐다고? 그럼 포탈3은 2015년에 나와야 되는 거 아니냐 [@_Yous][twitter_portal]

이런식의 수열 관련 문제가 나오면 나는 [적절한 짤][original_meme]로 반응한다.

![You do the math!](doge-math.jpg)

등차수열을 엿먹이는 목적으로는 괜찮은 짤이지만 짤의 값이 고정되어있다.
짤의 내용을 바꿀수 있으면 더 좋지 않을까? 이런 짤을 찍어낼수 있는걸 만들면 재밌지 않을까?
하는 생각에 doge math를 만들기로 했다.

## 목표
책읽을 시간, 게임할 시간, 애니볼 시간을 쪼개서 이걸 만드는거니까 최소한 이것만큼은 하겠다는 목표를 정했다.

가장 중요한 목표는 doge math 짤과 유사한 짤을 만들어낼수 있는 생성기의 구현이다.
doge math 짤을 보고 감동받아서 이 프로젝트를 시작한건데 이것만큼은 해야하지 않겠는가?
원작에 충실하게 만드는걸 기본 목표로 잡았다. 기본값은 원작과 동일한 값, 공식을 쓰도록 했다.

다음 목표는 서버 없이 모든걸 할수 있도록 만드는 것이다.
[짤 생성기](http://jjal.icou.kr/)라는 개념은 이미 존재한다.
하지만 기존의 대부분의 짤 생성기는 서버에서 짤을 생성해서 내려준다.
내 경우는 개인 서버를 운영하기에는 가난하고 서버가 있어도 그걸 유지할 수 있을 정도로 부지런하지 않다.
그래서 브라우저에서 모든것이 돌아가게 만들고 GitHub Pages로 호스팅 하기로 했다.

마지막 목표는 공유하기 쉬워야 한다는 것이다.
doge math 짤을 트위터에서 가끔 써먹었다.
공유하기 쉽게 만들면 내가 써먹기도 쉬울것이다.
또한 공유할수 있는 방법을 여러가지 제공하면 쓰기 편할것이라고 생각했다.

## 수학
Doge Math의 짤을 볼때 x의 차수가 높더라.
polynomial interpolation 을 이용한거같더라.
polynomial interpolation 중에서 제일 간단한 Lagrange polynomial를 고르고 초기값을 잘 조절하니까 원본과 동일한 수식을 얻을수 있더라.

[Lagrange polynomial][math_lagrange]만 구현하면 수학쪽은 문제 없겠다.
그런데 Lagrange polynomial 를 구현하려면 다항식 전개가 필요한데....

### 다항식 전개 + combination
(x+a)(x+b)(x+c)(x+d)를 전개해서 n차항의 계수를 계산하자.
코드로 다항식의 전개를 구현해야되니까 노가다가 많은건 상관없지만 규칙은 간단한게 좋다.
이때 조합(combination)을 이용하면 다항식의 전개를 쉽게 할 수 있다.

(x+a)(x+b)(x+c)(x+d)에서 4차항을 얻어낼수 있는 방법은 전부 x를 선택하는 것이다.
반대로 말하면 (a, b, c, d)중에서 아무것도 안고르는거다.
(x+a)(x+b)(x+c)(x+d)에서 2차항을 얻어낼수 있는 방법은 1개만 x를 선택하는 것이다.
반대로 말하면 (a, b, c, d)중에서 3개를 뽑아내는 경우의 조합이다.
이런 느낌으로 전개해보자.

* 0차항의 계수 : 4C4 = (abcd)
* 1차항의 계수 : 4C3 = (abc) (abd) (acd) (bcd)
* 2차항의 계수 : 4C2 = (ab) (ac) (ad) (bc) (bd) (cd)
* 3차항의 계수 : 4C1 = (a) (b) (c) (d)
* 4차항의 계수 : 4C0 = 1 (x항만 선택)

따라서 전개한 식은 다음과 같을 것이다.

`(x+a)(x+b)(x+c)(x+d) = x**4 + (a+b+c+d)x**3 + (ab+ac+ad+bc+bd+cd)x**2 + (abc+abd+acd+bcd)x + abcd`

[자바스크립트로 Combination을 구현한 코드][external_combination]를 이용해서 이를 구현했다.

## 공유
공유 방법은 크게 3가지를 생각하고 구현했다.

* 이미지를 이용한 공유
* URL을 이용한 공유
* 트위터를 통한 공유

### Download to PNG
Doge Math를 사용해서 유저가 원하는 형태의 짤을 만든 다음, 이를 다운받아서 SNS로 배포할 수 있으면 좋을거라고 생각했다.
그리고 HTML5 Canvas에 그림을 그리면 이를 이미지로 다운받는 것이 가능하다는걸 들은적이 있었다.
이것이 DOM을 쓰지 않고 Canvas를 사용한 가장 큰 이유이다.

* [HTML5 Canvas to PNG File][stack_overflow_download_canvas]

### Parse Query String
페이지 렌더링에 필요한 요소(질문, 수치등)를 전부 query string에 때려박는다.
그리고 페이지 진입시 query string 해석해서 초기값으로 쓰면 URL만으로 짤을 재현할수 있지 않을까?
이런 느낌으로 구현해봤는데 잘 돌아가더라.
단, query string이 비어있으면 빈 문자열이 아니라 기본값을 쓰도록 처리하는걸 잊으면 안된다.
(기본값 대응을 처음에 잊어버리고 구현 안했는데 query string이 없으니 전부 공백, NaN으로 되서 문제가 생기더라)

* [How can I get query string values in JavaScript?][stack_overflow_parse_query_string]

### Clipboard API
옛날옛적에는 브라우저에서 클립보드로 복사하는 것을 지원하지 않았다.
그래서 [ZeroClipboard][external_zeroclipboard] 같은 플래시 기반의 라이브러리를 사용해서 복사를 구현했다.
그런데 [요즘 자바스크립트][wd_clipboard_apis]로는 복붙이 되더라!
플래시를 가져다 붙여서까지 복붙을 만들고 싶은 생각은 없으니 브라우저에서 지원하는 Clipboard API를 쓰기로 했다.

* [HTML5 alternative to flash-based ZeroClipboard for safe copying of data to clipboard?][stack_overflow_clipboard]

우왕ㅋ굳ㅋ 이거면 모든게 해결되겠네!!!
같은 꿈과 희망은 역시 웹에 없었다.
크롬에서는 아무 문제가 없지만 파폭에서는 다음과 같은 에러가 나고 동작하지 않았다.

![firefox 40 clipboard API error](firefox-clipboard-error.png)

찾아보니까 [firefox 41부터 Clipboard API를 지원할 예정][mozilla_clipboard_error]이라고 하더라.
그리고 현재 파폭 안정버전은 40이다.
웹표준 문서를 써봤자 구현체가 지원하지 않으면 의미가 없다는걸 다시한번 느꼈다.
수정하기 귀찮아서 Doge Math에는 Clipboard API로 구현한 것을 그대로 사용했다.

### Twitter Card
Twitter Card는 이미 3번정도 구현한적 있어서 특별한 문제 없이 지나갔다.
너무 많이 구현해봐서 문제에 부딪치지 않으니 쓸게 없어서 재미없네.

### Twitter Button
트위터 버튼을 이용해서 유저가 링크를 공유하는 기능을 붙이기로 했다.
유저가 짤의 제목과 수치를 조정하게 그것에 맞춰서 트위터 버튼에 등록된 URL과 제목이 바뀌도록 구현하려고 했다.

트위터 버튼은 크게 2가지 방법으로 구현할수 있다.
첫번째 방법은 간단하게 HTML 태그를 사용하는 것이고 두번째 방법은 자바스크립트를 이용해서 버튼을 생성하는 것이다.

첫번째 방법을 먼저 사용해보았다. 다음은 트위터 버튼의 샘플 HTML이다.

```html
<a href="https://twitter.com/share"
  class="twitter-share-button"
  data-url="https://if1live.github.io"
  data-text="this is title"
  data-via="if1live">Tweet</a>
<script>.....</script>
```

유저가 Doge Math의 필드값을 고치면 동적으로 트위터 버튼 HTML의 `data-url`과 `data-text`를 조정하면 원하는걸 구현할수 있겠더라.
하지만 구현하니 생각치도 못한 문제가 발생했다. 트위터 버튼을 눌렀을때 팝업 경고창이 뜨더라.
원래 트위터 버튼을 누르면 팝업 경고창 없이 팝업이 바로 뜬다.
동적으로 HTML 문서를 손댔기때문에 팝업창 이슈가 생긴거로 보여서 다른 방법을 쓰기로 했다.

자바스크립트를 이용해서 트위터 버튼을 생성해보기로 했다.
하지만 이 방법도 문제가 있었다.
트위터 버튼을 자바스크립트로 생성하면 페이지에 바로 보이는게 아니라 트위터에서 버튼 DOM을 비동기로 긁어서 화면에 보여준다.
Doge Math의 수치를 조정할때마다 버튼이 사라졌다 새로 생겼다 하는 식으로 작동했다. 이것 역시 의도한 동작이 아니어서 포기했다.

그래서 결국 트위터 버튼의 URL와 트윗 텍스트는 고정시켜놨다.
이제와서 생각해볼때 트위터 버튼의 트윗 횟수를 제대로 표시하려면 동적으로 건들면 안되겠더라.

## 디자인
디자인없이 쌩 HTML로 보여주면 없어보이고 그렇다고 새로운 디자인을 만들자니 귀찮더라.
그래서 [블로그 테마](https://if1live.github.io)를 뜯어다 붙였다.
블로그 테마가 재사용하기 쉬운 형태로 만들어져있지 않아서 삽질을 했는데
Doge Math와 비슷한 페이지를 나중에 만들거 같으니 이번 기회에 정리해야겠더라.
(이미 후속작(?) 컨셉 몇가지도 머릿속에 있고)

### Comic Sans
Doge 짤이라면 폰트를 [Comic Sans][wiki_comic_sans]으로 하는것이 정석이다.
문제는 Comic Sans는 마이크로소프트에서 만든 폰트라서 안드로이드, 아이폰에서는 제대로 나오지 않는다.
그래서 Comic Sans 분위기의 다른 폰트를 가져다 사용했다.

무료 폰트가 많은 [Font Squirrel][free_font]에서 ComicRelief 폰트를 가져다 사용했다.
한글 렌더링은 신경 안쓰니 고를수 있는 폰트가 많아서 좋았다.
(개인적으로 한글 지원 웹폰트를 싫어한다. 그것들은 네트워크 트래픽을 너무 많이 잡아먹어서 모바일에서는 못쓴다)

## 서버가 느려!

정적 웹페이지로 만들거니까 특별한 웹서버는 필요 없었다.
폴더 하나를 특정 포트로 웹서빙 할수만 있으면 문제 없다.
그렇게 생각하던 시절이 저한테도 있었습니다....

지금까지는 특정 폴더를 웹으로 서빙해야될때 `python -m SimpleHTTPServer`를 사용했다.
내 생각에 이것의 장점은 2가지이다. 파이썬이 없는 시스템은 많지 않다. 내 경우는 윈도우에서도 파이썬을 깔고 리눅스/맥에는 보통 파이썬이 내장되어있다.
두번째로 추가 패키지가 필요없다는 점이다. `python`이 실행 가능하면 `python -m SimpleHTTPServer` 는 돌아간다.

하지만 개발하다보니 성능상의 문제가 발생했다.
페이지 로딩 속도가 느리고 가끔 F5를 눌러도 페이지를 제대로 못불러오더라. 원격 서버도 아니고 로컬 서버인데!!!

그래서 찾아보니 [같은 문제로 고통받는 사람들][stack_overflow_http_server]이 많더라.
StackOverflow 답변 중 제일 괜찮아 보이는게 node.js의 http-server라서 이번 기회에 갈아탔다.
`npm install -g http-server` 로 갈아탄 이후에는 특별한 문제가 발생하진 않았다. 앞으로는 http-server를 주로 쓸거같다.

## 다항식 렌더링

### HTML5 Canvas Polynomial Rendering Library

Lagrange polynomial 를 구현한 이후 나머지 부분을 직접 코딩하기 매우 귀찮았다.
다항식 렌더링도 매우 귀찮아서 라이브러리로 때우려고 했다.
다항식을 HTML5 Canvas로 렌더링해주는 라이브러리가 있으면 매우 간단하게 때울수 있겠다 싶었는데 마땅한 라이브러리를 찾지 못했다.
차라리 DOM으로 렌더링해도 되면 LaTeX 라이브러리를 쓰면 될텐데.

### LaTeX -> DOM -> SVG -> HTML5 Canvas

[@u64_t님][twitter_u64_t_dom_to_svg]이 [DOM을 SVG로 그리는 떡밥][mozilla_dom_to_svg]을 던졌다.

LaTeX 렌더링 라이브러리로는 [KaTeX][external_katex]를 선택하고
[Extract the current DOM and print it as a string, with styles intact][stack_overflow_extract_style]를 참고해서
LaTeX로 작성된 다항식을 KaTeX를 이용해서 DOM으로 뽑아내로 이를 SVG로 그린후 HTML5 Canvas로 렌더링하는 것을 구현했다.

구현하고 나니까 심각한 문제가 여러가지 발견되었다.
포스트모템을 작성할때 참고하려고 간단하게 [KaTeX를 이용해서 수식 렌더링 샘플 페이지][doge_math_latex]를 구현했다.
자신이 자주 사용하는 브라우저로 들어가서 어떤식으로 렌더링되나 한번 확인해보자. 그리고 update 버튼을 여러번 눌러보자.

대표적인 브라우저를 이용하면 해당 페이지가 어떤식으로 보여지는지 그리고 어떤게 문제인지를 요약했다.

#### Internet explorer
![KaTeX with Canvas - IE](latex-ie.png)

역시 IE는 우리의 기대를 배신하지 않는다.
아무것도 안그려진다.
IE는 DOM을 SVG로 직접 그리는걸 지원하지 않는 것으로 보인다.
[Mozilla 샘플][mozilla_dom_to_svg]에서도 아무것도 안보인다.
익스플로러는 마이크로소프트조차 버린 물건이니까 기대하진 않았다.
최신 브라우저로 보면 좀 낫겠지?

#### Microsoft Edge
![KaTeX with Canvas - Edge](latex-edge-20.png)

테스트한 Microsoft Edge의 버전은 20.10240.16384.0 이다.
HTML5 Canvas에 그린 결과물을 보면 수식이 아니라 문자열이 보인다.
KaTeX를 이용해서 그린 DOM에서 스타일을 빼내서 렌더링하는 부분이 잘못되어 있는거 아닐까?

#### Google Chrome
![KaTeX with Canvas - Chrome 45](latex-chrome-45.png)

테스트한 Google Chrome의 버전은 45.0.2454.93m 이다.
Microsoft Edge와 마찬가지로 HTML5 Canvas에 수식이 아니라 문자열이 보인다.
Edge와 Chrome 모두에서 제대로 렌더링 되지 않으니까 역시 DOM에서 스타일을 빼오는 코드가 잘못된 거 아닐까?

#### Mozilla Firefox
![KaTeX with Canvas - Firefox 40](latex-firefox-40.png)

테스트한 Mozilla Firefox의 버전은 40.0.3 이다.
Chrome, Edge와는 달리 HTML5 Canvas에 공식처럼 보이는 무언가가 보인다.
하지만 루트 기호의 위쪽 가로선이 보이지 않는다.
파폭에서는 돌아가는데 엣지와 크롬에서는 수식이 보이지 않는거로 볼때 접근법 자체는 맞지만 DOM에서 스타일을 빼오는 코드에 버그가 있는거같다.
버그가 있을지도 모르는 코드를 고치기 전에 이 접근법을 사용하는 것이 가능한지 추가로 테스트해보았다.

Doge Math의 최종 완성품은 숫자 입력하는 필드가 있고 이를 조작하면 짤이 실시간으로 갱신되어야한다.
그렇지 않으면 Fake Data를 조정해서 최종 결과를 정수로 만들기 어렵다.
이를 테스트하고자 새로운 LaTeX 수식을 생성해서 다시 렌더링하는 update 버튼을 만들었다.
파폭에서 update 버튼을 누르면 수식 갱신 속도가 느린걸 확인할수 있다.
KaTeX가 다른 Javascript LaTex 라이브러리보다 속도가 빠르다고 자랑하지만 그래봤자 결국은 LaTeX다.
Doge Math에 써먹을수 있을정도로 빠른 속도가 나오진 않더라.

그래서 LaTeX 라이브러리로 DOM을 만들고 이걸 이용해서 SVG를 그리고 다시 HTML5 Canvas에 그리는 방법은 고르지 않았다.

### 직접 구현

나한테 딱 맞는 라이브러리는 못찾았고 야매로 구현하는 것도 실패했으니 어쩔수 없이 직접 구현했다.
귀찮아서 직접 구현하려고 하진 않았으니 막상 직접 구현하니 특별히 어려운점은 없었다.
다항식 렌더링을 직접 구현하면서 신경쓴건 가운데 정렬뿐이다.

처음에는 다항식을 왼쪽정렬로 그렸다.
그런데 생성의 값을 조정하면 짧은 다항식부터 긴 다항식까지 어떤 다항식이 나올지 모르니까 짤에 다항식이 예쁘게 들어가지 않더라.
그래서 가운데 정렬을 구현하기로 했다.

가운데 정렬을 구현하려면 다항식의 폭을 알아야한다. 그래서 2-pass render 같은 느낌으로 다항식을 2번 그렸다.

1. 1-pass : 다항식을 실제로 Canvas에 그리진 않고 폭만 계산
2. 1-pass에서 얻은 다항식의 폭을 이용해서 다항식을 그릴 좌표를 계산
3. 2-pass : 다항식을 제대로 그린다.

다항식 렌더링에 더 공들이면
길어졌을때 2줄로 나누기, 예쁘게 보이는 수준으로 폰트 자동 계산하기 같은 기능을 구현할수도 있겠지만
그정도까지 잘 만들 필요는 없겠다 싶어서 하지 않았다.


## 작업 기록
* 2015/09/07 : 프로젝트 시작. Lagrange polynomial interpolation 구현 완료
* 2015/09/08 : 동아리 개강총회(라고 쓰고 노인 무료 급식소)에서 밥먹고 술먹고 놀았음
* 2015/09/09 : 맥스페인3 재밌다
* 2015/09/10 : 맥스페인3 클리어. 그것은 좋은 게임이었다.
* 2015/09/11 : 놀았다.
* 2015/09/12 : 수학공식을 제외한 나머지 개발. 프로젝트 릴리즈
* 2015/09/15~17 : 포스트모템 작성 시작

프로젝트 기간은 6일, 실제 작업일은 2일이다.
하지만 9/7은 월요일이라서 몇시간 하지 않았다. 대부분의 작업은 9/12(토요일)에 이루어졌다.
가장 작업을 시간을 많이 쓴 것은 다항식 렌더링이다.
HTML5 canvas에 직접 다항식을 그리기 귀찮아서 라이브러리 찾아보고 KaTeX로 삽질하거나 하는데 시간을 대부분 썼다.
하지만 라이브러리 사용하는걸 포기하고 직접 짜니까 얼마 안걸리더라.

## Doge Math를 만들면서 배운것/느낀것
* `npm install -g http-server` > `python -m SimpleHTTPServer`
* 재사용할수 있는 테마의 중요성
* 웹에서 LaTeX 써야할때는 KaTeX를 쓰자
* 모든 라이브러리가 세상에 존재하는거같지만 꼭 그런건 아니다. 직접 짜는걸 피할 순 없다.
* 웹 드래프트 문서가 있다고 그것이 모든 브라우저에 구현되어있을거라는 기대는 하지마라.
* 내가 웹개발 안하는 사이에도 웹은 발전한다. 웹에서 클립보드가 된다니!
* 나중에 재밌는 짤방 떡밥 생기면 브라우저 기반 범용 짤방 생성기를 만들어봐야지
* 고작 Doge Math 정도 수준의 프로젝트의 개발 기록을 정리하는 것도 큰 일이다. 앞으로는 개발 기록 꼬박꼬박 정리해야지
* [재귀로 Combination 구현한 코드 예쁘더라][external_combination]
* 맥스페인3 재밌다

[doge_math_repo]: https://github.com/if1live/doge-math
[doge_math_site]: https://if1live.github.io/doge-math/
[doge_math_latex]: https://if1live.github.io/doge-math/test_latex.html

[original_meme]: http://www.funnywebsite.com/funny-website/index.php/2015/05/04/doge-math/
[twitter_portal]: https://twitter.com/_Yous/status/640341772372054016
[twitter_u64_t_dom_to_svg]: https://twitter.com/u64_t/status/642636254497112064
[mozilla_dom_to_svg]: https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Drawing_DOM_objects_into_a_canvas
[external_katex]: http://khan.github.io/KaTeX/
[external_zeroclipboard]: https://github.com/zeroclipboard/zeroclipboard
[external_combination]: https://gist.github.com/axelpale/3118596
[stack_overflow_http_server]: http://stackoverflow.com/questions/12905426/what-is-a-faster-alternative-to-pythons-simplehttpserver
[stack_overflow_extract_style]: http://stackoverflow.com/questions/6209161/extract-the-current-dom-and-print-it-as-a-string-with-styles-intact
[stack_overflow_parse_query_string]: http://stackoverflow.com/questions/901115/how-can-i-get-query-string-values-in-javascript
[stack_overflow_download_canvas]: http://stackoverflow.com/questions/12796513/html5-canvas-to-png-file
[stack_overflow_clipboard]: http://stackoverflow.com/questions/10729570/html5-alternative-to-flash-based-zeroclipboard-for-safe-copying-of-data-to-clipb
[math_lagrange]: https://mat.iitm.ac.in/home/sryedida/public_html/caimna/interpolation/lagrange.html
[free_font]: http://www.fontsquirrel.com/
[wiki_comic_sans]: https://en.wikipedia.org/wiki/Comic_Sans

[wd_clipboard_apis]: http://www.w3.org/TR/clipboard-apis/
[mozilla_clipboard_error]: https://support.mozilla.org/ko/questions/1074098
