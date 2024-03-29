---
title: 한국형 반응형웹 프레임웍을 찾아서
tags: [ie7, ie8, ie, responsive web]
slug: ie7-responsive-web-framework
author: if1live
date: 2014-02-05
subtitle: 왜 우리는 IE 7,8때문에 고통받아야 하는가?
url: /posts/ie7-responsive-web-framework
---
[Trade My Info의 GA 통계]({{< ref "trade-my-info-ga" >}})에서 볼 수 있듯이 2014년 2월 기준 우리나라에서 IE 7,8의 사용비율은 데탑유저 중 15%이다. 이를 버리고 차세대 웹으로 가고싶은 마음은 있지만 어쩔수 없이 IE 7,8을 지원해야 하는게 현실이다.

반응형웹 디자인을 도입하면 데스크탑/모바일 웹디자인을 따로 만들 필요가 없다는 것을 포함해서 여러가지 장점이 있다. 그런데 "반응형웹 디자인은 좋구나!" 하고 도입하려고 하는 순간 한가지 문제가 발생한다. 구형IE에서 깨질 가능성이 높다는 것이다.

그래서 반응형웹 디자인을 IE7,8에서도 별 문제없이 도입하기 위해서 사용할만한 웹 프레임웍을 정리해봤다.

테스트한 반응형웹 프레임웍은 내가 평소에 알고있던 것과 [10 Best Responsive HTML5 Frameworks and Tools][10-responsive-framework]에 등장한 프레임웍이다.

* [996 Grid][996grid]
* [Bootstrap 2][bootstrap2]
* [Bootstrap 3][bootstrap3]
* [Pure][pure]
* [Foundation][foundation]
* [Skeleton][skeleton]
* [HTML5 Boilerplate][html5-boilerplate]
* [HTML Kickstart][html-kickstart]
* [MontageJS][montagejs]
* [SproutCore][sproutcore]
* [Zebra][zebra]
* [CreateJS][createjs]
* [Less Framework 4][less-framework]

이 글에서는 위의 프레임웍 각각에 대해서 설명하는것은 다루지 않는다.
IE 7,8을 지원하는 프레임웍에 한해서만 조금 더 자세하게 다룬다.

## IE 7,8을 공식적으로 지원하는가?

프레임웍이 IE 7,8을 지원하는가 여부는 다음을 기준으로 판단했다.

IE의 호환성모드를 IE 7,8이라고 가정했다. 실제 IE 7,8과 호환성 모드가 미묘하게 다른건 나도 안다.
하지만 IE 호환성 모드에서도 깨진 페이지가 진짜 IE 7,8에서 안깨질리가 없으니까 별 문제가 없다고 판단했다.

[프레임웍 소개 페이지에 IE 7,8을 지원한다고 명시했으면 IE 7,8을 지원한다고 취급했다.](http://www.getskeleton.com/#support)

IE 7,8에서 프레임웍 대문페이지가 특별한 문제 없이 뜬 것은 IE 7,8을 지원한다고 판단했다.
대문페이지는 프레임웍 개발자가 만든건데 그 사람들이 만든 페이지가 IE 7,8에서 깨지면 IE 7,8로는 테스트를 안했다는 소리일테니까 지원하지 않는 소리겠지.
[메뉴가 박살나거나](http://foundation.zurb.com/) [다른 브라우저와 렌더링 결과가 너무 다르면](http://zebra.gravitysoft.org/) 구형 IE를 지원하지 않는다고 분류했다.


Name               | IE7 | IE8
------------------ | --- | ---
996 Grid           | o   | o
Bootstrap 2        | o   | o
Bootstrap 3        |     | o
Pure               | o   | o
Foundation         |     |
Skeleton           | o   | o
HTML5 Boilerplate  |     | o
HTML Kickstart     |     | o
MontageJS          |     |
SproutCore         |     |
Zebra              |     |
CreateJS           |     |
Less Framework 4   | o   | o

IE 8은 지원하지만 IE 7은 지원하지 못하는 프레임웍은 몇개 있지만 내 관심밖이니까 제외한다.
[996 Grid][996grid], [Bootstrap 2][bootstrap2], [Pure][pure], [Skeleton][skeleton], [Less Framework][less-framework] 가 IE 7,8 에서 별 문제없다.

## 추가기능

[Bootstrap][bootstrap3]는 나같이 디자인 하나도 모르는 사람이 css속성만 추가해주면 버튼도 예쁘게 나오고 테이블도 예쁘게 나오고 메뉴도 예쁘게 만들어줬다. 반면에 [960 Grid System][960gs]라는 라이브러리의 경우는 레이아웃만 잡아준다.
반응형웹 프레임웩이니까 전부다 레이아웃 관련된 기능은 있을테고 레이아웃을 제외하고 어떤 추가기능이 있는지 정리했다.

Name               | 추가기능
------------------ | -------------------
996 Grid           | x
Bootstrap 2        | button, table, menu, nav, js library...
Pure               | form, button, table, menu
Skeleton           | button, form
Less Framework 4   | x

Skeleton의 경우 기능 목록만 보면 Typography가 있지만 해당 기능은 어차피 [Normalize.css][normalize], reset.css와 중복되니 무시했다.
추가기능 목록을 보면 Bootstrap 2가 제일 많다. 996 Grid와 Less Framework 4는 레이아웃 잡는게 전부다.


## 유지보수가 되는가?

라이브러리에 기능이 많다는 소리는 버그도 많이 있을지도 모른다는 소리다. 동시에 지속적으로 유지보수 되어야한다는 소리다.
그런 수정을 내가 직접 하는건 귀찮다. 프레임웍이 누군가에 의해서 유지보수 되고있으면 귀찮은게 하나 줄어들것이다.

2014/02/05 기준으로 마지막 커밋시간을 이용해서 유지보수 여부를 정리했다.

Name               | 마지막 commit 시간
------------------ | -------------------
996 Grid           | [4개월전](https://github.com/ComputerWolf/996-Grid)
Bootstrap 2        | Bootstrap 3으로 옮겨감
Pure               | [21일전](https://github.com/yui/pure/)
Skeleton           | [2년전](https://github.com/dhg/Skeleton)
Less Framework 4   | [2년전](https://github.com/jonikorpi/Less-Framework)

Bootstrap 2의 경우는 Bootstrap 3로 넘어갔다. 더이상의 유지보수는 없다.
새로운 기능이 필요하거나 기능을 사용하다가 문제가 생기면 그때는 알아서 잘 처리해야된다.
다른 라이브러리와 달리 bootstrap의 경우는 기능이 많아서 유지보수가 중요하니까 탈락시켰다.

Pure가 가장 최근까지 유지보수 되고있다. 저장소 주소에서 알 수 있듯이 YUI Library가 유지보수를 하고있다.
새로운 기능 추가라든가 버그 수정관련으로는 제일 좋은 선택지이다.

996 Grid와 Less Framework 4의 경우는 마지막 커밋이 꽤 오래되었다.
어차피 레이아웃 잡는게 전부인 라이브러리니까 별로 고칠게 없나보다.
반대로 말하면 앞으로도 레이아웃 이외의 기능이 추가될리가 없다.

Skeleton의 경우는 마지막 커밋이 오래되었지만 button, form 정도의 기능은 어려운게 아니니까 유지보수가 끊겨도 별 문제가 없을듯 하다.

## 반응형웹 구현방식

반응형웹 디자인을 하는 방법은 Mobile-First와 Desktop-First가 있다.
Mobile-First는 모바일 디자인을 먼저 만든 다음에 mediaquery를 통해서 데스크탑 디자인을 만드는것이다.
Desktop-First는 데스크탑 디자인을 먼저 만들고 mediaquery를 이용해서 모바일 디자인을 만드는것이다.

책이라든가 인터넷에 굴러다니는 자료를 찾아보면 보통 Mobile-first를 추천한다.
내 경우도 처음에는 이런말을 듣고 Mobile-first로 만들었다가 치명적인 문제로 인해 Desktop-first로 디자인을 갈아엎은 적이 있다.

**모바일 퍼스트로 만들면 구형 IE에서 속도가 느리기 때문이다.**
구형 IE는 미디어쿼리를 지원하지 않는다.
다행히도 [respond.js](https://github.com/scottjehl/Respond)를 붙이면 구형IE에서도 미디어 쿼리가 작동한다.
참 좋죠? 라고 생각하고 적용했는데 IE 7,8에서 페이지 로딩속도가 많이 느렸다.
respond.js 소개에는 "A fast & lightweight ..."라고 되어있지만 아무리 빨라봣자 라이브러리 안 다는것보다는 느리다.
게다가 DOM이 복잡해지면 덩달아 느려진다.

자세한 사항은 [모바일 퍼스트와 데스크탑 퍼스트의 장단점에 대해 비교한 글](http://tobyyun.tumblr.com/post/58232536556/rwd-3)을 참고하자.

각각의 프레임웍의 구현방식을 정리했다.

Name               | ?-first
------------------ | -------------------
996 Grid           | [Desktop-first](https://github.com/ComputerWolf/996-Grid/blob/master/code/css/grid.css)
Pure               | [Desktop-first](https://github.com/yui/pure/blob/master/src/grids/css/grids-r.css)
Skeleton           | [Desktop-first](https://github.com/dhg/Skeleton/blob/master/stylesheets/skeleton.css)
Less Framework 4   | [Desktop-first](https://github.com/jonikorpi/Less-Framework/blob/master/main-16px.css)

전부다 Desktop-first기반이다.
[IE8을 지원하기 위해서 respond.js를 추가하라는 bootstrap](http://getbootstrap.com/getting-started/#template)과는 달리 respond.js없이 잘 굴러간다.

## 폭 크기 지정 방법

예를 들어 현재 화면의 크기가 1000px이라고 가정하자. 이 화면에 가로로 2개의 div를 추가하고 싶다.
그러면 500px + 500px로 배치할수도 있지만 50% + 50% 로 배치할수 있을것이다.
즉, 반응형웹에서 레이아웃을 잡을때 pixel을 기준으로 잡을수도 있지만 percent를 기준으로 잡을수도 있다는거다.

각각의 프레임웍에서 폭을 잡는 방법을 살펴보면...

996 Grid 은 픽셀을 이용한다.

```css
/* Base Grid */
.container .grid_1 {width: 57px;}
.container .grid_2 {width: 140px;}
.container .grid_3 {width: 223px;}
....
```

Skeleton은 픽셀을 이용한다. (문제는 본격 영어 못하면 못쓰는 프레임웍. one, two, three, four...식으로 사용한다.)

```css
/* Base Grid */
.container .one.column,
.container .one.columns    { width: 40px;  }
.container .two.columns    { width: 100px; }
.container .three.columns  { width: 160px; }
....
```

![잘못된 영어 숫자의 예](wrong-english-number.jpg)

Less Framework 4의 경우는 요상한 흑마법을 쓰는거같은데 어쨋든 픽셀 기반이다.

```css
/*		Default Layout: 992px.
		Gutters: 24px.
		Outer margins: 48px.
		Leftover space for scrollbars @1024px: 32px.
-------------------------------------------------------------------------------
cols    1     2      3      4      5      6      7      8      9      10
px      68    160    252    344    436    528    620    712    804    896    */

body {
	width: 896px;
	padding: 72px 48px 84px;
	background: rgb(232,232,232);
	color: rgb(60,60,60);
	-webkit-text-size-adjust: 100%; /* Stops Mobile Safari from auto-adjusting font-sizes */
}
```

하지만 Pure는 좀 다르다. 얘는 %를 이용한다.

```css
.pure-u-2-3 { width:66.6667%; *width:66.6357% }
.pure-u-1-4 { width:25%; *width:24.969% }
.pure-u-3-4 { width:75%; *width:74.969% }
.pure-u-1-5 { width:20%; *width:19.969% }
```

* 픽셀 기반 : 996 Grid, Skeleton, Less Framework 4
* 퍼센트 기반 : Pure

반응형웹이라는 주제에는 %를 사용하는 Pure가 더 그럴싸해보인다.
하지만 픽셀단위를 쓰면 기존의 웹디자인 방법과 가까우니까 작업하기 편하다. (예를 들면 고정폭으로 디자인 하고 200px짜리 배너를 넣을때)

## 정리

조사한 범위에서 IE 7,8 까지 제대로 지원하는 반응형웹 프레임웍은 다음과 같다.

* [996 Grid][996grid]
* [Bootstrap 2][bootstrap2]
* [Pure][pure]
* [Skeleton][skeleton]
* [Less Framework 4][less-framework]

[Bootstrap 2][bootstrap2]의 경우는 유지보수가 끊겨서 추천하지 않는다.

[996 Grid][996grid], [Less Framework 4][less-framework]은 레이아웃만 잡아준다.
추가기능은 없으니 다른 CSS 라이브러리를 집어넣었을때 충돌할 가능성이 적다는 장점이 있다.
반응형웹 디자인으로 레이아웃 잡는 기능만으로 충분하면 추천한다.

[Skeleton][skeleton]은 약간의 추가기능이 있지만 없어도 티 안나는 수준이다.
다만 코딩 스타일(숫자를 영어로 써야한다)이 영어 못하면 짜증난다.
```.container .eleven.columns```에서 ```.container .eight.columns```으로 고치는게 쉬울까?
```.grid_11```에서 ```.grid_8```로 바꾸는게 쉬울까?

[Pure][pure]는 지속적으로 유지보수 되고있고 추가기능도 많다.
다만, 반응형웹을 픽셀이 아니라 %로 구현해서 배너와 같은 이미지를 깨끗하게 만들이 어려울 수 있다.

그리고 내 경우는 실제 프로젝트에서 [Pure][pure]를 써봤는데 폭크기 조절이 %라는 단점때문에 다음에 뭘 만들면 [996 Grid][996grid]를 써볼 생각이다.


[10-responsive-framework]: http://designinstruct.com/roundups/html5-frameworks/
[996grid]: http://www.996grid.com/
[bootstrap2]: http://getbootstrap.com/2.3.2/
[bootstrap3]: http://getbootstrap.com/
[pure]: http://purecss.io/
[foundation]: http://foundation.zurb.com/
[skeleton]: http://www.getskeleton.com/
[html5-boilerplate]: http://html5boilerplate.com/
[html-kickstart]: http://www.99lime.com/elements/
[montagejs]: http://montagejs.org/
[sproutcore]: http://sproutcore.com/
[zebra]: http://zebra.gravitysoft.org/
[createjs]: http://createjs.com/#!/CreateJS
[less-framework]: http://lessframework.com/
[960gs]: http://960.gs/
[normalize]: https://github.com/necolas/normalize.css/
