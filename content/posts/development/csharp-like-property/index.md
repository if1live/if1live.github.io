---
title: C#의 프로퍼티 C++에서 쓰기 
tags: [cpp, property]
slug: charp-like-property
author: if1live
date: 2013-01-25
url: /posts/charp-like-property
---

본문을 읽기전에 아래의 2개의 페이지를 읽어보면 이해에 도움이 될것이다. 사실상 내가 만든 코드는 위의 2개를 내 방식대로 묶은것뿐이다

 * [Property(programming)][1]
 * [C++에서 속성 구현하기][2]
  [1]: http://en.wikipedia.org/wiki/Property_(programming)#C.2B.2B
  [2]: http://www.gpgstudy.com/forum/viewtopic.php?t=3504

## Step 1. C# 느낌의 프로퍼티를 C++에서 왜 쓰고 싶은가?
단계별로 코드를 안드로보내기 위해서 처음에는 간단한것부터 시작하자.
```cpp
class Obj {
public:
	int a;
};

Obj obj;
obj.a = 1;
printf("%d\n", obj.a);
```
위의 코드처럼 클래스에 있는 멤버변수가 public이라면 그냥 .찍는것으로 변수를 얻을수있다. 하지만 개인적으로 public변수의 떡칠을 싫어한다. 첫번쨰 이유는 내가 컴맹이라서 브레이크 포인트를 변수에 거는 방법을 모른다. 두번째는 public으로 설정하면 readonly, const같은 통제를 하는것이 불가능하다.

그래서 멤버변수를 private로 하면 getter/setter를 쓰는것이 귀찮다. 왠지모르게 쓸데없이 코드가 길어지는 느낌이다. 이때 C#의 프로퍼티가 출동하면 좋지만 C++은 언어가 쓰레기라서 편법으로 구현해야한다. 

## Step 2. 간단한 구현
일단 간단한 편법으로 property를 구현해보자

```cpp
class PropertySample {
public:
	class {
	public:
		float &operator=(const float &i) { return value = i; }
		operator float() const { return value; }
		float value;
	} b;
};

int main() {
	PropertySample p1;
	p1.b = 2;
	int a = p1.b;
	return 0;
}
```

모든것의 시작이 되는 코드이다. 연산자오버로딩을 통해서 적절히 멤버변수값을 얻을수있다. 하지만 문제가 있다. 
getter/setter를 위한 함수가 property sample클래스에 있는것이 아니라서 함수가 흩어지게 된다.
또한 위의 클래스 존재하는 int a는 class b의 영역이 아니라서 getter에서 접근이 불가능하다. Step3에서 문제를 해결하자


## Step 3. Getter/Setter 함수 연결하기 
위의 문제를 해결하기 위한 중간과정으로 다음과 같은 방식으로 짤수있다

```cpp
class PropertySample {
public:
  int get_a() {
    return a.value * 2;
  }
  int set_a(const int &a) {
    return a * 2;
  }
public:
  //normal form
  class {
  public:
    int &operator=(const int &i) {
      unsigned int offset = offsetof(PropertySample, a);
      PropertySample *parent = (PropertySample*)((unsigned int)(this) - offset);
      value = parent->set_a(i);
      return value;
    }
    operator int() const {
      unsigned int offset = offsetof(PropertySample, a);
      PropertySample *parent = (PropertySample*)((unsigned int)(this) - offset);
      return parent->get_a();
    }
    int value;
  } a;
  int c;
};
int main() {
  PropertySample p1;
  p1.a = 1;   //set하면 2배가 저장
  return p1.a;   //get할때 다시 2배
}
```
offsetof를 적절히 이용해서 getter/setter 함수를 외부로 분리했다. 또한 getter/setter함수를 클래스로 둠으로써 클래스 멤버변수 int c에도 접근이 가능할수있다. 근데 저걸 일일이 치는것은 귀찮다. 적절한 수준으로 매크로와 템플릿의 도움을 받으면 참고자료2번째의 소스코드가 나온다. 하지만 그것과 똑같으면 재미 없잖아? 해서 생성자를 조금 고치니 템플릿의 한계로 인해 매크로로 템플릿을 찍어내고 그것으로 멤버변수를 찍어내는 괴상한 코드가 나왔다. 어쨋든 소스 올림

## Source
```cpp
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstddef>

#define SR_PROPERTY(TYPE, CLASS_NAME, GETTER, SETTER, NAME)  \
  template< \
  typename T, \
  typename ConType, \
  T (ConType::*GetFunc)(),  \
  T (ConType::*SetFunc)(const T &) \
  > \
class Property_##CLASS_NAME##_##NAME {  \
public: \
  Property_##CLASS_NAME##_##NAME() {} \
  Property_##CLASS_NAME##_##NAME(const T i) : value(i) {} \
  T &operator=(const T &i) {  \
    unsigned long offset = offsetof(CLASS_NAME, NAME); \
    ConType *parent = (ConType*)((unsigned long)(this) - offset);  \
    value = (parent->*SetFunc)(i);  \
    return value; \
  } \
  operator T() const {  \
    unsigned long offset = offsetof(CLASS_NAME, NAME); \
    ConType *parent = (ConType*)((unsigned long)(this) - offset);  \
    T result = (parent->*GetFunc)(); \
    return result;  \
  } \
  T value;  \
};  \
  Property_##CLASS_NAME##_##NAME <TYPE, CLASS_NAME, &CLASS_NAME::GETTER, &CLASS_NAME::SETTER> NAME
  

class PropertySample {
public:
  PropertySample() : c(0) {}

  int get_c() {
    int result = c.value * 3;
    return result;
  }
  int set_c(const int &c) {
    return c * 3;
  }
  
  std::string get_str() {
    return std::string("a");
  }
  std::string set_str(const std::string &str) {
    return str + "a";
  }

  SR_PROPERTY(int, PropertySample, get_c, set_c, c);
  SR_PROPERTY(std::string, PropertySample, get_str, set_str, d);
};

int main() {
  PropertySample p1;
  
  printf("default value. %d\n", p1.c);
  p1.c = 1;
  std::cout << "use cout. " << p1.c << std::endl;
  printf("use printf. %d\n", p1.c);

  
  p1.d = "msg";
  std::string tmp = p1.d;
  printf("%s\n", tmp.c_str());
  getchar();
}
```

우왕 이제 위쪽 코드가 더럽긴 하지만 프로퍼티를 찍어낼수있어요! 하고 좋아하기에는 이르다. 출력결과를 보자.
```cpp
default value. 0
use cout. 9
use printf. 3
```

use cout. 9, use printf. 3은 같은 값이 나오길 기대했으나 완전히 다른값이 나왔다. 이는 printf로 넘어가는것이 객체이지 int가 아니기때문이다(이는 위에서 지적하지 않았지만 모든 예제코드에서 공통적으로 발생한다) 

> ( format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘PropertySample::Property_PropertySample_c<int, PropertySample, &PropertySample::get_c, &PropertySample::set_c>’)

이부분을 주의하면 제한된 용도로 쓸수있을거같다

## 최종 코드의 권장되는 용도
pubilc member variable로 막 작업을 하고있었는데 함수를 거쳐서 추가작업을 수행해야되는경우, public멤버변수 때려박은곳을 전부 getter로 바꾸는건 미친짓이니까 그럴때는 이거라도 쓰면 좋을듯

## 결론 
**C++은 쓰레기**
