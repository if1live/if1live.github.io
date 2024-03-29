---
title: gksrmfdldkscuwudy 포스트모템
subtitle: 한글이 안쳐져요 (알파벳-두벌식 변환기)
tags: [postmortem]
slug: gksrmfdldkscuwudy-post-mortem
author: if1live
date: 2016-05-29
url: /posts/gksrmfdldkscuwudy-post-mortem
---
## 개요

* 개요: 알파벳-두벌식 변환기
* 개발기간: 2016/05/07 00:00 ~ 2016/05/07 02:00 (2시간)
* 사용언어: JavaScript
* 저장소: https://github.com/if1live/gksrmfdldkscuwudy
* 써보기: https://if1live.github.io/gksrmfdldkscuwudy/

![왜 만들었을까?](why-i-dev-it.png)

집에서 뒹굴고 있다가 떡밥이 나와서 자기전에 만들었다. (...)

## 접근법1. 한글 오토마타

처음에는 오토마타를 이용해서 구현하려했다.
간단히 말해서 한글 입력기 비슷한걸 생각했다.

1. 한글 오토마라를 적당히 준비한다.
2. `r`를 입력하면 `ㄱ`를 오토마타로 전달한다.
3. 한글 오토마타가 적당히 자모음을 조합해서 `ㄱ`를 출력한다.
4. `k`를 입력하면 `ㅏ`를 오토마타로 전달한다.
5. 한글 오토마타가 적당히 자모음을 조합해서 `가`를 출력한다.
6. 적절히 위의 과정을 반복한다.

직접 구현하긴 귀찮아서 GitHub에서 hangul automata, javascript로 검색하니까
[hangul-automata][hangul-automata] 나오더라.
이걸 이용해서 구현하려고했으나 몇가지 문제가 있어서 방법을 바꿨다.

첫번째 문제는 왠지 덜만들어진 라이브러리처럼 보였다는 점이다.
README에 아무 내용도 없어서 사용하기 껄끄럽더라.
그렇다고 동작하지 않는 코드였다거나 그런건 아니다. 돌아가긴 한다.
약간의 수정없이는 안돌아가지만.

두번째 문제는 라이브러리 코드 수정 없이는 못쓴다는거다.
고작 알파벳-두벌식 변환기를 구현하기 위해서 빌드 프로세스를 붙이긴 귀찮더라.
내가 원하는건 html파일 안에 js파일을 때려박고 즉시 사용하는거다.
이렇게 하기 위해서는 소스를 고쳐야했는데 귀찮았다.

세번째 문제이자 가장 큰 문제는 오토마타를 쓰면 자유로운 편집이 어려울거같다는 생각이 들어서이다.
오토마타에 자모음 한글자씩 넘기는건 간단히 만들수 있고 백스페이스까진 어떻게 구현할수 있을거다.
그런데 문장을 복붙해서 중간에 끼워넣는다면? 오토마타로는 한계가 있을거같더라.

그래서 더 무식한 방법으로 방향을 바꿨다.

## 접근법2. 알파벳 -> 한글 자모음 -> 조합

`ㅅ`, `ㅗ`, `ㄹ`, `ㅏ` 가 있을때 이를 `소라`로 합쳐주는 함수가 있으면 간단하게 만들수 있지 않을까?
내가 해야되는 일은 `t`를 `ㅅ`로, `h`를 `ㅗ`로 바꿔주는것 뿐이다.
각각의 알파벳을 두벌식에 대응되는 자모음으로 바꾼다음에 그냥 합쳐주면된다.

[Hangul.js][hangul_js]를 이용하면 이게 가능하다.
Hangul.js의 [입력 문장 (자음 및 모음을 조합)](http://e-.github.io/Hangul.js/examples/)를 보고 갖다쓰기로했다.


## 클립보드로 복사

옛날 옛적에 2013년에도 클립보드 복사 기능이 필요한 적이 있었다.
그때는 [ZeroClipboard][zeroclipboard]를 이용했었다.
이유는 간단하다. IE7을 지원해야되는 사이트였기 때문이다.
HTML5에는 [Clipboard API][clipboard-api]가 있지만 IE7은 그런걸 지원하지 않는다.
IE7에서 클립보드로 복사하는걸 구현하려면 플래시가 필요하다.

그리고 지금은 2016년, 이번에는 IE7따위를 지원할 필요가 없어서 라이브러리를 바꿨다.
플래시 의존성이 없는 [clipboard.js][clipboard_js]를 이용했다.
그래서 모바일에서도 클립보드로 복사 기능이 돌아간다.

## array.map()

내가 마지막으로 웹개발로 먹고살때는 IE7, IE8까지 지원해야했다.
그래서 IE7, IE8에서도 돌아가는 기능에 대해서만 기억했다.
새로운 기능이 있어도 IE7, IE8에서 안돌아가거나 기능을 쓰기위해 polyfill를 붙였다가 성능이 안나오면 안썼다.

IE7, IE8 시절 자바스크립트에서는 `array.map()`이라는 함수가 없었다.
그래서 일부 개발자들은 `array.map()`를 따로 구현해서 사용했다.

http://stackoverflow.com/a/1534416

```js
if (!Array.prototype.map) {
  Array.prototype.map = function(fun /*, thisp*/)   {
    var len = this.length >>> 0;  // make sure length is a positive number
    if (typeof fun != "function") // make sure the first argument is a function
      throw new TypeError();
    var res = new Array(len);  // initialize the resulting array
    var thisp = arguments[1];  // an optional 'context' argument
    for (var i = 0; i < len; i++) {
      if (i in this)
        res[i] = fun.call(thisp, this[i], i, this);  // fill the resulting array
    }
  return res;
};
```

내 경우는 IE7, IE8에서 `array.map()`는 안되길래 없는 함수로 취급하고 살았는데 이번에 해보니까 되더라?
내가 잘못 기억하는거 아닐까 싶어서 찾아봤는데 Internet Explorer 9부터 지원하더라.

https://developer.mozilla.org/ko/docs/Web/JavaScript/Reference/Global_Objects/Array/map


## 이건 쓸만한 물건인가?

한글이 안쳐질때 구글에서 검색하면 웬만한건 자동완성이 된다.

![Google](google-convert.png)

그리고 윈도우에서 한글 입력에 제대로 작동하지 않을때 cftmon.exe를 직접 실행하면 된다고카더라.

이 정도면 이걸 직접 꺼낼일이 없을거 같지만 가끔씩 필요할때가 있다.
예를 들면 리눅스 GUI를 사용하는데 한글 입력기가 뻑나서 영어밖에 못칠때,
누군가 한글 입력이 안되서 영어로 써놓은걸 한글로 바꿔보고 싶을때,
그리고 [지금 당장 한글 입력이 필요한 사람들][twitter_search]

## 좋은점

* IE7, IE8 죽은 세상에서 개발하니까 못쓰던 라이브러리를 쓸수 있어서 좋았다.
* IE7, IE8 에서 개발하던 기억때문에 좋은게 나와도 안쓰고 있었다는걸 깨달았다.
* 개발한거로 2.8K RT딸 처본건 처음이었다. https://twitter.com/if1live/status/728824709777760256
* 2016/05/05~2016/05/08 연휴동안 뒹굴면서 뭐라도 하나 하긴 했다.

[zeroclipboard]: https://github.com/zeroclipboard/zeroclipboard
[clipboard_js]: https://github.com/zenorocha/clipboard.js/
[hangul-automata]: https://github.com/saebyeok/hangul-automata
[hangul_js]: https://github.com/e-/Hangul.js
[clipboard-api]: https://www.w3.org/TR/clipboard-apis/
[twitter_search]: https://twitter.com/search?f=tweets&vertical=default&q=gksrmfdldkscuwudy&src=typd
