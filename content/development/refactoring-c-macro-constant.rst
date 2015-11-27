매크로 상수를 변경하기 쉽게 바꾸는 편법
=======================================
:subtitle: globals.h 고쳐도 전체 빌드 안하기
:tags: define, constant, macro, refactoring
:slug: refactoring-c-macro-constant
:author: if1live
:date: 2015-11-27

..  code-include:: refactoring-c-macro-constant/main.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 15
	:end-line: 20

프로젝트를 진행하면 아무리 설계를 잘해도 여러 파일에서 사용되는 상수를 피할수 없다.
이때는 보통 공유하는 상수/함수 등을 모아서 별도의 헤더 파일을 만들어서 관리한다.

* `Quake2/game/g_local.h <https://github.com/id-Software/Quake-2/blob/master/game/g_local.h>`_
* `DOOM3-BFG/doomclassic/doom/defs.h <https://github.com/id-Software/DOOM-3-BFG/blob/master/doomclassic/doom/defs.h>`_

예를 들어 위의 코드에서는 ``PROJ``, ``NAME``, ``VERSION`` 이 상수이다.
그리고 간단하게 매크로를 이용해서 상수를 구현했다.

..  code-include:: refactoring-c-macro-constant/simple_macro.h
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 3
	:end-line: 6

작은 규모에서는 이런식으로 상수를 사용해도 별 문제가 없다.
하지만 프로젝트가 커지면 심각한 문제가 생긴다.

예를 들어 현재 작업하고 있는 프로젝트를 전체 다시 빌드하면 10분 걸린다고 가정하자.
위의 프로젝트 전체에서 쓰이는 헤더파일에서 상수값 하나를 바꿔야될 일이 생겼다.
상수를 고치면 헤더파일이 바뀌었기때문에 의존성이 걸린 소스파일을 전부 다시 빌드해야된다.
상수값 하나 바꿨다고 10분이 사라진다.
상수를 바꾸고 테스트해봤는데 아무래도 잘못고친거 같은면 다시 10분을 기다려야한다.

..  figure:: {filename}/static/refactoring-c-macro-constant/compiling.png
	:alt: COMPILING
	:align: center

	`COMPILING <https://xkcd.com/303/>`_

상수값을 바꿔도 전체 빌드하는걸 피할수 있도록 코드를 고쳐보자.

매크로와 변수를 연결시키기
##########################

..  code-include:: refactoring-c-macro-constant/variable_is_macro.h
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 3
	:end-line: 13

..  code-include:: refactoring-c-macro-constant/variable_is_macro.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 2
	:end-line: 6

상수를 헤더에 때려박아서 상수값을 바꿀때 헤더가 바뀌는게 문제다.
그렇다면 헤더를 고정하고 소스파일에 값을 넣으면 되지 않을까?
이렇게 하면 상수값을 바꾸면 소스 파일만 고치고 변경된 파일에 대해서만 새로 컴파일 하면된다.

1. 전역변수, 싱글턴 등을 적절히 준비
2. 매크로를 전역변수, 싱글턴으로 연결
3. 상수를 사용하는 코드는 영향을 받지 않는다.


매크로와 함수를 연결시키기
##########################

일반적인 경우는 매크로와 변수를 연결하는거로 충분하다.
하지면 약간의 기능이 더 필요할때가 있다.
다음의 상황을 어떻게 구현할지 생각해보자.

* ``APP_URL`` 이라는 상수가 있다. 상수에는 ``http://en.libsora.so`` 이 연결되어있다.
* ``APP_URL`` 을 사용하는 곳이 한두군데가 아니다. 프로젝트 전체에서 ``APP_URL`` 상수가 사용된다.
* 어쩌다보니 다국어 지원을 해야된다. 옵션에서 한국어로 설정하면 ``http://kr.libsora.so`` 를 써야한다.
* 이제와서 ``APP_URL`` 상수를 사용하는곳이 너무 많아서 전부다 손보는건 좀 아닌거같은데...

``APP_URL`` 은 상수긴 하지만 상수라고 하긴 미묘하다.
컴파일 시간에 값이 정해지지 않는다.
어떤 값이 될지는 런타임이 되어야 알수있다.
그런데 이제와서 코드를 다 뜯어고기치긴 귀찮다.
이럴때는 **매크로==치환** 이라는 점을 이용해서 매크로를 함수에 연결하면 된다.

..  code-include:: refactoring-c-macro-constant/main.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 15
	:end-line: 27

::

	proj: use function and macro
	name: 春日野 穹
	version: 3
	name: kasugano sora

위의 예제에서 ``NAME`` 은 상수처럼 보이지만 설정값을 바꿔주면 값이 바뀐다.

..  code-include:: refactoring-c-macro-constant/function_is_macro.h
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 3
	:end-line: 15

..  code-include:: refactoring-c-macro-constant/function_is_macro.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2
	:start-line: 2
	:end-line: 29

1. 원하는 상수값을 얻을수 있는 전역함수, 싱글턴 등을 적절히 준비
2. 매크로를 함수에 연결
3. 상수를 사용하는 코드는 영향을 받지 않는다.

Sample Source
#############
* https://github.com/if1live/libsora.so/tree/master/content/development/refactoring-c-macro-constant
