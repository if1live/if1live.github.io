---
title: C++ 다중상속(interface) 관련 삽질 자료
tags: [cpp, interface, casting]
slug: cpp-casting
author: if1live
summary: C++ 다중상속(interface) 관련 삽질 자료
date: 2012-11-27
url: /posts/cpp-casting
---

나는 다중상속을 쓰는것을 싫어한다. 왜냐하면 다중상속을 써서 구현했다는것은 그 소스를 자바나 C#으로 똑같이 구현하는것이 불가능하다는 뜻이기 떄문이다. 
하지만 자바나 C#처럼 interface라는것은 설계를하다보면 자주 쓰게되는데 C++의 경우는 interface같은것이 문법차원으로 지원되지 않는다. 순수가상함수로만 채워진 함수를 상속받으면 그것이 interface처럼 동작하는것뿐이다. 그래서 실제 구현결과는 다중상속이나 마찬가지다
이로인해 발생할듯한 문제를 테스트 해봤다

실험에 사용된 클래스 소스는..
```cpp
class Super1 {
public:
    virtual int a() = 0;
};
class Super2 {
public:
    virtual int b() = 0;
};
class Target : public Super1, public Super2 {
public:
    virtual int a() { return 1; }
    virtual int b() { return 2; }
};
```

Target클래스가 Super1, Super2의 순수가상함수를 적절히 구현했다.

자바나 c#에서 2개의 인터페이스를 구현한 객체를 캐스팅하는 느낌을 C++로 짜봤다
```cpp
Target *target = new Target();
Super1 *s1 = (Super1*)target;
Super2 *s2 = (Super2*)s1;
EXPECT_EQ(2, s2->b());
```
target객체를 만들어서 일단은 Super1타입으로 캐스팅한다. 그리고 적절한 작업을 수행했다고치고 super1타입의 포인터를 그대로 super2타입으로 캐스팅했다. 그리고 Super2에 있는 b를 호출한다. 이것이 C#코드였으면 2가 나왔겠지만 테스트 결과 1이 나왔다. 이것이 다중상속한것의 포인터를 생각없이 갖고논작업의 최후이다. 더이상의 자세한 설명은 생략한다.

그렇다면 어떻게 해야 정상동작하는가? Super1->Target->Super2로 캐스팅해야된다
```cpp
Target *target = new Target();
Super1 *s1 = (Super1*)target;
Super2 *s2 = (Super2*)((Target*)s1);
EXPECT_EQ(2, s2->b());
```

근데 이짓을 수동으로 하다보면 Target으로의 캐스팅을 빼먹는 실수할 가능성이 높아진다. C++에 추가된 캐스팅방법 중 하나인 static_cast를 사용하면 컴파일러가 적절히 테스트해준다. static_cast는 알아서 공부하면 된다
```cpp
Target *target = new Target();
Super1 *s1 = static_cast<Super1*>(target);
Super2 *s2 = static_cast<Super2*>(s1);
EXPECT_EQ(2, s2->b());
```
위의 소스를 컴파일하면 static_cast에 의해서 컴파일러가 ```static_cast<Super2*>(s1)```하는것이 불가능하다는것을 컴파일타임에 알려준다. 그래서 니가 에러를 알아듣고 아래와 같은 형식으로 바꿀수 있는 가능성이 높아진다

```cpp
Target *target = new Target();
Super1 *s1 = static_cast<Super1*>(target);
Super2 *s2 = static_cast<Super2*>(static_cast<Target*>(s1));
EXPECT_EQ(2, s2->b());
```

아무리 생각해도 Super1->Target->Super2로 캐스팅을 2번하는짓은 미친짓같다. 뻔히 보이는데 왜 저짓을 해야되는가? C#, 자바처럼 런타임에 적절히 알아서 캐스팅됐으면 좋겠다싶으면 dynamic_cast를 사용하면 된다.
```cpp
Target *target = new Target();
Super1 *s1 = dynamic_cast<Super1*>(target);
Super2 *s2 = dynamic_cast<Super2*>(s1);
EXPECT_EQ(2, s2->b());
```

static_cast, dynamic_cast를 설명할수 있는 적절한 방법이 생각났으나 여백이 부족해 적지 않는다. (나중에 다른 페이지에 정리해야지)
라고 이 글을 쓴건 12년 7월. 결국 귀찮아서 안썼다.
