---
title: 셰이더 프로그래밍 입문 for GLSL
tags: [glsl, cg, shader]
slug: glsl-example
author: if1live
date: 2013-04-27
url: /posts/glsl-example
---
포프님의 **[셰이더 프로그래밍 입문][pope_book]**을 읽으면서 이거 GLSL로 구현하면서 공부해보자 하면서 만든게 있는데 귀찮아서 방치해놧다가 지금서야 정리했다. 책은 DX 기준으로 작성되어 있지만 GL과 DX는 수학상으로도(또한 구현상으로도) 다른게 많아서 좀 고쳤다.

그리고 오늘 프로젝트를 분리시켰다!

* [Repo](https://github.com/if1live/glsl-example)

## Library
OpenGL은 DirectX와는 달리 그 자체로는 뭐 어떻게 써먹을 방법이 없다. 수학라이브러리도 내장되어있지 않고 텍스쳐 로딩도 없고 창초기화도 없고 등등....되는게 하나도 없어서 라이브러리를 적절히 붙여야 돌릴 수 있다. 구현에 사용한 라이브러리/참고코드 목록이다. (아마도 다음부터 GL예제를 올리때는 귀찮아서 안쓰겟지만 한번은 쓰는게 낫겟지...)

* [GLFW][glfw] : OpenGL 창을 생성하기위해 사용

* [GLEW][glew] : Microsoft는 OpenGL 1.1 시절에 GL진영에서 뛰쳐나와서 DX를 만들다.
덕분에 라이브러리를 쓰지 않으면 윈도우에서 OpenGL 1.1 이후의 기능을 쓰는게 매우 귀찮다
(불가능한것은 아니다. [ShadowMapping with GLSL][fabiensanglard]를 참고하면 glew없이 최신버전 GL을 쓰는것이 나와있다)
이 라이브러리를 쓰면 최신 GL함수를 윈도우에서 쉽게 사용할수 있다.

* [OpenGL Mathematics][glm] : 쓰기 쉬운 적절한 수학라이브러리.
Header-only library라서 링크 신경안쓰고 사용할 수 있어서 자주 쓴다

* [LodePNG][lodepng] : 샘플 텍스쳐로 PNG를 선택하고 사용한 PNG 로딩 라이브러리이다.
개인적으로는 주로 다양한 파일포맷을 지원하는 [Simple OpenGL Image Library][soil]를 사용하는데 LodePNG는 lodepng.h/lodepng.cpp 만으로 구성되는게 간단해서 한번 써봣다.
나름 좋은듯?

덤으로 테스트용 Mesh를 생성하기 위해서 [iPhone 3D Programming][iphone3d]에서 예제 코드를 갖다 썻다.
(진짜 mesh loader까지 붙이면 왠지 예제스럽지가 않잖아?)

## Reference
* [셰이더 프로그래밍 입문][pope_book]
* [ShadowMapping with GLSL][fabiensanglard]

[pope_book]: http://www.hanb.co.kr/book/look.html?isbn=978-89-7914-949-4
[fabiensanglard]: http://fabiensanglard.net/shadowmapping/index.php
[release]: /static/static/glsl-example/glsl-example.zip
[iphone3d]: http://ofps.oreilly.com/titles/9780596804824/
[glew]: http://glew.sourceforge.net/
[glfw]: http://www.glfw.org/
[glm]: http://glm.g-truc.net
[lodepng]: http://lodev.org/lodepng/
[soil]: http://www.lonesock.net/soil.html
