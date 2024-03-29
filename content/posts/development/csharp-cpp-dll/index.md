---
title: C#에서 C++ DLL 사용하기
tags: [csharp, cpp, dll, pinvoke]
slug: csharp-cpp-dll
date: 2013-02-12
author: if1live
url: /posts/csharp-cpp-dll
---

## 개요
C#에서 C++ dll을 사용하기 위해서는 pinvoke(platform invoke)를 사용하면 된다. 아래의 내용은 사용법에 대한 간단한 정리이다. 내용 생략하고 [Sample](glcombinesample.zip) 를 받아도 된다.

## C++
### h
```cpp
// Ŭnicode please
#pragma once

#include <cstdio>

namespace ns {;
typedef struct tTest {
    char  strTest[128]; //문자열 128
    int   intTest;    //숫자형
    char byteTest[64]; //바이트형 배열
    unsigned int  uintTest[4];  //유니트형 배열

} typeTest;
}

extern "C" __declspec(dllexport) void   OnTest1(void);      //기본형
extern "C" __declspec(dllexport) int    intOnTest2(int intTemp);    //입출력 숫자형
extern "C" __declspec(dllexport) int*  strOnTest3();  //입출력 문자열형
extern "C" __declspec(dllexport) void   OnTest4(ns::typeTest *testTemp);    //입력 구조체(포인터 출력가능)
extern "C" __declspec(dllexport) void   OnTest5(int *intTemp);  //입출력 배열(포인터 출력가능)
```

* 문자열을 반환하는 경우 char포인터를 리턴하는 대신 int*를 리턴하고 C#쪽에서 적절히 캐스팅해서 받아야한다
* C#쪽에서 구조체를 구성하는것은 메모리 구조만 맞추면 되니까 네임스페이스건 뭐건 잇건 없건 큰 문제가 없다
* dll 인터페이스로 만들기 위해서 ```extern "C" __declspec(dllexport)```를 함수에 넣어준다

### cpp
```cpp
// Ŭnicode please
#include "sample.h"

void OnTest1(void)
{
    //기본형
  printf("fdsfd\n");
}

int intOnTest2( int intA)
{
    //입출력 숫자형

    ++intA; //입력받은 숫자에 +1

    return intA;
}

int* strOnTest3()
{
    //입출력 문자열형

    //static char strTemp2[128] = {0,};   //임시저장용 문자열
    //sprintf_s( strTemp2, "%s strOnTest3 에서 리턴", strTemp);   //문자열 합치기
    static char strTemp2[256] = "asdfasds\n";
    return (int*)strTemp2;
}

void OnTest4( ns::typeTest *testTemp )
{
    //입력 구조체형(포인터 출력가능)

    testTemp->byteTest[0] = 1;
    testTemp->intTest = testTemp->intTest + 2;
    sprintf_s( testTemp->strTest, "%s OnTest4에서 포인터", testTemp->strTest);
    testTemp->uintTest[0] = 1;
}

void OnTest5(int *intTemp)
{
    //입출력 배열형(포인터 출력 가능)
    for( int i = 0 ; i < 2 ; ++i )
    {
        intTemp[i] = intTemp[i] + i;
    }
}
```

## C\#
### 구조체
```csharp
public struct typeTest
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
	public String strTest;

	public int intTest;

	[MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
	public byte[] byteTest;

	[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
	public UInt32[] uintTest;
}
```

### 함수
```csharp
[DllImport("dll_sample.dll")]
extern public static void OnTest1();

[DllImport("dll_sample.dll", CallingConvention = CallingConvention.Cdecl)]
extern public static int intOnTest2(int intTemp);

[DllImport("dll_sample.dll", CharSet = CharSet.Ansi)]
extern public static IntPtr strOnTest3();

[DllImport("dll_sample.dll", CallingConvention = CallingConvention.Cdecl)]
extern public static void OnTest4(ref typeTest testTemp);

[DllImport("dll_sample.dll", CallingConvention = CallingConvention.Cdecl)]
extern public static void OnTest5(int[] intTemp);
```

```csharp
private void button1_Click(object sender, RoutedEventArgs e)
{
    OnTest1();
}

private void button2_Click(object sender, RoutedEventArgs e)
{
    int a = 1;
    a = intOnTest2(a);
    System.Console.WriteLine(a);
}

private void button3_Click(object sender, RoutedEventArgs e)
{
    IntPtr a = strOnTest3();
    string s = Marshal.PtrToStringAnsi(a);
    System.Console.WriteLine(s);
}

private void button4_Click(object sender, RoutedEventArgs e)
{
    typeTest testTemp = new typeTest();

    testTemp.byteTest = new byte[64];
    testTemp.uintTest = new uint[4];

    testTemp.strTest = "asd";
    testTemp.intTest = 3;
    testTemp.byteTest[0] = (byte)'a';
    testTemp.uintTest[0] = 12;


    OnTest4(ref testTemp);

    System.Console.WriteLine(testTemp);
}

private void button5_Click(object sender, RoutedEventArgs e)
{
    int[] intTemp = new int[2];
    intTemp[0] = 1;
    intTemp[1] = 2;
    OnTest5(intTemp);
    System.Console.WriteLine(intTemp);
}
```

* 함수호출할때 인자를 사용하는 경우, ```CallingConvention = CallingConvention.Cdecl``` 가 필요하다
* C++에서 char포인터를 반환하는 경우, C#에서의 type은 IntPtr이다. ```Marshal.PtrToStringAnsi``` 를 사용해서 string으로 바꿀수잇다. string으로 바로 받으면 에러가 발생한다

## Reference
* [How to Marshal a C++ Class](http://www.codeproject.com/Articles/18032/How-to-Marshal-a-C-Class)
* [함수마샬링](http://blog.danggun.net/16)
* [클래스마샬링](http://blog.danggun.net/799)
