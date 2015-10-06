스택 지역변수의 주소값을 이용한 흑마법
======================================

:subtitle: 욕먹는 코드를 짜는 기법
:tags: cpp, stack allocation, bug
:slug: stack-allocation-voodoo-magic
:author: if1live
:date: 2015-10-06
:status: draft

.. _so-basic: http://stackoverflow.com/questions/1102049/order-of-local-variable-allocation-on-the-stack
.. _cppref-vector-at: http://en.cppreference.com/w/cpp/container/vector/at

개요를 적당히 쓴다

..  code-include:: stack-allocation-voodoo-magic/simple.c
	:lexer: c
	:encoding: utf-8
	:tab-width: 2

::

	gcc simple.c -W -Wall
	./a.out
	addr x : 7fffcb2d08a4
	addr y : 7fffcb2d08a8
	addr diff: -4

::

	clang simple.c -W -Wall
	./a.out
	addr x : 7fff5918a928
	addr y : 7fff5918a924
	addr diff: 4

버그 만들기
-----------

..  code-include:: stack-allocation-voodoo-magic/inf_loop_bug.c
	:lexer: c
	:encoding: utf-8
	:tab-width: 2

::

	gcc main.c -W -Wall
	./a.out
	addr i    : 7ffef5e604f4
	addr array: 7ffef5e604e0
	curr addr : 7ffef5e604e0
	curr addr : 7ffef5e604e4
	curr addr : 7ffef5e604e8

::

	clang main.c -W -Wall
	./a.out
	addr i    : 7ffddfb95ec4
	addr array: 7ffddfb95ebc
	curr addr : 7ffddfb95ebc
	curr addr : 7ffddfb95ec0
	curr addr : 7ffddfb95ebc
	curr addr : 7ffddfb95ec0
	curr addr : 7ffddfb95ebc
	ERROR: Infinitely Loop!!!


해결책
------

코드를 잘 짜자
##############

`for(i = 0 ; i <= ARRAY_SIZE ; ++i)`

배열을 접근하다 인덱스를 넘은 것이 버그의 원인이다.
코드를 짤때 신경써서 실수를 하지 않으면 버그가 생기지 않을 것이다.

하지만 이걸 해결책이랍치고 적어놓으면 무책임하다.
아무리 신경쓴다고 해도 사람은 실수를 하게 된다.
내가 실수를 안해도 다른 사람이 내 몫의 실수도 대신 해줄거다.

자료구조 변경
#############

std::vector::at
@@@@@@@@@@@@@@@

..  code-include:: stack-allocation-voodoo-magic/use_vector.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2

custom std::array
@@@@@@@@@@@@@@@@@

..  code-include:: stack-allocation-voodoo-magic/custom_array.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2

루프 인덱스를 없애자
####################

std::for_each
@@@@@@@@@@@@@

..  code-include:: stack-allocation-voodoo-magic/for_each.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2

enhanced for loop
@@@@@@@@@@@@@@@@@

..  code-include:: stack-allocation-voodoo-magic/enhanced_for_loop.cpp
	:lexer: cpp
	:encoding: utf-8
	:tab-width: 2

다양한 컴파일러, 다양한 환경에서 테스트
#######################################

..

	컴파일러 경고 수준은 최대한 높이는 것이 좋다.
	모든 플랫폼에 대해 경고 수준을 허용할 수 있는 한도 내에서 최대한 높게 설정할것
	(Visual Studio의 경우는 /W, gcc는 -Wall 등).
	컴파일러 중에는 특히 더 까다로운 것들이 있다.
	같은 코드를 여러 컴파일러들로 컴파일해보면, 코드를 실행시키기 전에라도 버그를 발견할 가능성이 높아진다.
	그리고 경고를 오류로 취급하게 하는 옵션도 활성화시킬 것.
	그러면 코드의 깨끗함을 유지할수 있으며,
	별 문제가 없어 보이는 코드를 다른 팀에게 배포했는데 그 팀이 작업하는 플랫폼에서 오류와 경고가 발생하는 일을 피할 수 있다.
	
	Game Programming Gems 4 - 1.4 커다란 크로스플랫폼 라이브러리의 설계와 유지

Environment
-----------

본문에 나오는 코드를 테스트한 환경이다.
다른 환경에서 테스트했을때도 같은 결과가 나온다는 보장이 없어서 적어둔다.

clang 3.4
#########

::

	$ clang --version
	Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)
	Target: x86_64-pc-linux-gnu

gcc 4.8.4
#########

::

	$ gcc --version
	gcc (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4

Links
-----

* `stack overflow : Order of local variable allocation on the stack`__

__ so-basic_
