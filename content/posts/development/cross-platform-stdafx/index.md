---
title: 윈도/ios/안드로이드를 커버할수 있는 PCH 구조 잡기
tags: [cpp, pch]
slug: cross-platform-stdafx
author: if1live
summary: 윈도/ios/안드로이드를 커버할수 있는 PCH 구조 잡기
date: 2012-12-08
url: /posts/cross-platform-stdafx
---

미리 컴파일된 헤더(PreCompiled Header, 이하 PCH라고 지칭한다)는 컴파일시간을 줄이는데 적절한 기능이다. (PCH가 무엇인지에 대한 설명은 지금 관심없기 때문에 일단은 건너뛴다. 알아서 검색하자. 어쨋든 좋은거다) 다만 PCH라는 개념은 대부분의 컴파일러(그래봣자 내가 쓰는 컴파일러라고 해봐야 msvc, llvm, gcc)에서 지원하긴하지만 표준은 아니다(마치 ```#pragma once```같은 느낌) 덕분에 윈도우/아이폰/안드로이드에서 PCH를 사용하는 방법이 달라서 하나의 소스에서 3가지를 동시에 지원하기 위해서는 약간의 편법을 부려야한다. 일단 각각의 플랫폼에서의 PCH를 사용하기 위한 방법/제약은 다음과 같다

 * 윈도우
  * 일반적으로 stdafx.h를 파일명으로 쓴다. (당연히 바꿀수도 있다)
  * 모든 cpp 파일의 주석을 제외한 **첫번째 줄**에는 ```#include "stdafx.h"```가 들어가야한다.
  * stdafx.cpp 파일을 따로 만들어야한다. 프로젝트 자체는 PCH를 사용, stdafx.cpp는 PCH 만들기 라고 설정해야 돌아간다

 * iOS
  * 일반적으로 proj_name_Prefix.pch 를 기본 파일명으로 사용한다. (당연히 바꿀수 있다)
  * cpp파일에서 명시적으로 ```#include "proj_name_Prefix.pch"```를 인클루드 할 필요 없다
  * obj-c 소스와 C++이 같은 PCH를 공유한다 (PCH는 프로젝트마다 1개씩만 존재할수 있다. 아니, 프로젝트보다 타겟이라는 표현이 적절하겠다)
    * PCH파일안에 obj-c문법이 들어가면 c++소스 컴파일할때 에러가 난다. 반대로 PCH파일안에 C++문법이 들어가면 obj-c소스를 컴파일할때 컴파일 에러가 난다.

 * 안드로이드
  * 공식적으로는 PCH를 지원하지 않는다
    * gcc는 PCH를 지원한다. 하지만 안드로이드 NDK에 사용되는 특수한 makefile 문법에는 pch를 정의할 자리가 없다. [외쿡의 삽질사례](http://code.google.com/p/android/issues/detail?id=25412)를 볼때 ndk 자체를 고치면 가능하긴 한듯하다. 시간나면 해봐야지

즉, 윈도/iOS/안드로이드를 동시에 지원하는 pch소스 파일을 만들기 위해서는 pch를 지원하는 컴파일러, 지원하지 않는 컴파일러를 모두 커버할수 있도록 적절히 짜야한다는 소리이다. 사실 pch가 지원되지 않는 컴파일러라고 컴파일이 안되는건 아니다. 다만 pch가 지원되지 않는 컴파일러에서 pch같이 크고 아름다운 헤더를 모든 cpp에 인클루드 하면 컴파일타임이 막장스러워지는 사태가 발생하기 때문에 이를 방지하기 위해서 코드를 적절히 처리해줘야한다. (해당 사항은 GPG 4. 커다란 크로스플랫폼 라이브러리의 설계와 유지 에서도 다루는 내용이다)

내가 프로젝트에 사용하는 코드의 구조는 다음과 같은 느낌이다.

일단 msvc를 위해서 stdafx.cpp파일을 다음과 같이 만든다.
```cpp
#include "stdafx.h"
```

PCH를 지원하는 플랫폼/지원하지 않는 플랫폼에 대한 처리를 하기 위해서 다음과 같은 arch.h라는 파일을 미리 만든다. 소스의 내용은 [플랫폼 매크로 선언 없이 윈도/iOS/안드로이드 구분하는 매크로]({{< ref platform-macro >}})에서 이어진다
```cpp
// 적절히 현재 플랫폼에 따라서 SR_WIN, SR_IPHONE, SR_ANDROID가 0 or 1으로 설정된다
//pch
#if SR_WIN || SR_IPHONE
#define SR_USE_PCH 1
#else
#define SR_USE_PCH 0
#endif
```

진짜 핵심인 PCH의 내용은 다음과 같다.
```cpp
// Ŭnicode please
// pch를 지원하지 않는 환경에서도 인클루드한게 통쨰로 인클루드 하는걸 방지하기위해서 이런식으로 처리했다
#include "arch.h"

#ifdef __cplusplus

#include "assert_include.h"   // assert는 거의 전역에서 쓰니까

#if SR_USE_PCH

#include <cstdlib>
#include <cstdio>
#include <cmath>
// 적절히 잡탕 c++라이브러리

#endif  // SR_USE_PCH

#endif  // __cplusplus

#ifdef __OBJC__
#import <Foundation/Foundation.h>
// 적절히 잡탕 obj-c 라이브러리
#endif
```

첫번째 줄에서 arch.h를 인클루드함으로써 해당 플랫폼이 PCH를 지원할수 있는지, 아니면 못하는지를 확인한다

```__cplusplus```와 ```__OBJC__``` 매크로를 사용해서 C++일때만 사용할 소스/아닐떄 사용할 소스를 분리한다. 이것으로 ios에서 obj-c와 c++을 섞어쓸수 있다.

SR_USE_PCH 매크로를 사용해서 진짜 PCH의 내용이 될것을 잔뜩 집어넣는다. 이부분은 일반적으로 PCH를 작성하는방법과 동일하다.

이런식으로 구성하면 별 문제없이 윈도우/아이폰/안드로이드를 커버할수 있다. 적절히 적용하여 빌드시간을 줄이고 적절히 개발을 하자.
