Title: 플랫폼 매크로 추가 선언 없이 윈도/iOS/안드로이드 구분하는 매크로
Tags: cpp, cross platform
Slug: platform-macro
Author: if1live
Summary: 플랫폼 매크로 추가 선언 없이 윈도/iOS/안드로이드 구분하는 매크로
Date: 2012-11-30

크로스플랫폼 코딩을 하다보면 플랫폼 별로 ifdef...endif같은것을 사용해서 코딩할 일이 생긴다. 프로젝트설정이나 Makefile안에 플랫폼 정보를 추가해서 처리할수도 있는데 이렇게 하면 귀찮다. (cocos2d-x의 경우는 프로젝트 설정-매크로에서 ```WIN32;_WINDOWS```과 같은 것을 추가해서 플랫폼별 처리에 사용한다). 그래서 나는 아래처럼 컴파일러 수준에서 추가된 내장 매크로를 사용해서 플랫폼 분기할때 사용한다. 자세하게 따지면 뭔가 부실하지만 아이폰/윈도우/안드로이드 까지는 문제없이 구분한다

```cpp
// Ŭnicode please
#pragma once
//platform check macro
//cocos2d의 내장 기능을 써도 되는데 그것을 쓰면 프로젝트자체에서 Macro 설정을 해야되서
//귀찮아 -_- 그래서 독립 파일로 인클루드해도 돌아가도록 만들엇다
#if __APPLE__
//iphone check
    #define SR_IPHONE 1
#else
    #define SR_IPHONE 0
#endif

#if (defined _WIN32) || (defined _W64)
    #define SR_WIN 1
#else
    #define SR_WIN 0
#endif

//android check
#if (SR_IPHONE == 0) && (SR_WIN == 0)
	#define SR_ANDROID 1
#else
	#define SR_ANDROID 0
#endif

```
