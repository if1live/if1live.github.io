---
title: 한글 입력을 지원하는 크롬OS 터미널을 찾아서
subtitle: 발상의 전환, 생각보다 가까운 답
tags: [hangul, chrome os]
slug: find-chrome-os-terminal-which-support-cjk-input
author: if1live
date: 2018-02-11
url: /posts/find-chrome-os-terminal-which-support-cjk-input
---


## 개요

2017년 7월에 Acer R11 chromebook를 샀다.
그리고 크롬OS에서 한글 입력이 제대로 되는 터미널을 돌리기 위해서 삽질한 기록을 요약했다.
비영어원 개발자가 크롬OS를 주웠을때 어떤 고통을 받는지 다들 구경해보자.


## 크롬 OS 내장 터미널

크롬OS에서 개발자 모드를 활성화 시키면 Ctrl-Alt-T 를 눌러서 터미널에 접근할수 있다.
크롬OS에는 패키지 관리자가 내장되어 있지 않지만 [chromebrew][site-chromebrew]라는 패키지 관리자를 설치하면 적당히 쓸만한 리눅스가 된다. (chromebrew는 osx의 brew 정도의 프로그램이라고 생각하면 된다)

Ctrl-Alt-T를 누르면 터미널이 새탭으로 뜬다.
그것보다는 독립된 창으로 뜨면 사용할떄 편할 것이다.
chrome web store에서 [Crosh Windows][store-crosh]를 설치하면 된다.

터미널은 준비되었으니 한글을 입력해보자.

![crosh][video-crosh]

한글 입력이 뭔가 이상하다?

문제를 해결하는게 가능한지 궁금해서 소스를 찾아봤다.
[crosh repository][repo-crosh]
저장소 이름이 `chromium/chromiumos/platform2/master/./crosh` 이다.
crosh는 크롬OS에 내장된 쉘이라고 봐도 될거다.
크롬OS를 다시 빌드하지 않는 이상은 crosh의 버그는 못고친다는 소리같다.
나는 포기가 빠른 인간이라서 다른 방법을 찾기로 했다.

## 내장 쉘이 없으면 ssh 터미널을 쓰면 되지!

crosh 만이 크롬OS의 유일한 터미널이 아니다.
방법은 뒤지면 나온다.

크롬OS 위에서 굴러가는 여러가지 ssh 터미널이 있다.
요즘 크롬OS는 안드로이드로 지원하기 때문에 안드로이드 ssh 터미널도 있다.
크롬 OS에서 ssh server를 돌릴수 있다면 ssh 터미널과 붙여서 몇가지 시도를 더 해볼수 있을것이다.

[crouton][repo-crouton] 이라는 좋은 물건이 있다.

> crouton
> Chromium OS Universal Chroot Environment 

우분투같은 배포판을 크롬OS 위에 설치하고 chroot로 돌릴수 있다.
chroot로 돌리니까 시스템 설정을 건드지리 않고 우분투를 돌려볼수 있다.
crouton으로 설치한 우분투 안에 ssh 서버를 돌려두고 밖에서 ssh 터미널로 접속해보자.

크롬 웹스토어에 가면 ssh 터미널 구현체가 몇가지 있다.
[Secure Shell][store-secure-shell]라고 구글에서 만들어준 ssh 터미널도 있다.
이걸 이용해서 ssh 서버에 접속한 다음에 한글 입력을 해보자.

![secure shell][video-secure-shell]

한글 입력이 제대로 되지 않는다.
ascii세상 사람들이 만든 물건은 non-ascii세상 사람들에 대한 배려가 없다.

크롬에서 만든 secure shell 말고도 많은 크롬OS ssh 터미널, 안드로이드 ssh 터미널을 테스트해봤다. 하지만 내가 원하는 수준에 도달한건 없었다. 그래서 포기했다.

## 포기하면 편해. 그냥 우분투를 쓰자

위에서 crouton으로 우분투를 설치할수 있다고 했다.
우분투를 설치할때 옵션을 바꾸면 GUI 지원도 넣을수 있다.
크롬OS를 포기하고 보통의 리눅스 배포판을 선택하면 고민할게 사라진다.

리눅스 배포판에서 한글을 지원하기 위한 삽질의 역사는 거의 20년이다.
그리고 반도에는 크롬OS를 쓰는 사람보다 그냥 우분투를 쓰는 사람이 훨씬 많다.
크롬OS를 버리고 우분투를 쓴다면 어떻게든 방법을 찾아낼수 있다.

그래서 crouton + xfce4 + fcitx 를 조합해서 써봤다.
한글 입력에서는 더이상 문제가 없었다.
그렇다고 내가 만족한건 아니다.

Ctrl-Alt-Shift + F1/F2를 누르면 크롬OS와 Crouton사이에 화면 전환이 가능하다.
이런 모습으로 화면 전환이 된다.

{{< youtube i8cQ_SZQSRw >}}

OS간의 화면 전환이 빠른것도 아니고 화면 전환할때마다 화면이 깜빡인다.
OS하나에서만 쓴다면 모를까 계속 왔다갔다하면서 작업하기에는 불편하다.

또한 크롬OS와 우분투는 OS가 다르기때문에 단축키가 다르다.
크롬OS에서 볼륨을 올리고싶을때는 볼륨키만 누르면 되는대 우분투에서는 Search-볼륨키를 눌러야한다. 이런식으로 OS가 다르니까 단축키가 달라지는게 적응하기 어려웠다.

문제는 이것이 끝이 아니다.
둘은 서로 다른 OS이기 때문에 alt-tab으로 이동 불가능하다.
우분투를 깔았으면 우분투에서 모든 작업을 끝내야지 우분투와 크롬OS를 섞어서 작업하는건 불편했다.

그렇다고 크롬OS를 버리고 우분투만 쓰고 싶은 생각이 들지 않았다.
크롬OS에서 안드로이드를 공식적으로 지원한다고 발표했다.
그리고 초기부터 공식 지원한다고 발표한 모델 중 하나가 Acer R11이었다.
나는 이거 하나 때문에 다른 크롬북 대신 Acer R11을 샀다.
크롬OS를 버리면 안드로이드 지원도 버려야하는 문제가 생긴다.

불편한게 하나 더 있다.
리눅스 어플리케이션은 키보드/마우스로 조작하도록 UI가 설계되었다.
Acer R11에는 터치패드/키보드/터치스크린이 붙어있다.
그리고 나는 터치패드보다 터치스크린을 자주 썼다.
기존 X환경이 터치스크린에 맞게 설계되어있지 않아서 여러가지로 불편했다.

우분투에서 cli를 갖다쓰는건 상관없지만 일반 배포판의 X는 쓰고 싶지 않았다.
그래서 다른 방법을 찾기로 했다.

## 한글 입력이 되도록 이미 돌아가는걸 고치자

대륙에 secure shell을 개조해서 중국어 입력을 붙인 사례가 있더라.
[puritys/chromeSecureShell](https://github.com/puritys/chromeSecureShell)
그것을 참고하면 한국어 입력도 붙일수 있지 않을까?
크롬에서 [소스][repo-secure-shell]도 줬잖아?

그래서 소스의 README를 봤다.

> nassh/ is the Secure Shell Chrome App 
> (currently a "v1.5" app, soon to become a "v2" or platform app) 
> that combines hterm with a NaCl build of OpenSSH
> to provide a PuTTY-like app for Chrome users.

조금 더 기다리면 v2나온다는데 v1.5 붙잡고 삽질할라니 귀찮다.
게다가 크롬 앱과 관련된 문서를 보니 chrome app 을 더이상 권장하는 분위기가 아니었다.

[Transitioning from Chrome apps on Windows, Mac, and Linux][news-chrome-app-migrate]

아무래도 이제와서 크롬앱으로 터미널 만드는건 아닌거같다.
고생에 비해서 얻는게 적을거같다.
어차피 삽질할거 최신 기술로 삽질하는게 재밌지 않을까?

## termianl server <-> terminal client

오픈소스 터미널을 찾다보니 [Hyper][site-hyper]라는게 있더라?

> A terminal built on web technologies

electron으로 터미널을 구현했더라.
이런 느낌으로 하면 왠지 될거같다.

하지만 크롬OS에서는 웹앱이라는 개념이 데탑에서의 그것과 다르다.
electron이 안돌아간다. 크롬만 돌아간다.
chrome app도 크롬 위에서 돌아가는 물건이다.

hyper를 그대로는 못쓰고 이것을 참고해서 새로운걸 만들어보려고 했다.
`서버(터미널) <-> 클라(웹)` 같은 구조로 통신하는 걸 만드는거다.
클라 웹은 hyper를 보고 만들수 있을거 같다.
서버-클라는 웹소켓같은것으로 통신하게 구현하고.

1. crouton으로 리눅스 설치하고
2. 리눅스에서 쉘 띄우는 서버 접속하고
3. 크롬에서 서버에 접속해서 쉘을 다룬다.

이거는 왠지 될거같다.
근데 어디에서 본거같은데...?

## Web Shell

어디서 봤나 했더니 웹셀이 저런 구조로 굴러간다.
[Web Shell][info-webshell]은 보안이 망했다는 것의 상징이지만 나한테는 이게 필요했다.

몇개 찾아보다가 ttyd를 쓰기로 했다.
Features를 읽어보니 나한테 필요한 기능이 있더라.

> Fully-featured terminal based on Xterm.js with CJK and IME support

![ttyd][screenshot-ttyd]

## 다시 처음으로, 나한테 필요한게 무엇인가?

이제 스스로 뭘 원하는지 정의할수 있겠더라.

### 요구사항

1. 안드로이드 지원을 유지하고 싶다.
2. 터치 노트북의 사용성을 유지하고 싶다.
3. 크롬OS 작업 환경은 유지하고 싶다.
4. 한글 입력할때 문제없는 터미널이 필요하다.

### 진행 방향

1. web shell을 쓴다. ttyd로 확정
2. ttyd를 컴파일할때 필요한 라이브러리 설치를 쉽게 하고싶다.
3. crouton + ubuntu 16.04 xenial 사용하기로 확정
4. x.org 안쓸거니까 ubuntu 설치시에는 core만 있어도 된다.

### 시스템 구성

`chrome` in chrome os <-> `websocket` <-> `ttyd` in crouton

## 설치 과정

1. [crouton][repo-crouton] 설치
    * `sudo sh crouton -t core`
2. [ttyd][repo-ttyd] 설치
3. ttyd 실행
    1. `ttyd bash`
    2. `http://127.0.0.1:7681` 접속 후 한글 입력 테스트

![ttyd + locale=POSIX][video-ttyd-fail]

한글 입력이 제대로 되지 않는다.
`locale`를 실행하면 `POSIX`만 보인다.
로케일 설정을 하지 않아서 그렇다.
[crouton languages][crouton-languages] 를 참고해서 로케일 설정을 진행하자.

```bash
sudo apt-get install locales
sudo dpkg-reconfigure locales
```

`ko_KR.UTF-8`만 있어도 된다.

로케일 설정후 crouton을 로그아웃 했다가 다시 crouton을 실행하자.

`ttyd bash`로 ttyd를 실행후 `http://127.0.0.1:7681`에 다시 접속해보자.

![ttyd][video-ttyd]















[video-crosh]: video-crosh.gif
[video-secure-shell]: video-secure-shell.gif
[video-ttyd-fail]: video-ttyd-locale-fail.gif
[video-ttyd]: video-ttyd.gif


[site-hyper]: https://hyper.is/

[site-chromebrew]: http://skycocker.github.io/chromebrew/

[store-crosh]: https://chrome.google.com/webstore/detail/crosh-window/nhbmpbdladcchdhkemlojfjdknjadhmh
[repo-crosh]: https://chromium.googlesource.com/chromiumos/platform2/+/master/crosh/

[store-secure-shell]: https://chrome.google.com/webstore/detail/secure-shell/pnhechapfaindjhompbnflcldabbghjo
[repo-secure-shell]: https://chromium.googlesource.com/apps/libapps/

[news-chrome-app-migrate]: https://developers.chrome.com/apps/migration

[repo-gotty]: https://github.com/yudai/gotty
[repo-ttyd]: https://github.com/tsl0922/ttyd
[screenshot-ttyd]: ttyd-screenshot.gif

[info-webshell]: https://openwiki.kr/tech/webshell

[repo-crouton]: https://github.com/dnschneid/crouton
[crouton-languages]: https://github.com/dnschneid/crouton/wiki/Languages
