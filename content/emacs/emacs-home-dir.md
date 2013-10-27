Title: 윈도우 환경에서 emacs HOME 디렉토리 변경하기
Tags: emacs
Slug: emacs-home-dir
Author: if1live
Summary: 윈도우 환경에서 emacs HOME 디렉토리 변경하기
Date: 2012-12-31

유닉스 환경의 경우 이맥스 설정파일은 `/home/user_name/.emacs`에 위치한다. 그런데 윈도우라면 저런 경로가 존재할리가 없잖아? 윈도우의 경우는 `C:/Users/user_name/blabla....` 어딘가에 적절히 위치한다. (볼때마다 까먹어서 기억도 안난다..)

저렇게 찾기 어려운 위치에 설정파일이 존재하는것보다는 `c:/home` 처럼 찾기 쉬운곳에 이맥스 설정파일이 존재하는것이 좋지 않겠는가? 해결책은 크게 2가지가 존재한다

### 전역 HOME 환경 변수 추가
시스템 변수로 HOME을 추가하면 된다. 이맥스가 아닌 다른 프로그램에 대해서까지 영향을 받으니까 개인적으로는 별로 안좋아한다.

### emacs 한정 HOME 환경변수 추가
이맥스에서만 HOME 변수를 새로 만들어서 쓰는 방법이다. 윈도우용 이맥스를 설치한 경로에 가서 `site-lisp`라는 디렉토이에 `site-start.el`이라는 파일을 만들고 내용을 다음과 같이 한다.

```cl
(defvar %~dp0 (substring data-directory 0 3)) (defvar home-dir (concat %~dp0 "home/"))
(setenv "HOME" home-dir)
```

이맥스가 C드라이브 어딘가에 존재한다면 `c:/home`가 앞으로 HOME 디렉토리로 동작한다. 

## 응용 예시
이맥스를 USB메모리에 집어넣는 다음에 위와 똑같은 방식으로 하면 `<usb_drive>:/home`가 HOME 디렉토리로 동작한다. portable emacs를 만드는데 사용할수 있는 편법이다.
