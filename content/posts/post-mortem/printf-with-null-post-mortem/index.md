---
title: printf("%s\n", NULL)의 출력 결과는?
subtitle: undefined behavior과 compiler
tags: [c, c++, printf, undefined behavior, gcc, postmortem]
slug: printf-with-null-post-mortem
date: 2016-03-06
author: if1live
url: /posts/printf-with-null-post-mortem
---

## 떡밥의 시작
오늘도 평소와 똑같이 [트위터는 인생의 낭비](https://namu.wiki/w/%ED%8A%B8%EC%9C%84%ED%84%B0%EB%8A%94%20%EC%9D%B8%EC%83%9D%EC%9D%98%20%EB%82%AD%EB%B9%84)를 라는 명언대로 행동하다 재밌는 트윗을 봤다.

> New Tweet "트위터만 보고 있지 말고 %s 해야지"
> [@_Yous](https://twitter.com/_Yous/status/706349364965289985)

트윗을 보자마자 `%s`에다가 `NULL`을 넣어보았다.

> New Tweet "트위터만 보고 있지 말고 %s 해야지", NULL
> [@if1live](https://twitter.com/if1live/status/706350121651339265)

트윗을 쓰고 나니 옛날에 비슷한걸 해본적이 생각났다.
**그러고보니 printf(%s”, NULL) 해도 프로그램이 안뒤졌던거같은 기분이 드는데?**
그래서 직접 돌려봤다.

```c
#include <stdio.h>
int main()
{
  printf("string = %s\n", NULL);
  return 0;
}
```

```bash
$ clang simple.c && ./a.out
string = (null)
```

안죽네? 게다가 `(null)`은 어디서 튀어나온거지?
왜 그런지 이유를 찾아보자.

## 스펙...스펙을 보자!
[printf reference](http://en.cppreference.com/w/c/io/fprintf)를 읽어봤는데 "(null)"과 관련된 내용이 안보이더라.
`%s`에다가 `NULL`을 넣는게 undefined behavior일거같다는 생각이 들었는데 좀더 깊이 파보기로 했다.
까막눈이라 영어 읽는게 어려우니 차라리 코드를 뒤져보자.
코드가 어떻게 행동하는지를 보고 이 동작이 스펙인지 아닌지를 유추해보자.

## glibc
glibc의 printf 구현은 다음 파일에서 찾을수 있다.
[stdio-common/vfprintf.c](https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/vfprintf.c;h=6829d4dc8e7fe7c066a06f1857ee926e0f48c379)
printf 코드는 크고 아름다우니 `printf("%s\n", NULL)` 의 행동과 관련된 코드만 찾아보자.

```c
REF (precision), /* for '.' */
```
printf의 경우 포매팅에서 `.`를 정밀도(precision)으로 사용한다.

```c
int prec = -1;	/* Precision of output; -1 means none specified.  */
```
정밀도(prec)의 기본값은 -1이다. 나중에 이 값을 쓰게 된다.

```c
REF (form_string), /* for 's', 'S' */
```
확인하고 싶은건 `%s`의 행동이다. `form_string` 으로 연결된 것만 나중에 확인하면 된다.

```c
/* Global constants.  */
static const CHAR_T null[] = L_("(null)");
```
`(null)`를 찾았다. 문자열 `null`이 어디에서 쓰이는지를 찾아보면 될거다.

```c
LABEL (form_string):
  {
    ...
    if (string == NULL)
    {
      /* Write "(null)" if there's space.  */
      if (prec == -1 || prec >= (int) sizeof (null) - 1)
        {
          string = (char *) null;
          len = sizeof (null) - 1;
        }
      else
        {
          string = (char *) "";
          len = 0;
        }
    }
    ...
```

1. `string`가 `NULL`인 경우...
2. 정밀도(prec)가 기본값이거나 `(null)`의 길이보다 큰 경우
    1. `(null)`를 출력
3. 그렇지 않으면(올바른 범위의 정밀도를 사용한다면)
    1. 아무것도 출력하지 않는다.

`%s`에서 정밀도를 쓸 수 있는건 처음 알았다. 배웠으니 한번 써보자.

```c
#include <stdio.h>
int main()
{
  printf("%s\n", NULL);
  printf("%.0s\n", NULL);
  printf("%.3s\n", NULL);
  return 0;
}
```

```bash
$ clang printf_prec.c && ./a.out
(null)

(nu
```

내가 OSX를 쓰는데 glibc의 코드에서 확인한 행동과 osx에서의 행동이 다르다.
glibc였다면 마지막 출력은 `(nu`가 아니라 공백이어야 한다.
여기에서 두가지 사실을 유추할수 있다.

1. OSX에서는 glibc를 쓰지 않는다. 다른 C 라이브러리를 쓰고있다.
2. glibc와 OSX에서 사용하는 C 라이브러리의 출력이 다르니 이것은 undefined behavior이다.

## libc
여기까지 온 이상 OSX의 C 라이브러리에서는 어떻게 구현했는지도 확인해보자.
[‏@sairion](https://twitter.com/sairion/status/706360731562672128)님의 도움을 받아서 OSX에서는 [libc](http://www.opensource.apple.com/source/Libc/)를 사용한다는걸 알았다.

libc에서는 [stdio/FreeBSD/vfprintf.c](http://www.opensource.apple.com/source/Libc/Libc-1082.20.4/stdio/FreeBSD/vfprintf.c)에 printf의 구현체가 있다.
glibc와 마찬가지로 코드를 살펴보자.

```c
case 's':
  if (flags & LONGINT) {
    ...
    if ((wcp = GETARG(wchar_t *)) == NULL)
      cp = "(null)";
    ...
  } else if ((cp = GETARG(char *)) == NULL)
    cp = "(null)";
  size = (prec >= 0) ? strnlen(cp, prec) : strlen(cp);
  sign = '\0';
  break;
```

1. 출력할 대상이 `NULL`이면 `(null)`을 출력 후보로 바꾼다.
2. 정밀도가 있으면 정밀도만큼 size를 잡는다. 정밀도가 없으면 문자열 `(null)`의 길이를 size로 쓴다.

그래서 libc에서는 `NULL`과 정밀도를 같이 쓰면 문자열이 잘려서 출력된다.

## 번외편 : gcc + printf("%s\n", NULL)
> gcc 에서 printf("%s\n", NULL); 하면 segfault 나고 printf("Test %s\n",NULL) 하면 Test (null) 나옴
> [@mega_kina](https://twitter.com/mega_kina/status/706357586090598400)

clang만 쓰다보니까 gcc의 동작은 테스트를 제대로 안해봤는데 진짜로 저렇게 행동한다.
뒤져보니 gcc의 최적화에 대해서 정리한 글이 있더라. 자세한 내용은 아래의 글을 참고하기 바란다.
[About GCC printf optimization](http://www.ciselant.de/projects/gcc_printf/gcc_printf.html)

영어 읽기 싫은 사람을 위해서 간단하게 요약했다.
**gcc는 특정 조건에서 `printf()`를 `puts()`로 바꾼다. 코드에서 `printf()`를 호출해도 `puts()`가 대신 호출된다는거다.**

참고: 2005년도에 작성된 글이라 gcc 최신버전인 5.3.0에서는 파일, 함수이름이 다르다. `gimple_fold_builtin_printf()`
[gcc/gimple-fold.c](https://github.com/gcc-mirror/gcc/blob/gcc_5_3_0_release/gcc/gimple-fold.c#L2691-L2851)

## 흑마법사를 위한 가이드
새로운 undefined behavior를 배웠으면 이를 이용해서 어떻게 사람을 엿먹일지를 생각해봐야한다. 그래야 내가 엿먹기 전에 남을 엿먹일수 있다. (선빵필승!)
몇가지 알려진 편법을 응용해보자.

* 회원가입을 할때 생일을 `1970년 1월 1일`로 입력한다. 개발자는 날짜가 제대로 입력되지 않는 버그가 있는줄 알고 디버깅을 할 것이다.
* 회원가입을 할때 이름을 `NULL`로 입력한다. 개발자는 어딘가에 버그가 있다고 생각하고 디버깅을 할 것이다.

```c
#include <stdio.h>
const char *hello(const char *name)
{
  static char msg[256];
  sprintf(msg, "Hello, %s", name);
  return msg;
}

int main()
{
  printf("%s\n", hello("(null)"));
  printf("%s\n", hello(NULL));
  return 0;
}
```

```bash
$ clang trap_fake_null.c && ./a.out
Hello, (null)
Hello, (null)
```

진짜로 `(null)`을 출력하고자한 경우와 실수로 `NULL`이 들어간 경우를 구분할수 없다.
함정을 밟지 않으려면 `NULL` 확인을 앞에 배치하는게 좋을거다.

```c
#include <stdio.h>
#include <assert.h>
const char *hello(const char *name)
{
  assert(name != NULL);
  static char msg[256];
  sprintf(msg, "Hello, %s", name);
  return msg;
}

int main()
{
  printf("%s\n", hello("(null)"));
  printf("%s\n", hello(NULL));
  return 0;
}
```

```bash
$ clang trap_fake_null_alt.c && ./a.out
Assertion failed: (name != NULL), function hello, file trap_fake_null_alt.c, line 5.
Hello, (null)
/var/folders/mt/f4mnd72x2_526m85rvxhg3bc0000gn/T/maya264151972: line 1:  1788 Abort trap: 6           ./a.out
```


## 요약
* `printf("%s\n", NULL)`은 undefined behavior이다.
* `vsprintf()` 계열의 함수에서 undefined behavior부분은 구현체에 따라 다르다.
* 최적화를 위해 컴파일러가 호출하는 함수를 바꿔버리는 경우도 있다.
* <s>잘 모르겠으면 일단 구현체 소스를 까보자</s>


## 도전 과제
그렇다면 윈도우에서는 어떻게 작동할까?
내가 컴맹이라서 윈도우는 잘 모르니 참고용 트윗만 기록해둔다.

[왜냐하면 널이 인자로 넘어가는 경우는 undefined behavior라 컴파일러 벤더 맘대로 구현해도 되서 그렇습니다..](https://twitter.com/summerlight00/status/706358610528329728)

[msvc 같은 경우는 최근 오픈 소스 라이브러리의 윈도우 이식을 쉽게 하려고 c99 함수들 구현하는 김에 c 표준 라이브러리를 새로 재구현했는데요, 이런 맥락이라면 gcc 동작을 그대로 베끼는게 이해는 갑니다.](https://twitter.com/summerlight00/status/706360559063576576)

[Microsoft Visual Studio 14.0\VC\crt\src\i386 경로에 있네요 ](https://twitter.com/sokcuri/status/706366233583595520)
