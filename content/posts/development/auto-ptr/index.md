---
title: auto_ptr을 썻는데 소멸자가 불리지 않는다?
tags: [cpp]
slug: auto-ptr
author: if1live
date: 2013-01-25
url: /posts/auto-ptr
---
auto_ptr은 아주 간단한 스마트 포인터의 일정으로 동적할당된 객체를 자동으로 delete하는데 쓰인다. 하지만 잘못된 코딩을 하면 소멸자가 호출되지 않는다. 소멸자를 호출시키지 않는 auto_ptr의 예제이다

## 문제의 소스
### main.cpp

```cpp
// Ŭnicode please
#include <memory>
#include <cstdio>
#include "example.h"

using namespace std;

class SampleObj {
public:
  ~SampleObj() { printf("dtor\n"); }
};

int main() {
  Example *ex = new Example(new SampleObj());
  delete(ex);
  getchar();
  return 0;
}
```

### example.h

```cpp
// Ŭnicode please
#pragma once
#include <memory>

class SampleObj;
class Example {
public:
  Example(SampleObj *obj);
  ~Example();
  std::auto_ptr<SampleObj> ptr_obj;
};
```

### example.cpp

```cpp
// Ŭnicode please
#include "example.h"
#include <cstdio>

Example::Example(SampleObj *obj)
  : ptr_obj(obj) {}

Example::~Example() {
  printf("example dtor called\n");
}
```

## 실행 결과
```cpp
example dtor called
```

컴파일은 되는데(워닝, C4150 으로 하나 뜨고) 실행결과 auto_ptr에 들어잇는 객체의 소멸자가 호출되지 않앗다. 원인은 auto_ptr이 들어잇는 Example 클래스의 소멸자(example.cpp)에서는 SampleObject를 알고잇지 않기 때문이다. 해결책으로는 example.cpp에 헤더를 인클루드한다는등의 식으로 SampleObject를 찾을수 잇도록 하면 된다. 이 문제는 소멸자에 대해서 발생하는것인데 내 경우 내 코딩스타일과 조합되면서 auto_ptr쓸떄 많이 보더라

## 문제 발생 예제 상황
- 컴파일시간 아끼겟다고 전방선언을 떡칠하다가 c++에서 까먹고 실제 클래스를 인클루드 안한 경우
- 클래스 헤더쪽에서 코드 몇줄 덜 치겟다고 auto_ptr.reset(obj) 를 사용햇는데 헤더가 실제 클래스를 인클루드 하지 않은 경우

## Reference
- [C4150](http://msdn.microsoft.com/ko-kr/library/ba5dy3f2.aspx)
- [c auto ptt forward decleration](http://stackoverflow.com/questions/1951933/c-auto-ptr-forward-decleration)
