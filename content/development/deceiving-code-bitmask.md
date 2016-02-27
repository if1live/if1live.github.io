Title: bit mask로 사람 낚기. (flags & MASK == MASK)
Subtitle: 연산자 우선순위로 낚시하기. 그리고 회피법
Slug: deceiving-code-bitmask
Tags: bitmask, 비트마스트, 낚시
Date: 2015-11-01
Author: if1live

낚시 코드
---------

C/C++ 에서는 한번에 여러개의 플래그를 넘길떄 비트 연산자를 사용하기도
한다. 다음은 간단한 예제 코드이다. 사용 가능한 플래그로는 fullscreen,
stencil, double buffer가 있다. 이중에서 fullscreen, double buffer를 비트
OR 연산자를 이용해서 합친 다음에 함수로 넘겼다. 그리고 함수에서는 어떤
플래그가 켜져있는지 확인해서 적절한 출력을 한다.

```c
#include <stdio.h>

typedef enum {
  WIN_MODE_FULLSCREEN = 1 << 0,
  WIN_MODE_STENCIL = 1 << 1,
  WIN_MODE_DOUBLE_BUFFER = 1 << 2
} win_mode_t;

int create_window(win_mode_t mode)
{
  if(mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN) {
    printf("fullscreen\n");
  }
  if(mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL) {
    printf("stencil\n");
  }
  if(mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER) {
    printf("double buffer\n");
  }
  return 0;
}

int main()
{
  create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER);
  return 0;
}
```

```
$ gcc buggy.c
./a.out
fullscreen
stencil
double buffer
```

코드를 실행해보면 위의 설명과 실제 동작이 다르다. 옵션으로 fullscreen,
double buffer를 사용했는데 의도하지 않은 stencil 이 출력되었다. 원인은
연산자 우선순위이다.

[C Operator
Precedence](http://en.cppreference.com/w/c/language/operator_precedence)

`==` 연산자의 우선순위는 7인데 `&` 연산자의 우선순위는 8이다. 그래서
`mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL` 의 진짜 의미는
`(mode & WIN_MODE_STENCIL) == WIN_MODE_STENCIL` 가 아니라
`mode & (WIN_MODE_STENCIL == WIN_MODE_STENCIL)` 이다.

제대로 돌아가게 하기 위해서는 괄호를 사용해서 계산 순서를 바꾸면 된다.
하지만 실수를 근본적으로 막을수 있는 방법이 있지 않을까?

대안1. 경고 강화
----------------

이전에 컴파일할때는 gcc에 옵션을 아무것도 넘기지 않았다. `-W -Wall`
옵션을 사용해서 다시 컴파일해보자.

```
$ gcc buggy.c -W -Wall
buggy.c: In function ‘create_window’:
buggy.c:11:3: warning: suggest parentheses around comparison in operand of ‘&’ [-Wparentheses]
if(mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN) {
...
```

옵션없이 컴파일할때는 아무런 경고 없이 컴파일 되었지만 옵션을 추가하니까
경고가 보인다. 경고의 내용은 괄효를 이용해서 `&` 연산자가 사고치지 않게
바꾸라는거다.

컴파일 워닝은 표준이 있는게 아니라 컴파일러의 구현에 따라 다르기 때문에
다른 컴파일러를 사용하면 다른 결과를 볼 수 있다. clang의 경우는 옵션을
붙이지 않아도 경고 메세지를 보여준다.

컴파일러가 코드의 잠재적 문제를 인식해서 경고를 보여줄 수 있으니까
여기에 경고를 에러로 취급하는 컴파일 옵션을 사용하면 버그를 피할 수
있다.

대안2. int != boolean
---------------------

연산자 우선순위를 고려해서 `mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL`
를 풀어쓰면 다음과 같다.

1.  `WIN_MODE_STENCIL == WIN_MODE_STENCIL` 를 계산. 타입은 `boolean`
2.  int인 mode와 boolean인 위의 계산결과를 이용해서 비트 AND
    연산자 수행.

C/C++의 경우는 int과 boolean을 계산에서 같이 이용할수 있다. boolean
true면 int 1, boolean false 는 int 0과 같은 것으로 취급한다. 그래서 비트
AND 연산자의 수행이 가능하다.

하지만 C#의 경우는 int와 boolean으로 비트 AND 연산자 수행이 불가능하다.
C/C++ 처럼 암묵적으로 타입을 바꾸지 못한다. 그래서 컴파일 에러가
발생하고 버그가 있는 코드를 짜지 못한다.

```csharp
public class Hello1
{
    const int WIN_MODE_FULLSCREEN = 1 << 0;
    const int WIN_MODE_STENCIL = 1 << 1;
    const int WIN_MODE_DOUBLE_BUFFER = 1 << 2;

    public static int create_window(int mode)
    {
        if(mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN) {
            System.Console.WriteLine("fullscreen");
        }
        if(mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL) {
            System.Console.WriteLine("stencil");
        }
        if(mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER) {
            System.Console.WriteLine("double buffer");
        }
        return 0;
    }

    public static void Main()
    {
        create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER);
    }
}
```

```
$ gmcs alternative_strongly_typed.cs
alternative_strongly_typed.cs(9,12): error CS0019: Operator `&' cannot be applied to operands of type `int' and `bool'
alternative_strongly_typed.cs(12,12): error CS0019: Operator `&' cannot be applied to operands of type `int' and `bool'
alternative_strongly_typed.cs(15,12): error CS0019: Operator `&' cannot be applied to operands of type `int' and `bool'
Compilation failed: 3 error(s), 0 warnings
```

대안3. 연산자 우선순위 조정
---------------------------

문제가 발생하는 코드를 파이썬으로 옮겨쓰면 다음과 같다. 그리고 이를
실행하면 최초에 의도한대로 작동한다.

```py
WIN_MODE_FULLSCREEN = 1 << 0
WIN_MODE_STENCIL = 1 << 1
WIN_MODE_DOUBLE_BUFFER = 1 << 2

def create_window(mode):
    if mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN:
        print("fullscreen");

    if mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL:
        print("stencil");

    if mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER:
        print("double buffer")


if __name__ == "__main__":
    create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER)
```

```
$ python alternative_operator_precedence.py
fullscreen
double buffer
```

파이썬의 경우는 연산자 우선순위를 바꾸는 방법으로 문제를 피했다.

[Python Operator
precedence](https://docs.python.org/3/reference/expressions.html#operator-precedence)

파이썬의 경우는 `&` 연산자의 우선순위가 `==` 보다 높다. 그래서
`mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL` 는
`(mode & WIN_MODE_STENCIL) == WIN_MODE_STENCIL` 처럼 작동한다.

파이썬의 이러한 접근법은 재밌는 회피법이지만 언어의 기본인 연산자
우선순위를 바꿔버렸기 때문에 다른 언어에서 작성된 코드를 그대로 옮길때
조심해야한다. (C#의 경우는 컴파일을 못하는 정도로 끝나지만 파이썬은
기존의 동작이 바뀔 수도 있다)

Summary
-------

* C/C++ 을 사용하는 경우
    * 경고 옵션을 높인다.
    * 경고를 에러로 취급하는 옵션을 사용한다.
* 언어에 따라서 문제 해결 방식이 다르다. 자신이 사용하는 언어의 특징을 기억해야 한다.

Links
-----

* [Test codes](https://github.com/if1live/libsora.so/tree/master/content/development/deceiving-code-bitmask)
