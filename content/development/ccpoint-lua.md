Title: cocos2d-x 1.x->2.0으로 변하면서 바뀐 부분 :: CCPoint
Tags: cpp, cocos2dx
Slug: ccpoint-lua
Author: if1live
Summary: cocos2d-x 1.x->2.0으로 변하면서 바뀐 부분 :: CCPoint
Date: 2012-12-04

나는 cocos2d-x를 1.X에 쓰기 시작했다. 최근에 다시 게임을 만들면서 cocos2d-x 2.0으로 옮겨갔다. 1.x->2.0으로 버전이 올라가면서 대부분의 영역에서 설계 측면에서 약간씩 설계적으로 개선된것이 보인다. 하지만 일부의 경우는 개선이라고 하기에는 너무 달라져서 새로 짯다 라는 느낌이다. 그 중의 하나가 CCGeomerty.h에 정의된 CCPoint 클래스이다. 

cocos2d-x 1.x에서 사용하는 CCPoint는 다음과 같이 생겼다. 구시대 버전을 찾아서 CCGeomerty.h를 찾으면 나온다.

```cpp
class CC_DLL CCPoint
{
public:
	float x;
	float y;
.......
};
```

cocos2d-x 2.0에서 사용하는 CCPoint는 다음과 같이 생겼다.
```cpp
class CC_DLL CCPoint : public CCObject
{
public:
    float x;
    float y;
    ......
```

2D게임에서의 점을 가리키는 클래스는 int, float과 유사한 기본자료형이라고 볼수있다. 그런데 기존에는 없던 **상속**을 추가해서 의존성을 늘리고 새로운 멤버변수가 붙으면서 클래스의 크기가 커진다고? (CCObject를 상속받았으니까 CCObject의 멤버변수가 CCPoint에 달라붙는다. sizeof로 찍어보면 클래스는 크게 나온다) 게다가 기존에는 소멸자가 기능이 아무것도 없엇는데 이제 CCObject의 소멸자까지 호출된다고? 나로써는 왜 이짓을 했나 이해하기가 힘들었다. 

하지만 cocos2d-x 2.0의 개선방향을 보니까 이해를 할수 있겠더라. 버전이 올라가면서 많이 개선된 부분은 Lua지원이다. 이제 나같은 네이티브 덕후들이나 C++을 쌩으로 써서 게임을 만들지, lua로 어지간한걸 다 처리할수 있게 된거같더라. CCObject 클래스를 보면 다음과 같이 lua지원을 위한 멤버변수가 보인다. 이를 상속받아 CCPoint를 만듬으로써 Lua와의 통합을 노린거같다.
```cpp
class CC_DLL CCObject : public CCCopying
{
public:
    // object id, CCScriptSupport need public m_uID
    unsigned int		m_uID;
    // Lua reference id
    int                 m_nLuaID;
protected:
    ................
```

왜 바꿧는지는 이해가 되지만 나같은 Only C++ 유저한테는 별 이득이 될거같진 않다.
