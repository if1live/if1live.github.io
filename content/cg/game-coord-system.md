Title: 게임 좌표계에 대한 고찰
Tags: axis, gamedev
Slug: game-coord-system
Author: if1live
Date: 2013-01-25

## 좌표계에 대한 고찰

3D 그래픽스 좌표계에는 크게 2가지(왼손 좌표계/오른손 좌표계)가 있다.
왼손/오른손 좌표계 모두 왼쪽..오른쪽 방향을 +x, 아래..위쪽 방향을 +y로 쓰지만 왼손 좌표계는 화면속으로 들어가는 방향이 +z이고 오른손 좌표계는 화면에서 나오는 방향이 +z이다.
directx는 왼손 좌표계를 사용하고 opengl은 오른손 좌표계를 사용한다. 여기까지는 그래픽스에 대해서 잠깐이라도 공부해본 사람들은 다들 알만한 내용이다.

![Alt Left/Right Axis]({filename}../static/game-coord-system/leftrght.gif)

그래픽스 좌표계의 특징을 보면 세로방향을 y축으로 사용하고 카메라에서 가까워지는/멀어지는 방향을 z축으로 사용한다. (가로 방향을 x축을 사용하는것은 어지간한 다른 좌표계에서도 마찬가지니까 일단 건너뛴다) 그래픽스 좌표계는 최종적으로 화면(모니터)에 대응되어야하기 때문에 모니터 화면에 대응되는 평면을 xy평면으로 만든것으로 보인다. 아래..위로 올라가는 방향을 +Y방향으로 정의한것은 수학에서 일반적으로 위쪽을 +방향으로 쓰니까 그대로 갖고온거같다. 그리고 남은 z축은 깊이를 표현햇다(그래서 깊이버퍼와 z버퍼는 동일한 의미)

하지만 게임에서의 좌표계는 어떨까? 게임에서는 맵이 존재할거다. 대부분의 게임은 캐릭터가 땅을 밟고 서있으니까 지도의 컨셉을 그냥 갖다쓰는게 편할것이다.

![Alt map]({filename}../static/game-coord-system/Census_Bureau_map_of_New_Brunswick,_New_Jersey.gif)

지도에 축을 부여한다면 아마도 대부분의 사람들이 가로를 x축이라고 하고 세로방향이 y축이라고 부를것이다. 그리고 높이(등고선)를 z축이라고 부를것이다. 이를 시각화하면 다음과 같다.

![Alt Half-Life coordination system]({filename}../static/game-coord-system/Half-Life_coordinate_system.jpg)

그래픽스 좌표계에서의 y/z축의 역할이 서로 바뀌어있다. x축을 기준으로 90도 회전되어있다! 만약 그래픽스 좌표계를 그대로 게임 좌표계로 가져와서 객체를 배치했다면 지도에 물체를 배치한다는 느낌으로 점을 찍기가 힘들었을것이다.


그래픽API에서 사용되는 그래픽스 좌표계와 게임 내부에서 사용되는 게임 좌표계를 다르게 할 필요가 없다라는 생각을 물론 내가 처음 생각해냈을리는 없다. 이미 둠/퀘이크에서 쓰인 idTech 엔진에서 그렇게 쓰고있다. idTech엔진의 경우 xy로 지도를 구성하고 z로 높이를 표현했다.(그리고 idTech엔진을 계승한 하프라이프, 콜옵등도 게임내에서는 동일한 좌표계를 사용할것이다) idTech 엔진의 경우는 울펜슈타인 시절부터 이어져 내려오던 거니까 그것의 영향도 있을것이다. 게임 자체는 아래처럼 3D처럼 보이지만

![Alt Wolfenstein 3D Screenshot]({filename}../static/game-coord-system/wolfenstein4.gif)

실제로 맵을 찍는건 아래처럼 평면 지도를 구성하는거니까 축을 x,y로 표현했겟지..

![Alt Wolf3d Map Editor]({filename}../static/game-coord-system/Wolf3d_episode6_level3.png)

그리고 퀘이크용 맵에디터인 gtkradiant의 스크린샷을 보면 높이(z축)보다 평면(xy) 구성이 핵심이라는걸 알수있다.

![Alt GtkRadiant]({filename}../static/game-coord-system/screenshot_sm.jpg)


그렇다고해서 "그래픽스 좌표계와 달리 게임 좌표계는 땅바닥 평면을 xy평면으로 구성해야한다" 라는건 아니다. 유니티와 같이 엔진툴기능 자체가 빵빵해서 수동으로 좌표값을 찍을 필요가 없다면 그럴 필요는 없는듯하다. (유니티는 그래픽스 좌표계중 왼손 좌표계를 그냥 게임 좌표계로 사용한다)

## 결론
어디까지나 나의 주관이다. 알아서 생각하자
  * 엔진툴이 좋아서 수동으로 좌표찍을 일이 없으면 좌표축은 엔진에서 사용하는 그래픽스 라이브러리(dx/gl/etc)의 좌표축을 그냥 쓰자.
  * 엔진툴이 후져서(혹은 없으으면) 수동으로 좌표찍을 일이 생기면 xy로 평면으로 정의하고 z를 등고선(높이)로 쓰자.
  * 역시 엔진툴 좋은 유니티가 좋은 엔진이다.

## Reference
  * [Microsoft DirectX 9.0 3D 좌표계](http://telnet.or.kr/directx/graphics/programmingguide/gettingstarted/3dcoordinatesystems/coordinatesystems.htm)
  * [Half-Life coordinate system](http://www.hhg-clan.de/hlae/wiki/index.php?title=Half-Life_coordinate_system)
  * [FABIEN SANGLARD'S WEBSITE](http://fabiensanglard.net/quake2/)
