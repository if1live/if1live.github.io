Title: C++ namespace 들여쓰기 방지하는 편법
Tags: cpp, namespace, indent
Slug: namespace-indent
Author: if1live
Summary: C++ namespace 들여쓰기 방지하는 편법
Date: 2012-11-28

## 개요
C++에는 C에는 없는 namespace라는 문법이 있다.(C가 구식이라서 없는거지 요즘 언어는 어지간하면 다 namespace와 유사한 문법이 있다.) 네임스페이스는 문법이 블럭과 동일하기때문에 내부 영역이 적절히 들여쓰기가 된다

```cpp
namespace sora {
    class SomeClass { }; 
}
```

그런데 네임스페이스의 깊이가 항상 1 level일수는 없잖아? 더 깊어질수도 있지 않겠는가? 그렇게 되면 코드가 다음과 같은 꼴이 된다. 

```cpp
namespace sora {
    namespace gl {
        class SomeClass {
            // TODO 적절히 구현하기
        }; 
    }
}
```

고작 네임스페이스 좀 그럴듯하게 보겟다고 코드의 왼쪽공간이 죄다 공백으로 도배되는 참사가 발생한다! 이를 해결하겟다고 irrlicht engine의 경우는 아래처럼 코딩했다. 
```cpp
namespace irr
{
namespace scene
{
    class ISceneManager;

    typedef core::list<ISceneNode*> ISceneNodeList;
    typedef core::list<ISceneNodeAnimator*> ISceneNodeAnimatorList;
    //....
```
일부러 2번쨰 레벨의 네임스페이스를 들여쓰기 없이 사용했다. 하지만 이 방법은 visual studio랑 조합되면 별로 좋은 방법이 아니다. visual studio에서 제공하는 코드 포맷팅을 거치면 어김없이 밀려버린다.
```cpp
namespace irr
{
    namespace scene
    {
        class ISceneManager;

        typedef core::list<ISceneNode*> ISceneNodeList;
        typedef core::list<ISceneNodeAnimator*> ISceneNodeAnimatorList;
        //....
```

들여쓰기 없는 다음과 같은 네임스페이스를 쓰기 위해서는 어떻게 짜야 하는가?
```cpp
namespace sora {
class SomeClass { }; 
}
```



## ;의 순기능
예전에 이를 막기위한 방법을 찾으면서 [stackover](http://stackoverflow.com/questions/3727862/is-there-any-way-to-make-visual-studio-stop-indenting-namespaces)에서 적절한 편법을 찾았다. namespace를 구성하는 블럭의 {뒤에 ;를 붙여주는 거다

```
namespace sora {;
class SomeClass { };
}
```

다만 이 방법에 한가지 문제가 있다면 visual studio에서만 지원한다. xcode같은거로 저 코드를 그래도 들고가서 자동 포맷팅을 한번 돌려주면 들여쓰기가 개발살 나는걸 볼수있다.

## 매크로
cocos2d-x 에서 사용하는 편법이다.
```
#define NS_CC_BEGIN                     namespace cocos2d {
#define NS_CC_END                       }

NS_CC_BEGIN
class SomeClass { }; 
NS_CC_END
```

매크로를 사용하기 때문에 왠지 코드가 이상해보이지만 멀쩡히 작동한다. 매크로를 해석해서 치환하지 않으면 namespace라는걸 알수 없기 떄문에 자동 포맷팅에 의한 강제 들여쓰기도 피할수 없다. cocos2d-x를 처음쓸때는 왜 저짓을 햇나 한참 고민했엇는데 어제밤에 자다가 깨달은 좋은 편법이다

## 내가 채택한 방법
기본적으로는 cocos2d-x와 같이 매크로를 쓰는 기법을 채택했다. namespace 문법에 유사하게 만들기 위해서 매크로 함수같은 느낌으로 만들었다
```cpp
#define SR_NS_BEGIN(NS)     namespace NS {
#define SR_NS_END(NS)       }

SR_NS_BEGIN(sora)
SR_NS_END(sora)

```
 
## 추가 잡소리
namespace에 들여쓰기가 없어짐으로써 코드의 끝부분이 읽기 어려워질수도 있다. 함수와 네임스페이스의 끝이 같이 섞여서 }가 계속 나오기 때문이다.
```cpp
namespace sora {;
void print() {
    //TODO PROFIT!?
}
}
```

이를 방지하기 위해서 네임스페이스의 끝에 주석을 달아서 끝을 명시적으로 표현하기도 한다. [google-coding-style](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml#Namespace_Names)
```cpp
namespace sora {;
void print() {
    //TODO PROFIT!?
}
} // namespace sora
```
