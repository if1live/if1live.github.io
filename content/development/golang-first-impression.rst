Go language 첫인상
=====================================
:subtitle: Go language 입갤
:tags: go, golang
:slug: golang-first-impression
:author: if1live
:date: 2015-12-20

개요
####

주력 언어를 바꾸는건 어려운 일이다.
주력 언어를 바꾸면 상당히 많은걸 버려야하기 때문이다.
익숙한 세계를 버리고, 지금까지 배운 라이브러리를 잊고, 지금까지 배운 상식까지 버려야할 수도 있다.

그럼에도 불구하고 현재의 주력 언어인 파이썬, 루비, C++를 버리고 다른 언어로 갈아타기로 했다.
기존의 주력 언어에 남아서 어떻게든 문제를 해결하려고 삽질하는 것보다
내가 지금 처한 문제가 해결되어있는 새로운 언어로 갈아타서 다시 공부하는게 빠르겠더라.

다음 주력 언어로는 Go language를 찍었다. 결정한 이유는 크게 3가지이다.

첫번째 이유는 옆애 erlang, Go language를 찬양하는 아저씨가 옆에 있어서다.
erlang은 나도 공부 약간 해봤는데 좀 아니다 싶어서 접었고
(안정성, 병렬프로그래밍 측면에서는 좋지만 erlang으로 짜면 나중에 퇴사 못하겠더라)
Go language는 옆에서 하는거 보니까 괜찮아보이더라.

두번째 이유는 docker가 go로 구현되어있어서다.
docker같이 널리 쓰이고 있는 프로젝트가 go로 구현되어있다는걸 두가지 의미로 해석했다.
첫번째는 Go language 스펙이 하루아침에 바뀔리가 없다는거다.
만약 Go language가 스펙이 바뀔정도로 불안정한 개발단계의 언어면 docker팀이 go를 고르진 않았을거다.
다른 이유는 좋은 언어 공부 교재가 있다는거다. 나는 언어를 문서로 공부하면 막상 코딩은 못하겠더라.
그래서 내 경우 less를 bootstrap 코드 보면서 공부했다. go의 경우는 docker가 좋은 예제가 될거라고 생각했다.

세번째 이유는 "How We Moved Our API From Ruby to Go and Saved Our Sanity" 문서 때문이다.
http://blog.parse.com/learn/how-we-moved-our-api-from-ruby-to-go-and-saved-our-sanity/
글쓴이는 Ruby on Rails를 쓰다가 Go language로 뛰쳐나갔고 괜찮은 선택지였다고 썻다.
그리고 나도 Ruby on Rails를 쓰고있고 최근들어 레일즈의 한계를 느끼고 있다.

갈아타고자 마음먹고 Go language 공부를 시작해서 간단한 사이트 정적 생성기를 짯다.
https://github.com/shipduck/umi
그리고 처음 Go language를 공부하면서 느꼈던걸 정리했다.

History
-------

* 2015/11/30 2300 : https://go-tour-kr.appspot.com/ 읽기 시작
* 2015/12/2 : 봇잼 (개소리의 시작)
* 2015/12/3 : 첫번째 Go language 프로젝트 시작
* 2015/12/4 : go 코딩 마저하기.  800줄 짜리 간단한 코드 작성 완료
* 2015/12/5 : Go language 첫인상 정리하자! 라고 생각만 함
* 2015/12/20 : 이거 쓰는중


승리의 컴파일 시간
##################
Go language를 한번 써보겠다고 마음먹음 가장 큰 이유다.
현재 회사에서 작업중인 C++ 프로젝트는 전부 컴파일할때 10분정도 걸리는데 이렇게 버려지는 시간이 아깝더라.
컴파일 시간이 짧으면 제한된 시간동안 더 자주 실행하고 테스트하고 개선할 수 있다.
스크립트 언어급의 속도로 개발 이터레이션을 돌릴 수 있다는게 매력적이었다.

Garbage Collection
##################

가상머신 위에서 돌아가는 언어도 아니고 인터프리터 위에서 돌아가는 언어도 아닌데 Garbage Collector가 붙어있다고???

make가 없어도 되네?
###################
커맨드 라인에서 ``go`` 를 실행하면 여러가지 명령이 뜬다. ``go run`` , ``go test`` , ``go build`` ,...
C/C++이었으면 makefile을 만들고 run, test, build등에 대해서 어떤 기능을 수행할지 짜야한다.
makefile 을 굳이 만들 필요가 없어서 편하더라.

들여쓰기
########

나는 코드에서 들여쓰기는 필요악의 존재라고 생각한다.
들여쓰기가 없는 코드는 읽는게 매우 어렵다.
하지만 들여쓰기를 열심히 하면 화면 왼쪽이 쓸데없이 버려지는 느낌이 들더라.
(내 경우 특별한 문제가 없다면 들여쓰기를 스페이스 2글자로 사용한다)
게다가 들여쓰기가 많아질수록 80-columns 같은 정책을 지키기 어렵다.

들여쓰기로 버려지는 왼쪽 영역을 보여주는 C# 예제이다.
들여쓰기 정책은 스페이스 4칸, 4레벨, 합쳐서 16칸의 왼쪽 영역이 버려진다.

1. 모듈화를 생각하면 namespace는 써야겠지?
2. class를 피하는건 불가능하겠지.
3. 함수니까 들여쓰기.
4. for/if 같은 코드는 당연히 나오겠지?

..  code-include:: golang-first-impression/indent.cs
	:lexer: csharp
	:encoding: utf-8

C# 예제와 동일한 기능을 수행하는 Go 예제이다.
들여쓰기는 정책은 스페이스 4칸으로 같지만 2레벨 밖에 사용하지 않는다. 8칸의 왼쪽 영역이 버려졌다.
Go lang은 파일이 모듈 역할을 수행해서 namespace로 인한 들여쓰기가 없다.
구조체 안에서 함수를 구현하는게 아니라 밖에서 구현하기 때문에 class로 인한 들여쓰기도 없다.
그래서 C# 같은 언어에 비해 들여쓰기 2레벨을 아낄수 있다.

..  code-include:: golang-first-impression/indent.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

간단한 모듈 이름
################

파이썬 코딩할떄는 개떡같은 모듈 이름을 매일같이 봤다.
게다가 파이썬의 경우는 모듈 import 방법이 한개가 아니다.
``from ... import ...`` 를 쓰는 경우도 있지만 이름 충돌때문에 ``import ...`` 를 쓸때도 있다.
모듈을 갖고오는 방식이 두가지니까 사용할때의 모습도 두가지다.

* ``from django.contrib.flatpages.forms import FlatpageForm`` + ``FlatpageForm()``
* ``import os.path`` + ``os.path.join()``

Go language의 경우는 아무리 여러개의 token의 모듈(``foo.bar.spam.dummy.demo``)을 가져와도 마지막 토큰을 모듈 이름으로 사용한다.
그래서 코드가 짧게 느껴지더라.


string, slice, map : 문법 내장 자료구조
#######################################

C#, C, C++ 같은 언어와 Go language를 비교하면 기본적으로 지원하는 자료구조에서 차이가 있다.

* C : 되는게 없다
	* 문자열이라는 타입이 없다. 문자열은 char의 배열일뿐.
	* 배열은 있지만 추가삭제는 안된다. 이게 다 컴퓨터 친화적인 언어라서...
	* map? 그런 고급 자료구조는 기본 라이브러리에 없다.
* C++ : STL 에 포함됨. 뭘 해도 header include
	* 문자열이 표준라이브러리에 추가되었다. ``std::string``. header include
	* ``std::vector``, ``std::array`` 가 라이브러리에 추가됨. header include
	* ``std::map`` 라이브러리에 추가됨. header include
* C# : string은 내장, 나머지는 라이브러리
	* string은 문법에 포함되어있음. 이제 인클루드 안해도 된다
	* ``using System.Collections.Generic``
* Go language : 내장
	* string은 문법에 포함되어있다.
	* c++의 vector 정도로 쓸수있는 slice라는게 문법에 포함되어있음
	* map이 문법에 포함되어있음.

자주 사용하게 되는 자료구조인 리스트, 맵을 문법 수준에서 지원하는게 마음에 든다.

..  code-include:: golang-first-impression/data_structure.cs
	:lexer: csharp
	:encoding: utf-8

..  code-include:: golang-first-impression/data_structure.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

Remember, No Generic.
#####################

Go language에는 제네릭(Generic)이 없다.
그래서 호랑이 담배피던 시절의 자바를 보는 기분이다.
(옛날 옛적 자바에는 제네릭이 없었다. 넣은걸 꺼내면 Object 타입이라서 적절히 캐스팅해서 사용했다)

..  code-include:: golang-first-impression/no_generic.java
	:lexer: java
	:encoding: utf-8
	:tab-width: 4

Go language의 내장 라이브러리중에서 `이중 링크드 리스트, container/list <https://golang.org/pkg/container/list/>`_ 가 있다.
제네릭이 없는 덕분에 아무거나 리스트에 넣을수 있다는 장점(?)은 있지만 꺼낼때 타입확인하기 귀찮다는 치명적인 단점이 있다.
요즘 언어 주제에 제네릭이 없다니... 무슨 스크립트 언어도 아니고.

그래도 slice, map가 문법 내장이라서 아주 빡칠일은 많을거같지 않다.
(C++ 할때도 템플릿 제일 자주 쓰던게 ``std::vector<Foo>``, ``std::map<Foo>`` 정도였으니까...)

..  code-include:: golang-first-impression/no_generic.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

문법나치
########

C에서는 안쓰는 변수가 있어도 경고 레벨을 올리지 않으면 조용하다.
안쓰는 헤더를 인클루드해도 조용하다.
덕분에 나중에 뒤통수 맞는 코드(쓸데없는 헤더를 너무 인클루드해서 컴파일 시간이 우주로 가거나 신경 안썻던 변수가 의도치 않게 사용되거나)를 볼 수 있다.

..  code-include:: golang-first-impression/unused.c
	:lexer: c
	:encoding: utf-8

::

	$ clang -o unused unused.c -W -Wall
	unused.c:4:7: warning: unused variable 'unused_variable' [-Wunused-variable]
	  int unused_variable = 0;

Go language의 경우는 이런걸 철저하게 지킨다.
쓸데없는 모듈을 import하면 에러가 난다. 또는 안쓰는 변수가 있으면 문법 에러로 취급한다.
처음에 이런 특징을 보았을때는 완성된 코드가 깔끔할테니 좋을거라고 생각했다.
다음에는 개발하면서 "fmt" 모듈을 디버깅하기 위해서 넣었다가 디버깅 끝나고 print문 지우면서 같이 지워주는게 짜증나더라.
그다음에는 goimports와 emacs를 연동해서 파일 저장할때 자동으로 import 정리해주니까 좋더라.

Go language를 제대로 개발하려면 최소한 vi, emacs급의 에디터와 이것이 적절히 세팅되어 있어야 한다.
그런 것도 없으면 짜잘한 문법 에러를 자주 봐서 짜증난다.

..  code-include:: golang-first-impression/unused.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

::

	# command-line-arguments
	./unused.go:6: unusedVariable declared and not used

네이티브 언어 주제에 리플렉션이라니!
####################################

리플렉션(Reflection)은 JVM, CLR 같은 가상머신이나 파이썬같은 Interpreter에서는 기본적으로 제공되는 기능이다.
하지만 Go language는 네이티브 언어다. 가상머신위에서 굴러가지 않는다.
그렇기 때문에 비슷한 수준의 언어인 C++과 비교해보았다.

C++에는 리플렉션이 없다. 83년에 나온 언어에 뭘 기대하는가? (C++/CLI 는 제외하자. 그걸 표준 C++ 이라고 부를수 있을까?)
대신 RTTI(Run-time type information)는 있다.
리플렉션만큼의 거창한 기능은 없지만 ``typeid()`` 를 사용해서 객체, 포인터의 타입을 알아낼수 있다.
디버깅같은 목적으로는 가끔 쓸모있지만 몇가지 문제가 있다.

1. 컴파일러마다 결과가 다르다. Visual C++, clang++로 다음 코드를 돌려보면 결과가 다르게 나온다.
2. 알수있는 정보는 타입 이름뿐이다. 객체안에 어떤 멤버변수가 있는지, 값이 무엇인지에 대한 정보는 얻을수 없다.

이런 문제가 있어서 C++의 경우는 RTTI이 필요하면 C++ 내장 RTTI 대신 Custom-RTTI를 구현해서 사용한다.

* `c++ 에서 구조체 RTTI 정보 남기기 <http://lacti.me/2011/09/30/using-rtti-at-cpp/>`_
* `c++에서 reflection 사용하기 1 <http://lacti.me/2012/06/03/using-reflection-at-cpp-1/>`_
* `c++에서 reflection 사용하기 2 <http://lacti.me/2012/06/04/using-reflection-at-cpp-2/>`_
* `c++에서 reflection 사용하기 3 <http://lacti.me/2012/06/09/using-reflection-at-cpp-3/>`_
* `c++에서 reflection 사용하기 4 <http://lacti.me/2012/06/09/using-reflection-at-cpp-4/>`_

..  code-include:: golang-first-impression/rtti.cpp
	:lexer: cpp
	:encoding: utf-8

::

	## vc++'s output
	class Sample
	class Sample *

	## clang++'s output
	6Sample
	P6Sample

Go language의 경우는 %T, %t, %v, %#v 를 이용해서 객체의 정보를 찍어볼 수 있다.
struct에 저장된 값 정보도 볼수 있으니 C++의 RTTI보다 더 유용하게 쓸수있다.

게다가 `reflect <https://golang.org/pkg/reflect/>`_ 패키지도 있다.
네이티브 언어 주제에 본격적인 리플렉션이 된다.
그래서 C++과는 달리 흑마법을 사용할 필요없다.

..  code-include:: golang-first-impression/rtti.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

::

	%T  : main.Sample
	%t  : {%!t(int=1) %!t(uint8=97)}
	%v  : {1 97}
	%#v : main.Sample{a:1, b:0x61}
	%T  : *main.Sample
	%t  : &{%!t(int=1) %!t(uint8=97)}
	%v  : &{1 97}
	%#v : &main.Sample{a:1, b:0x61}

왠지 안쓸거같은 private
#######################

객체지향 프로그래밍에는 캡슐화(Encapsulation)이라는 개념이 있다.
나는 캡슐화를 한줄로 표현할때 이런 표현을 쓴다.
``당신은 알것만 알면 됩니다``
클래스 외부에서 알아도 되는건 public으로 쓰고 클래스 내부에서만 알아야하는건 private라고 쓰자.
클래스 밖에서는 private에는 접근할수 없다.

public/private 의 개념을 설명하고자 구현한 루비 예제이다.

..  code-include:: golang-first-impression/private.rb
	:lexer: ruby
	:encoding: utf-8

::

	this is public
	this is private
	Error: private method `private_method' called for #<Sample:0x007f94e6069578>

캡슐화라는 개념은 파이썬에도 있다.
파이썬의 경우는 public, private 키워드가 없기 때문에 편법으로 구현했다.
인스턴스 메소드의 이름이 ``__`` 로 시작하면 private 으로 취급한다.
개념 자체는 간단하지만 실제로 사용해보면 치명적인 문제가 있다.

처음에 개발할때는 설계가 언제 바뀔지도 모르고 테스트하기 쉽도록 public으로 메소드를 만들었다.
어느정도 개발하고나니까 public으로 만들었던 메소드 중 일부를 private으로 바꾸고 싶더라.
근데 public을 private로 바꾸려면 이름을 바꿔야한다?
게다가 파이썬은 컴파일 언어가 아니니까 실수로 한곳의 이름을 바꾸지 않아도 호출만 하지 않으면 잘 돌아간다?
차라리 private/public을 선언하는 언어였으면 해당 메소드 앞에만 private를 붙여서 문제를 해결했을텐데.

내 경우 위의 시나리오를 몇번 겪었다.
나중에는 private 문법 있는거 알고있으면서도 안 쓰게 되더라.

..  code-include:: golang-first-impression/private.py
	:lexer: python
	:encoding: utf-8

::

	this is public
	this is private
	cannot access private in this scope


Go language의 경우는 대문자로 시작하면 외부에 public으로 간주하고 소문자로 시작하면 private로 간주하다.
이름이 바뀌어야한다는 점에서 파이썬하고 비슷하다.
파이썬에서 했던 짓을 반복할거같은 기분이 들더라.
(그래도 Go language는 파이썬과 달리 컴파일 언어기때문에 잊어버리고 한군데의 함수 이름을 바꾸지 않았다거나 하는 일은 없다)

..  code-include:: golang-first-impression/private/private.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

..  code-include:: golang-first-impression/private_main.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

Call by Value, Call by Reference
################################

함수호출에서 인자를 넘기는 방식은 Call by Value, Call by Reference 가 있다.
인자가 int 같은 primitive type 인 경우와 object 인 경우의 정책이 보통 다른데 object일때만 생각하자.

자바의 경우는 object는 Call by Reference 로 넘어간다.
object를 Call by Value로 넘기는 방법은 없다.
굳이 하고싶으면 object를 적절히 복사해서 복사된 것을 Call by Reference로 넘기면 될거다.
개인적으로는 call by reference, call by value 중 어떤것을 선택해서 함수를 호출할지를 내가 정할 수 있으면 좋겠다.

..  code-include:: golang-first-impression/call_by_xxx.java
	:lexer: java
	:encoding: utf-8
	:tab-width: 4

::

	before call by reference : 0
	in function : 1
	after call by reference : 1


C#의 경우는 class와 struct가 있다. C++의 class, sturct와는 다르게 C#에서의 그것은 동작이 완전히 다르다.
class는 heap 영역에 객체가 생기고 struct는 stack 영역에 객체가 생긴다는 것 이외에도 다른게 있다.
함수 인자로 넘어갈때 정책이 다르다.
class는 call by reference로 넘어가지만 struct는 call by value로 넘어간다.
자바와는 달리 call by value가 가능하다는건 좋지만 개념 자체가 간단하진 않다.
class/struct라는 서로 다른 2개의 시스템이 공존하고 있고 함수만 보고 이것이 call by value로 작동할지 call by referece로 작동할지 알기 어렵다.
(예제에서는 보기쉽게 SampleStruct, SampleClass라고 했지만 현실에서는 한눈에 class인지 struct 보일까?)

..  code-include:: golang-first-impression/call_by_xxx.cs
	:lexer: csharp
	:encoding: utf-8

::

	before call by value : 0
	in function : 1
	after call by value : 0
	before call by reference : 0
	in function : 1
	after call by reference : 1


Go language의 경우는 C++과 유사하다. 포인터가 있다.
object를 그냥 넘기면 call by value로 작동하고 포인터를 넘기면 call by referece로 작동한다.
C++을 하루이틀 한게 아니니까 포인터라는 개념이 익숙해서 Go language의 방식이 마음에 들더라.

..  code-include:: golang-first-impression/call_by_xxx.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

::

	before call by value :  0
	in function :  1
	after call by value :  0
	before call by reference :  0
	in function :  1
	after call by reference :  1

손쉬운 병행성
#############

요즘 세상에서는 멀티코어를 제대로 활용해서 성능을 뽑아낼수 있다.
C++11 부터는 이런 시대의 흐름을 따라가서 thread 헤더가 표준에 포함되었다.
표준 thread 라이브러리를 이용하면 다양한 플랫폼에서 돌아가는 멀티 쓰레드 프로그램을 작성하는 것이 가능하다.

..  code-include:: golang-first-impression/thread.cpp
	:lexer: cpp
	:encoding: utf-8

하지만 쓰레드라게 무엇인가? 운영체제 배우면 나오는 개념 아닌가?
그렇다면 low-level 의 개념이라는거 아닌가?

코드에서 2개의 thread를 쓴다고 작성했으면 2개의 OS thread가 생성될 것이다.
만약 32개의 thread를 쓴다고 작성했으면 32개의 OS thread가 생성될 것이다.
내가 짠 프로그램은 듀얼코어 CPU에서 돌아갈 수도 있고 64코어 CPU에서 돌아갈 수도 있다.
듀얼코어 CPU에서 32개의 thread를 생성하면 효율적일까? 64코어 CPU에서 2개의 thread를 생성하면 효율적일까?
게다가 기능 개발하느라 바쁜데 쓰레드를 몇개 생성해서 돌리는게 가장 성능이 좋을지 같은 영역까지 신경쓸 시간이 있을까?

OS thread 에서 멀리 떨어져서 바라보면 위의 문제를 간단하게 만들수 있지 않을까?

work queue가 있고 나는 queue에 작업을 밀어넣는다.
work queue에 작업이 있다는걸 확인하면 프로그램이 thread pool에서 쓰레드를 하나 꺼내서 작업을 수행한다.
thread가 작업을 끝내면 결과값을 어딘가에 적절히 저장해두고 thread는 다시 thread pool에 넣는다.
thread pool의 크기는 CPU 코어수에 맞춰서 프로그램이 알아서 결정한다.
적당히 작업이 끝났겠지 싶을때 계산한 결과를 읽는다. 만약 계산이 끝나지 않았으면 끝날때까지 대기한다.

이런식으로 구현하면 문제가 간단해지지 않을까?
C++11 부터는 ``std::async``, ``std::future`` 가 추가되었다.
위와 같은 컨셉으로 돌아간다. 그래서 이전의 쓰레드 예제와 달리 std::async를 10000개 만든다고 OS thread가 10000개 생기진 않는다.

``std::async`` 를 이용해서 작업을 생성하고 처리한다.
처리된 결과는 ``std::future`` 를 이용해서 나중에 받을수 있다.

..  code-include:: golang-first-impression/worker.cpp
	:lexer: cpp
	:encoding: utf-8

Go language에서는 go routine, channel이라는 개념을 이용해서 이를 해결했다.
go routine을 이용해서 작업을 생성한다.
channel을 이용해서 결과값을 받는다.

std::future나 channel이나 비슷해보이지만 실제로 사용할때는 접근법이 다른 느낌이다.
std::future의 경우는 반환값을 미래(future)에 받아본다는 느낌이다.
channel은 파이프가 있고 데이터를 한쪽에서 밀어넣고 한쪽에서는 꺼내보는 느낌이다.
그래서 하나의 채널로 여러 go routine이 통신하는 것도 채널을 닫는것(close)도 가능하다.

..  code-include:: golang-first-impression/worker.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

Go routine은 생각없이 쓰기에는 편해서 좋지만 가끔 통수칠때가 있더라.
적절한 형태의 루프 안에서 call by reference로 go routine을 만들면 문제가 생기더라.
Go language 공부 3일차에서 이 문제를 당해서 기억에 남더라.

..  code-include:: golang-first-impression/broken_goroutine.go
	:lexer: go
	:encoding: utf-8
	:tab-width: 4

::

	Call by Value + go routine
	3
	1
	2
	Call by Reference + go routine
	3
	3
	3


생각보다 쓸만한 내장 템플릿 라이브러리
######################################

Go language에는 템플릿 라이브러리가 포함되어있다.

* `text/template <https://golang.org/pkg/text/template/>`_
* `html/template </https://golang.org/pkg/html/template/>`_

언어 내장 템플릿 라이브러리 주제에 안에서 비교, 분기, 루프 등의 동작을 지원한다.
어정쩡한 템플릿 엔진보다 내장 템플릿 엔진에 기능이 더 많다.
(내 경우 내장 템플릿 엔진이 당연히 후질거라고 생각해서 mustache를 쓰려고했는데 내장을 썻다. 훨씬 기능이 많더라)


CPU 사용률 100% 돌파
####################

C, C++, Java 코딩 많이 하는 사람한테는 당연한 일이겠지만 나는 최근 3년동안 파이썬, 루비로 먹고살았다.
파이썬, 루비의 경우는 메모리 관리를 쉽게 하려고 GIL(Global interpreter lock)를 이용해서 구현했다.
덕분에 언어 내장 쓰레드 라이브러리를 써도 CPU는 하나밖에 못쓴다.
(CPU를 여러개 사용하고 싶으면 쓰레드가 아니라 프로세스를 여러개 만들어야한다)

파이썬, 루비와 마찬가지로 Garbage Collector 달고있는 언어 주제에 Go language에는 GIL이 없다.
오랜만에 CPU 사용률 100% 돌파하는 프로그램 짜보니까 신선하더라.

Summary
#######

::

	이상적인 언어의 조건
	우아한 문법
	없는게 없는 라이브러리
	5년전 코드를 지금도 돌아갈 수준의 하위호환성
	갓고수가 많은 커뮤니티
	빠른 이터레이션
	궁극의 성능
	쉬운 메모리 관리
	언제든지 사람을 뽑을수 있는 인력풀
	..
	차라리 이상적인 애인을 찾고말지

https://twitter.com/if1live/status/672691933433585664

세상에 완벽한 언어는 존재하지 않는다. 어느정도 수준에서 타협을 해야한다.
내 경우는 다음을 중요한 가치로 생각한다.

* 빠른 개발 주기 : 빨리 개발해서, 실행하고, 테스트하고, 개선한다.
* 적은 들여쓰기 : C++, 자바쓸때 버려지는 왼쪽 여백이 아까웠다
* 멀티 코어 : 병렬 프로그래밍은 그냥하면 어려우니까 언어에서 지원 해주는게 많으면 좋겠다.
* GC : 메모리 관리 신경쓸 시간에 추가 기능 구현하는게 돈이 된다.

Go language은 내가 중요하게 생각하는 가치에서는 높은 점수를 줄 수 있다.
그래서 나는 주력언어를 Go language로 갈아탈 생각이다.
이번에 갈아타면 앞으로 2-3년 정도는 Go language를 쓰게 되겠지.
