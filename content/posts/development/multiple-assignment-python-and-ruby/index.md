---
title: 예제로 보는 Python과 Ruby의 문법 차이 - Multiple Assignment
subtitle: Multiple Assignment with (Python 2.x, Python 3.x, Ruby)
tags: [python, ruby, multiple assignment]
slug: multiple-assignment-python-and-ruby
author: if1live
date: 2016-02-14
url: /posts/multiple-assignment-python-and-ruby
---
파이썬과 루비는 겉보기에는 비슷해보이면서 완전히 다르게 행동하는 문법이 있다.
그래서 파이썬을 생각하면서 루비 코드를 짜면 어처구니 없는 버그가 생긴다.
이런 일을 방지하고자 지금까지 당해본 파이썬과 루비의 문법 차이를 정리하려고 한다.
오늘은 Multiple Assignment이다. (여러 개의 변수에 값을 동시에 할당하기)

## Python 2.x

변수 1개에 정수를 1개 할당하는 것부터 시작하자.
매우 기본적인 문법이다.
```
source: a = 1
a=1
```

변수 2개에 정수를 2개 할당하면 어떻게 될까?
``1, 2`` 이라고 썻기때문에 값이 2개인 것처럼 보이지만 사실 파이썬에서는 ``(1, 2)`` 같이 튜플로 묶어서 처리한다.
2개의 값이 들어있는 하나의 튜플을 쪼개서 2개의 변수에 할당하는 식으로 동작한다. (이는 쉘에서 확인할 수 있다)

```
source: a, b = 1, 2
a=1
b=2

>>> 1, 2
(1, 2)
```


변수 1개에 정수를 2개 할당하면 어떻게 될까?
``1,2``는 ``(1, 2)``과 동일하기 때문에 변수에 튜플이 할당된다.
```
source: a = 1, 2
a=(1, 2)
```

변수 2개에 정수 값을 하나만 할당하면 어떻게 될까?
튜플이었으면 쪼개서 변수에 값을 할당하는 것이 가능하지만 정수는 그렇게 할수 없다.
그래서 예외가 발생한다.

```
source: a, b = 1
TypeError: 'int' object is not iterable
```

변수 2개에 정수값 3개를 할당하면 어떻게 될까?
남는 값을 변수에 할당할수 없어서 예외가 발생한다.
```
source: a, b = 1, 2, 3
ValueError: too many values to unpack
```

wildcard 같은 느낌으로 매칭되고 남은 것을 튜플로 묶어서 변수에 할당하는게 가능할까?
아쉽게도 파이썬 2.x에는 이런 문법이 존재하지 않는다.
```
source: a, *b = 1, 2, 3
SyntaxError: invalid syntax (<string>, line 1)
```

## Python 3.x

파이썬 3.x는 단순한 버전 업그레이드가 아니다. 문법 차이가 많이 존재한다.
그 중의 하나가 multiple assignment이다.

파이썬 2.x와 동일한 내용은 결과만 기록해두었다.
```
source: a = 1
a=1

source: a, b = 1, 2
a=1
b=2

source: a = 1, 2
a=(1, 2)

source: a, b = 1
TypeError: 'int' object is not iterable
```

변수 2개에 정수값 3개를 할당하면 파이썬 2.x와 비슷하면 예외가 발생하지만 완전히 똑같진 않다.
예외 메세지 안에 ``(expected 2)`` 가 추가되었다.
예외 메세지가 바뀌었다는건 multiple assignment에서 파이썬 2.x와 행동이 다른 부분이 있다는거겠지?

```
source: a, b = 1, 2, 3
ValueError: too many values to unpack (expected 2)
```

파이썬 3.x부터는 튜플의 크기가 변수보다 큰 경우 남는 것을 몰아서 할당하는 것이 가능하다.
wildcard같은 느낌으로 변수앞에 ``*``를 붙여주면 된다.
``*``가 없는 항목에 값이 1개 들어가고 ``*``가 있는 것에는 남은 변수가 튜플 형태로 할당된다.
```
source: a, *b = 1, 2, 3
a=1
b=[2, 3]
```

``*``는 맨 앞에 오는것도 가능하고 중간에 오는것도 가능하고 끝에 오는것도 가능하다.
적절하게 매칭되서 변수에 값이 할당된다.

```
source: a, b, *c = 1, 2, 3, 4, 5
a=1
b=2
c=[3, 4, 5]

source: a, *b, c = 1, 2, 3, 4, 5
a=1
b=[2, 3, 4]
c=5

source: *a, b, c = 1, 2, 3, 4, 5
a=[1, 2, 3]
b=4
c=5
```

변수의 갯수보다 튜플의 크기가 작아서 ``*``에 할당될 변수가 없을때도 예외없이 잘 굴러간다.

```
source: a, *b, c = 1, 2
a=1
b=[]
c=2
```

## Ruby

변수 하나에 정수값 하나를 할당하는건 기본적인 문법이다.
```
source: a = 1
a = 1
```


변수 2개에 정수값 2개를 할당하는 것도 가능하다.
```
source: a, b = 1, 2
a = 1
b = 2
```

겉보기에는 파이썬과 똑같아보이지만 내부는 다르다.
루비에서는 ``1, 2``만 쓰면 문법 에러가 발생한다. (파이썬에서는 튜플로 취급)
```
irb(main):001:0> 1, 2
SyntaxError: (irb):1: syntax error, unexpected ',', expecting end-of-input
        from /Users/yurie/.rbenv/versions/2.1.5/bin/irb:11:in `<main>'
```

변수 하나에 정수값 2개를 할당하면 배열이 된다.
```
source: a = 1, 2
a = [1, 2]
```


변수 2개에 정수값 하나를 할당하면 어떻게 될까?
파이썬에서는 예외가 발생했다.
루비에서는 문제없이 실행된다. 다만 짝이 맞지않는 변수에는 ``nil``이 들어간다.
```
source: a, b = 1
a = 1
b = nil
```

변수 2개에 정수갑 3개를 할당하면 어떻게 될까?
파이썬에서는 예외가 발생했다.
루비에서는 문제없이 실행된다. 그리고 짝이 맞지 않는 값은 버린다.
```
source: a, b = 1, 2, 3
a = 1
b = 2
```

``*`` 를 이용하면 파이썬 3.x와 같은 결과가 나온다.

```
source: a, *b = 1, 2, 3
a = 1
b = [2, 3]

source: a, b, *c = 1, 2, 3, 4, 5
a = 1
b = 2
c = [3, 4, 5]

source: a, *b, c = 1, 2, 3, 4, 5
a = 1
b = [2, 3, 4]
c = 5

source: *a, b, c = 1, 2, 3, 4, 5
a = [1, 2, 3]
b = 4
c = 5

source: a, *b, c = 1, 2
a = 1
b = []
c = 2
```

## 응용 뻘짓

{{< gist if1live 5ec0e00d0b27c6feaee9 >}}

할당할 변수는 10개지만 Array에 들어있는 요소는 9개이다.
그래서 마지막 요소인 Pluto에는 nil이 들어간다.
만약 파이썬이었으면 문법 에러가 발생했을 것이다.

## test code
* [test code](https://github.com/if1live/libsora.so/tree/master/content/development/multiple-assignment-python-and-ruby)
