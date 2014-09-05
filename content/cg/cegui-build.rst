Build CEGUI 0.8.2 on Windows + OGRE
================================================

:date: 2013-10-09
:tags: cegui, ogre, build, windows
:slug: cegui-build
:author: if1live
:subtitle: CEGUI 윈도우에서 컴파일하기

`CEGUI <http://cegui.org.uk/>`_ 빌드 하느라 삽질좀 해서 공유용 문서를 썻다. 어차피 요즘 대세는 유니티니까 팀원말고는 보지도 않겠지.

준비물
####################
* OGRE 빌드해놓기. 귀찮으면 prebuilt library가 있어도 된다. 오그레으로 샘플 한번이라도 제대로 빌드해봤으면 뭔소리인지 알거다
* `CEGUI 0.8.2 source <http://cegui.org.uk/download>`_ 에서 받을 수 있다. 문서 작성 시점의 안정버전이 0.8.2 라서 그거 쓴다.
* `Dependencies (Windows / Apple OS X Only) <http://cegui.org.uk/download>`_ 를 받는다. CEGUI는 의존성이 크고 아름답게 걸린 물건이라서 수동으로 관련 라이브러리 하나씩 깔면 미친다.
* cmake. 왜냐하면 나는 cmake 빠돌이니까

빌드 과정
#######################

CEGUI Dependencies
*****************************

cegui-deps-0.8.x-src.zip 압축을 적절히 푼다.
앞으로는 해당 경로를 **cegui-deps-path** 라고 부른다.
그리고 cegui-deps-path 안에 build 폴더를 만든다.
cmake 빌드 결과를 독립폴더로 분리하는건 그냥 개인취향이다(...)

cmake를 사용해서 의존 라이브러리를 빌드할 준비를 한다. cmake gui를 사용했다.

* Where is the source code = cegui-deps-path
* Where to build the binaries = cegui-deps-path/build

configure 돌리고 generate를 돌려준다. 생성된 cegui-deps-path/CEGUI-DEPS.sln을 적절히 빌드해준다. debug, release로 각각 빌드해 주는걸 잊지말자. 빌드가 끝나면 **cegui-deps-path/build/dependencies** 가 생겨있다.

CEGUI
*****************************
cegui-0.8.2.zip 압축을 적절히 푼다.
앞으로는 해당 경로를 **cegui-path** 라고 부른다.

cegui-deps를 빌드한 다음에 생긴 **cegui-deps-path/build/dependencies** 를 **cegui-path** 로 복사한다.

CEGUI가 OGRE를 지원하도록 빌드하기 위해서는 OGRE의 경로를 알려줘야한다. cmake gui 돌릴떄 변수를 추가해서 돌릴수도 있지만 귀찮은 관계로 cmake 파일 자체를 수정했다.

**cegui-path/cmake/FindOgre.cmake** 파일의 다음 라인을 찾는다. ::

	include(FindPackageHandleStandardArgs)

위의 줄 아래에 다음과 같은 느낌의 코드를 추가한다. OgreSDK를 설치한 경로이다. 자신의 경로에 맞게 적절히 수정한 다음에 넣는다. 첫번째는 경로, 나머지 2개는 라이브러리 파일이다. ::

	set(OGRE_H_PATH "D:/ogre/ogre/build/sdk/include/Ogre")
	set(OGRE_LIB "D:/ogre/ogre/build/sdk/lib/release/OgreMain.lib")
	set(OGRE_LIB_DBG "D:/ogre/ogre/build/sdk/lib/debug/OgreMain_d.lib")

**cegui-path/cmake/FindOIS.cmake** 파일에 다음의 코드를 추가한다. FindOgre.cmake 에서 했던것과 내용은 동일하다. ::

	set(OIS_H_PATH "D:/ogre/ogre/build/sdk/include/OIS")
	set(OIS_LIB "D:/ogre/ogre/build/sdk/lib/release/OIS.lib")
	set(OIS_LIB_DBG "D:/ogre/ogre/build/sdk/lib/debug/OIS_d.lib")

cmake gui 를 돌린다. 일단 경로를 아래와 같이 설정하고 Configure를 돌린다. cegui-deps를 cmake돌릴때는 build라는 폴더를 따로 만들었지만 cegui는 그렇게 하면 망한다. cegui를 cmake돌리면 header 파일이 생성되는데 build폴더를 따로 만들면 경로잡기 귀찮다.

* Where is the source code = cegui-path
* Where to build the binaries = cegui-path

돌려서 나온 결과물에서 다음을 확인한다.

* CEGUI_BUILD_RENDERER_OGRE : ogre 로 돌리는데 목표인데 꺼져있으면 삽질
* CEGUI_SAMPLE_USE_OGRE : ogre + cegui 가 제대로 붙었는지 확인할떄는 역시 예제 있는게 좋지 않겠어?

나머지 설정은 취향에 따라서 보고 Configure 다시 돌리고 Generate 를 돌려준다. cegui-path/cegui.sln 이 생성되어있을 것이다.

cegui.sln의 내용을 전부 컴파일한다. 일단 컴파일을 돌리면 아마도 컴파일에러가 난다.

크게 하나는 CEGUI_NEW 를 못찾겠다는 문제인다. 발생위치는 Renderer.cpp:60 이다.
**CEGUI_NEW** 를 **CEGUI_NEW_A0** 으로 바꾸면 제대로 된다. 아무래도 오타낸거 릴리즈한듯(...) ::

	//DefaultResourceProvider* rp = CEGUI_NEW CEGUI::DefaultResourceProvider();
	DefaultResourceProvider* rp = CEGUI_NEW_AO CEGUI::DefaultResourceProvider();

다른거는 **error C2001: newline in constant** 이다. 여러곳에서 발생하는데 아무래도 인코딩문제같다. 그냥 포기하고 관련 라인을 날려버리거나 이상해보이는 문자를 그럴싸한 문자로 고치는 식으로 해결했다(...) ::

	//finalText += reinterpret_cast<const encoded_char*>("❚");
	finalText += reinterpret_cast<const encoded_char*>("-");

	//(encoded_char*)"+ - ? B I W Y f n t....
	(encoded_char*)"EPIC_FAIL"

컴파일이 한번 된다 싶으면 deps떄와 마찬가지로 debug/release 로 돌려준다. **cegui-path/bin** 에 그럴싸해 보이는 exe와 dll이 보이면 성공한거다.

예제 실행
###########################

vs에서 **CEGUISampleFramework-0.8** 를 바로 실행하려고 하면 dll을 못찾겠다고 안돌아간다. dll 찾는 경로를 적절히 설정해주거나 dll을 실행파일이 있는 위치로 복사해야된다.

PATH 환경변수에 다음 경로를 적절히 추가해준다. OgreSDK의 경로는 OgreMain.dll, OgreMain_d.dll 이 존재하는 경로로 알아서 잘 설정한다.

* cegui-deps-path/dependencies/bin
* D:/ogre/ogre/build/sdk/bin/debug
* D:/ogre/ogre/build/sdk/bin/release

ogre engine renderer로 돌리면 사용가능한 렌더러가 아무것도 없다. 어떤 render system dll을 갖다써야할지 몰라서 그런거다. 오그레 설치된곳에서 plugins_d.cfg, plugins_d.cfg, 잡탕 ogre dll을 적절히 복사해서 넣어준다. 다시 돌리면 렌더러를 고를수 있을것이고 고른다음에 돌리면 아마도 잘 돌아갈것이다.


Changelog
####################
* 2013/10/09 : 최초 작성일
* 2013/10/10 : cegui 빌드할 떄 build 폴더 안 만듬
