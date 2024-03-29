---
title: 크롬OS에서 같은 앱을 2개 이상 여는 방법
subtitle: window.open()
tags: [chrome os]
slug: how-to-open-multiple-webapp-window-in-chrome-os
author: if1live
date: 2018-02-23
url: /posts/how-to-open-multiple-webapp-window-in-chrome-os
---

## 크롬OS에서 웹앱 창으로 열기

크롬OS에서는 웹사이트를 창으로 여는게 가능하다.
창으로 웹사이트를 열면 주소바가 사라져서 웹사이트가 아닌 크롬앱처럼 보인다.
방법은 간단하다.
창으로 열고싶은 웹사이트에 들어가서 `메뉴` -> `도구 더보기` -> `실행기에 추가...`를 선택한다.

!['실행기에 추가' 메뉴][img-add-to-launcher-menu]

다이얼로그가 열리면 `창으로 열기`가 선택된 상태에서 추가한다.
(창으로 열지 탭으로 열지는 나중에 실행기에서 우클릭을 통해 바꿀 수 있다)

!['실행기에 추가' 팝업][img-add-to-launcher-dialog]

실행기를 열어보면 아까 추가한 웹사이트가 아이콘으로 등록되어 있다.

![실행기][img-launcher-menu]

실행기에서 아이콘을 누르면 웹사이트가 창으로 열린다.
주소창이 없는만큼 넓게 쓸 수 있다.

![창으로 열기][img-webapp-window]

이것을 알고 있으면 유용하게 쓸 수 있다.
나는 멍청해서 이 기능을 몰랐다.
그래서 chrome app의 webview를 이용해서 사이트를 래핑하는 크롬앱을 짰었다.
(나중에 `창으로 열기` 기능을 배우고 짠걸 버렸다)

https://github.com/GoogleChrome/chrome-app-samples/tree/master/samples/webview-samples

## 단점: 1개의 창

안드로이드 또는 아이폰에서 어떤 앱을 깔았다고 치자.
그 앱을 실행하면 하나의 프로세스만 존재한다.
홈버튼을 눌러서 밖으로 나와서 앱을 다시 누르면 앱이 새로 실행되지 않고 기존에 실행되던 앱이 다시 뜬다.

크롬OS도 기본적으로는 이렇게 굴러간다.
하나의 앱은 동시에 한개만 열 수 있다.
실행기에 앱을 누르면 새로운 앱이 실행되지 않고 기존의 실행중이던 앱이 다시 열린다.
(몇가지 예외는 있다. 탐색기, 파일관리자같은 앱은 동시에 2개 이상을 창으로 띄울수 있다)
이 제약은 크롬앱에만 있는게 아니다. `실행기에 추가`를 통해 등록한 것도 제약에 걸린다.

일반적인 상황에서는 같은 앱을 2개 이상 띄울 일이 없을지도 모른다.
하지만 나는 일반적인 상황에서만 작업하는게 아니다.
윈도우에서 터미널 작업을 할때는 putty창을 2-3개씩 띄웠었다.
크롬OS에서도 그런식으로 작업을 할거같더라.
적어도 2개의 창을 띄우고 싶었다.
탭으로 열면 갯수 제한이 없지만 주소창 공간이 아까워서 가급적 창으로 열고 싶었다.

## 예외: Crosh Window

아이콘을 눌러서 앱을 실행했을때 실행되던 창이 다시 뜨지 않고 새로운 창이 뜨는 예외가 몇가지 있다고 했다.
크롬이나 파일관리자는 크롬OS 내장 앱이니까 동작이 특별해도 신기하지 않다.
하지만 크롬OS 내장 앱이 아니면서 창을 여러개 띄울수 있는 앱이 존재한다.
[Crosh Window][store-croshwindow]이다.
대체 Crosh Window는 어떻게 구현되었길래 같은 창을 여러개 띄울수 있을까?

## 잘 모를때는 남꺼를 뜯으면 된다

웹의 장점이 무엇일까? 역시 코드가 투명하다는거 아닐까?
잘 모를때는 뜯어보면 된다.
자바스크립트 코드를 보면 답이 보인다.

[crx4chrome][download-croshwindow]에서 Crosh Window를 받아서 뜯어봤다.
crx 확장자는 zip 파일이다.

생각보다 답이 단순하더라. 아래는 뜯어서 얻은 자바스크립트 소스이다.

```js
chrome.app.runtime.onLaunched.addListener(() => {
  // crosh built-in: chrome-extension://nkoccljplnhpfnfiajclkommnmllphnl/
  // Secure Shell: chrome-extension://pnhechapfaindjhompbnflcldabbghjo/
  // Ssh dev: chrome-extension://okddffdblfhhnmhodogpojmfkjmhinfp/
  window.open(
      'chrome-extension://pnhechapfaindjhompbnflcldabbghjo/html/crosh.html', '',
      'chrome=no,close=yes,resize=yes,scrollbars=yes,minimizable=yes,' +
      'width=735,height=440,background');
  // Note: "background" here (and in the manifest) are to workaround
  // https://crbug.com/708870.  Once that hits stable, we can drop it.
});
```

`window.open()`를 쓰면 되더라.
crosh window 앱이 하는 일은 간단하다. 실행되면 다른 확장을 연다.
이것의 복선은 crosh window 앱 설명에 있긴 했다.
crosh window를 쓰기 위해서는 secure shell 앱을 받으라고 하더라.

```
*** For Chrome OS only *** You must have Secure Shell installed for this to work:…
*** For Chrome OS only ***

You must have Secure Shell installed for this to work: https://chrome.google.com/webstore/detail/pnhechapfaindjhompbnflcldabbghjo

When clicked, this app opens a new crosh window without any chrome. This way, Ctrl-N/T/etc go to crosh and aren't picked up by Chrome.

Note: If you want, you can always open a crosh window by using Secure Shell directly.  This app is merely a convenience for people who like it.  See the Secure Shell FAQ for more details:
https://goo.gl/muppJj
```

## summary

`window.open()`을 이용하면 크롬OS에서 같은 앱을 두번 열 수 있다.
앱, 또는 웹을 여는 새로운 앱을 만들면 된다.
 
그래서 나도 한번 만들었다. 웹쉘을 여러개 열기 위해서 만든 [크롬앱][repo-webshell-window]이다.

![webshell window screenshot](https://raw.githubusercontent.com/if1live/webshell-window/master/document/multi-instance.png)

[repo-croshwindow]: https://github.com/adlr/croshwindow
[store-croshwindow]: https://chrome.google.com/webstore/detail/crosh-window/nhbmpbdladcchdhkemlojfjdknjadhmh
[download-croshwindow]: https://www.crx4chrome.com/crx/59783/

[img-add-to-launcher-dialog]: add-to-launcher-dialog.png
[img-add-to-launcher-menu]: add-to-launcher-menu.png
[img-launcher-menu]: launcher-menu.png
[img-webapp-window]: webapp-window.png

[repo-webshell-window]: https://github.com/if1live/webshell-window


