Title: 완벽한 싱글턴을 찾아서 - 테스트
Tags: singleton, unittest, test, testing
Slug: singleton-test
Author: if1live
Date: 2013-06-18


[이전에 사람들이 많이 사용하는 방식의 싱글턴을 까는 글을 쓰고 그것을 해결책을 정리했었다]({filename}i-hate-singleton.md).(사실은 복붙에 더 가깝지만...) 최근에 조금 더 생각해보니 그거보다 더 좋은 방법(특히 **테스트**측면에서)이 생각나서 똑같은 주제를 재탕하기로 했다.

## 테스트에서 싱글턴의 뭐가 문제인가?

전역변수 기반의 싱글턴이 사고치는 경우를 준비하자.

> ```FileSystem```이라는 싱글턴 클래스가 존재한다.
> 싱글턴으로 사용하는 전역변수의 이름은 ```gFileSystem```이다.
> 그리고 ```gFileSystem```을 기반으로 구현된 ```PackageManager```라는 클래스가 있다.
> ```PackageManager```에는 ```foo()```, ```bar()```라는 함수가 존재한다.

위의 상황에 대응하는 유닛테스트를 다음과 같이 작성했다.

```cpp
class PackageManagerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gFileSystem.setUp();
	}
    virtual void TearDown() {
        gFileSystem.shutDown();
    }
};

TEST_F(PackageManagerTest, foo)
{
    PackageManager mgr;
    mgr.foo();
}
TEST_F(PackageManagerTest, bar)
{
    PackageManager mgr;
    mgr.bar();
}
```
위의 유닛테스트는 대충 보기에는 그럴듯해보인다. 하지만 유닛테스트로 쓰기에는 치명적인 문제가 2가지 존재한다.

### 싱글턴 객체를 공유한다니!
```gFileSystem``` 전역변수는 foo, bar 유닛테스트 함수에서 공유한다.
```gFileSystem.setUp()``` 또는 ```gFileSystem.shutDown()```의 구현에 문제가 있어서 ```gFileSystem```이 **깨끗한 상태**로 유지되지 않으면, foo->bar 순서로 테스트를 실행할 때와 bar->foo 순서로 테스트를 실행할 때 다른 결과를 보여줄 수 있다. 즉, foo와 bar가 완전히 독립된 테스트라고 부를 수 없다.

### MockObject 사용 불가능
```gFileSystem```는 컴파일 시간에 정해지는 전역변수이다. 런타임 시에 MockObject 같은거로 바꿔끼우는건 안된다. 그래서 ```FileSystem```이 제대로 구현되기 전에는 ```PackageManager```를 테스트하는 것이 불가능하다.

## 해결책

Doom3에서는 싱글턴을 다음과 같은 형태로 사용한다. (존카멕 아저씨가 C 시대의 사람이라서 이런 흔적이 남아있는걸까...?)

### ModelManager.h
```cpp
// this will be statically pointed at a private implementation
extern  idRenderModelManager    *renderModelManager;
```

### ModelManager.cpp
```cpp
idRenderModelManagerLocal   localModelManager;
idRenderModelManager *      renderModelManager = &localModelManager;
```

싱글턴을 그냥 객체로 코드에 때려박지 않고 **포인터**를 사용하는것이다.
이렇게 바꾸면 런타임에 싱글턴에 해당하는 것을 바꿀 수 있다.
추가로 인터페이스를 도입하면 MockObject를 만드는 것도 가능하다.
아래는 위의 개념을 도입한 유닛테스트 코드이다.

```cpp
class PackageManagerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
		prevFS = gFileSystem;
        // 싱글턴을 테스트마다 새로 만들어서 테스트할때 마다
        // 새로운 싱글턴 객체를 사용할 수 있다
        gFileSystem = new FileSystem();
		gFileSystem->setUp();

	}
    virtual void TearDown() {
        gFileSystem->shutDown();
		delete(gFileSystem);
		gFileSystem = prevFS;
    }
	FileSystem *prevFS;
};

TEST_F(PackageManagerTest, foo)
{
    PackageManager mgr;
    mgr.foo();
}
TEST_F(PackageManagerTest, bar)
{
    PackageManager mgr;
    mgr.bar();
}
```
