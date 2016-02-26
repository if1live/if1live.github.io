Title: LibOVR에서 찾은 적절한 assert macro
Subtitle: 어지간한 플랫폼에서 잘 굴러가는 assert macro
Date: 2013-10-14
Tags: assert, macro
Slug: libovr-assert-macro
Author: if1live

개발하려면 어쨋든 assert가 있긴 해야겠더라. 근데 기본 assert를 쓰면 callstack 추적하기가 귀찮더라.
그래서 맨날 간단한 매크로([int 3](http://kblog.popekim.com/2011/06/assert-int-3.html)) 만들어서 사용했었는데 나중에 알고보니까 해당 매크로는 64비트 환경에서 작동하지 않는다.
남들은 어떻게 하나 찾아보다가 때마침 Oculus SDK를 보게 되었는데 매우 적절하게 있어서 그냥 갖다쓴다.
원본 소스는 [OVR_Types.h](https://github.com/if1live/LibOVR/blob/master/Src/Kernel/OVR_Types.h) 이다.

```cpp
// Microsoft Win32 specific debugging support
#if defined(OVR_OS_WIN32)
#  ifdef OVR_CPU_X86
#    if defined(__cplusplus_cli)
#      define OVR_DEBUG_BREAK   do { __debugbreak(); } while(0)
#    elif defined(OVR_CC_GNU)
#      define OVR_DEBUG_BREAK   do { OVR_ASM("int $3\n\t"); } while(0)
#    else
#      define OVR_DEBUG_BREAK   do { OVR_ASM int 3 } while (0)
#    endif
#  else
#    define OVR_DEBUG_BREAK     do { __debugbreak(); } while(0)
#  endif
// Unix specific debugging support
#elif defined(OVR_CPU_X86) || defined(OVR_CPU_X86_64)
#  define OVR_DEBUG_BREAK       do { OVR_ASM("int $3\n\t"); } while(0)
#else
#  define OVR_DEBUG_BREAK       do { *((int *) 0) = 1; } while(0)
#endif

// This will cause compiler breakpoint
#define OVR_ASSERT(p)           do { if (!(p))  { OVR_DEBUG_BREAK; } } while(0)
```

p.s. OculusSDK 소스를 보면 플랫폼 감지 플랫폼에 별게 다 들어가있다. 아이폰, 안드로이드까지 감지 가능한 매크로이다. 이것이 OulusRift의 야심찬 계획...??

```cpp
//-----------------------------------------------------------------------------------
// ****** Operating System
//
// Type definitions exist for the following operating systems: (OVR_OS_x)
//
//    WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
//    DARWIN   - Darwin OS (Mac OS X)
//    LINUX    - Linux
//    ANDROID  - Android
//    IPHONE   - iPhone

#if (defined(__APPLE__) && (defined(__GNUC__) ||\
     defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
#  if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#    define OVR_OS_IPHONE
#  else
#    define OVR_OS_DARWIN
#    define OVR_OS_MAC
#  endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define OVR_OS_WIN32
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define OVR_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#  define OVR_OS_LINUX
#else
#  define OVR_OS_OTHER
#endif

#if defined(ANDROID)
#  define OVR_OS_ANDROID
#endif
```
