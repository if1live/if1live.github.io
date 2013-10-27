Title: 나는 싱글턴이 싫어요
Tags: singleton
Slug: i-hate-singleton
Author: if1live
Date: 2013-05-19


옛날에는 싱글턴을 자주 썼는데 요즘에는 안쓴다. 내가 싱글턴을 안쓰게 된 몇가지 이유가 있는데 다음과 같다

## 소멸 시점이 있긴한가?
static을 이용해서 구현한 싱글턴 설계에서 존재하는 문제점이다. 
```cpp
class Singleton {
private:
    Singleton() {}
    Singleton(const Singleton& other);
public:
    static Singleton& getInstance() { 
        static Singleton ctx;
        return ctx; 
    }
};
```

```Singleton::getInstance()```함수를 호출하면 싱글턴을 적절히 사용할수있다. 하지만 소멸은...? 싱글턴의 소멸은 OS님께서 알아서 해주시는거 외에는 기대할 방법이 없다. 싱글턴 A/B/C가 있는데 의존성 문제로 각각이 일정한 순서로 소멸되어야한다면? 위의 설계로는 처리할 방법이 없다. 

## lazy initialization의 함정

lazy initialization을 사용한 싱글턴은 대충 아래같은 느낌이다.

```cpp
template<typename T>
class Singleton {
public:
    static T& GetInstance() {
        if (ctx_ == NULL) { 
            ctx_ = new T();
        }
        return *ctx_;
    }

}
```
싱글턴이 필요한곳에서 ```SomeSingleton::GetInstance()```를 호출하면 싱글턴을 적절히 사용할 수 있습니다.... 로 끝날리가있나! 아래의 2개의 조건이 조합된 상황에서는 이 싱글턴은 문제가 된다.

1. lazy initialization는 실제로 객체가 필요해지기 전까지는 객체를 만들지 않는다. 그래서 자원을 아낄수있습니다
2. 싱글턴에 크고 아름다워서 초기화에 3초가 걸린다.
3. 게임과 같은 실시간성이 중요한 프로그램이다.

위의 내용이 조합되서 어떤 문제를 발생시키는가? 게임 로딩시간으로 3초가 추가되는건 심각한 문제는 아니다. 하지만 게임중에 싱글턴 객체가 필요해져서 ```SomeSingleton::GetInstance()```함수를 호출하고 3초간 게임이 멈추면 이건 심각한 문제다. lazy initialization만 믿고 언제 초기화를 할지에 대해서는 신경쓰지 않는다는건 말이 안된다. 

또한 싱글턴이 여러개 존재할 경우, 각각의 싱글턴이 어떤 순서로 초기화되는지 코드를 전부 까보지 않는 이상 알아낼 방법이 없다. ```SomeSingleton::GetInstance()``` 함수를 전부 찾아낸다고 하더라도, GetInstance() 함수가 객체 생성/그냥 반환하는 기능을 동시에 수행하니까 코드를 보는것만으로는 초기화 순서를 파악하기 매우 어렵다.

## 단위 테스트 불가능
단위테스트를 할 경우, 각각의 테스트 함수는 다른 테스트 함수와는 독립적이어야한다. test_foo(), test_bar() 라는 단위테스트 함수가 있는데 test_foo()의 성공/실패 여부가 test_bar()의 결과에는 영향을 주면 안된다. 근데 싱글턴은 프로그램안에 단 1개 존재하는 물건이다. 그렇다면 싱글턴과 단위테스트를 조합하면 무엇이 문제인가?

단위테스트의 SetUp/TearDown 함수에 Singleton의 생성/소멸 함수를 일일이 넣어줘야한다. 왜냐하면 싱글턴은 시스템상에 1개만 존재하는 객체이기 떄문에 테스트 함수 안에서 그냥 객체 생성하듯이 만드는게 불가능하기 떄문이다. 

게다가 테스트함수 안에서 호출되는 실제 기능의 함수 중 일부가 ```OtherSingleton::GetInstance()```를 호출해버리면 나는 A싱글턴을 테스트하고 있는데 뜬금없이 B싱글턴이 생성되어버려서 이후의 테스트에 어떤영향을 끼칠지 알 수 없다. 내 경우에 이를 다음과 같이 싱글턴을 일반 객체로 만들고 싱글턴과 관련된 내용을 미리 초기화해서 유닛테스트를 수행했다.

```cpp
class DebugDrawManagerTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        FS_Init();
        RenderState_Init(640, 480);
    }
    virtual void TearDown()
    {
        RenderState_Deinit();
        FS_Deinit();
    }
};

TEST_F(DebugDrawManagerTest, Update) 
{
    // 실제 싱글턴으로 사용할 객체. 평범한 객체처럼 지역변수로 생성가능
    DebugDrawManager mgr;
    EXPECT_EQ(0, mgr.CmdCount());

    mgr.AddAxis(mat4(1.0f), 1);            // without duration
    mgr.AddAxis(mat4(1.0f), 1, 1.0f);    // with duration
    EXPECT_EQ(2, mgr.CmdCount());

    mgr.Update(0.5f);
    EXPECT_EQ(1, mgr.CmdCount());

    mgr.Update(1.0f);
    EXPECT_EQ(0, mgr.CmdCount());

    mgr.Update(1.0f);
    EXPECT_EQ(0, mgr.CmdCount());
}
```

## 결론
"Game Engine Architecture 5.1.2 A Simple Approach That Works" 에서는 다음과 같은 방법을 제시한다. 

* singleton의 용도로 사용할 객체를 전역변수로 만든다. 
* 싱글턴 목적의 클래스에 초기화/소멸용 함수를 만든다
* 싱글턴 초기화/소멸용 함수를 수동으로 호출한다

```cpp

// step 1. 싱글턴으로 사용할 전역변수
RenderManager gRenderManager;
VideoManager gVideoManager;
TextureManager gTextureManager;

int main()
{
    // step 2. 수동 초기화. 싱글턴간의 의존성을 고려해서 적절한 순서로 호출
    gVideoManager.startUp();
    gTextureManager.startUp();
    gRenderManager.startUp();

    // step 3. 적절히 구현하기. 싱글턴 갖다써도 문제없다
    runLogin();

    // step 4. 수동 소멸. 초기화와 반대 순서로 쓰기
    gRenderManager.shutDown();
    gTextureManager.shutDown();
    gVideoManager.shutDown();
}
```


## Reference
* Game Engine Architecture 5.1. Subsystem Start-Up and Shut-Down
* [Singleton Pattern](http://en.wikipedia.org/wiki/Singleton_pattern)