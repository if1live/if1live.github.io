+++
title: 올바르지 않은 값을 정수로 변환하기 - 파이썬 vs 루비
subtitle: 같은 목적, 다른 언어, 다른 결과
slug: make-int-from-invalid-value-python-and-ruby
tags: [python, ruby, integer, int, exception]
date: 2016-02-24
author: if1live
+++
올바르지 않은 값을 정수로 바꾸는 일은 웹을 포함한 여러가지 개발에서 자주하는 작업 중 하나이다.
예를 들어서 `/articles/?id={int}` 를 구현한 서버가 있다고 가정하자.
`/articles/?id=1234`와 같이 id에 정수 문자열이 들어갈수도 있다.
하지만 `/articles/?id=test`와 같이 숫자가 아닌 문자열이 들어갈수도 있고 `/articles/`처럼 id가 없을수도 있다.
개발자는 이런 예외 상황을 잘 처리해야된다.

이 글에서는 파이썬과 루비에서 문자열을 숫자로 변환할때의 차이점을 간단하게 정리했다.

## String to int

### Success
`"123"`과 같이 딱봐도 정수처럼 보이는 문자열을 정수로 바꿔보았다.

~~~maya:view
file=str_to_int_success.py
~~~

~~~maya:execute
cmd=python str_to_int_success.py
attach_cmd=true
~~~

~~~maya:view
file=str_to_int_success.rb
~~~

~~~maya:execute
cmd=ruby str_to_int_success.rb
attach_cmd=true
~~~

파이썬과 루비 모두 정상적으로 작동한다.

### Fail
`"fail"`과 같이 정수처럼 보이지 않는 문자열을 정수로 바꿔보면 어떨까?

~~~maya:view
file=str_to_int_fail.py
~~~

~~~maya:execute
cmd=python str_to_int_fail.py
attach_cmd=true
~~~

~~~maya:view
file=str_to_int_fail.rb
~~~

~~~maya:execute
cmd=ruby str_to_int_fail.rb
attach_cmd=true
~~~

파이썬에서는 예외가 발생하지만 루비에서는 0으로 변환된다.

* Python: [int()](https://docs.python.org/3.5/library/functions.html#int)
* Ruby: [String#to_i](http://ruby-doc.org/core-2.3.0/String.html#method-i-to_i)


## None to int, nil to Integer
파이썬에서는 None, 루비에서는 nil이라고 부르지만 둘은 비슷한 개념이다.
이것을 정수로 변환하면 어떤 결과가 나올까?

~~~maya:view
file=null_to_int.py
~~~

~~~maya:execute
cmd=python null_to_int.py
attach_cmd=true
~~~

~~~maya:view
file=null_to_int.rb
~~~

~~~maya:execute
cmd=ruby null_to_int.rb
attach_cmd=true
~~~

파이썬에서는 예외가 발생하지만 루비에서는 0으로 변환된다.

* Ruby: [NilClass#to_i](http://ruby-doc.org/core-2.3.0/NilClass.html#method-i-to_i)

## 동일한 동작을 하게 만드려면?

"파이썬에서는 예외가 나는게 정상이고 루비에서는 0으로 변환되는게 정상입니다"로 끝내기에는 간단한 문제가 아니다.
파이썬으로 작성된 코드가 있고 이것을 루비로 그대로 옮겨서 사용하고 싶다고 가정하자.
만약 파이썬 코드안에 문자열을 숫자로 바꾸는 코드가 있다면 루비로 그대로 옮겼을때 동일하게 동작한다는 보장이 없다.
예외를 던지는 것과 0으로 변환되는 것은 다른 행동이기 때문에 코드를 옮기면서 로직을 수정해야될지 모른다.
만약 파이썬처럼 루비에서도 문자열을 정수로 변환하려다 실패한 경우 예외를 던지면 포팅 작업이 더 편하지 않을까?

루비에서 파이썬처럼 예외를 던지려면 어떻게 해야될까?
`#to_i`를 이용해서 정수로 변환해보고 만약 0이 나온 경우 문자열이 `"0"`, `"0.00"`등과 같은지 확인한 다음에 예외를 던지는 식으로  구현할수 있다.
하지만 좋은 방법처럼 보이진 않는다. 더 쉬운 방법 없을까?

[Kernel#Integer](http://ruby-doc.org/core-2.2.0/Kernel.html#method-i-Integer)를 사용하면 된다.

~~~maya:view
file=str_to_int_fail_alt.rb
~~~

~~~maya:execute
cmd=ruby str_to_int_fail_alt.rb
attach_cmd=true
~~~

~~~maya:view
file=null_to_int_alt.rb
~~~

~~~maya:execute
cmd=ruby null_to_int_alt.rb
attach_cmd=true
~~~

비슷한 목적으로 [Kernel#Float](http://ruby-doc.org/core-2.2.0/Kernel.html#method-i-Float)도 있다.
Kernel 클래스에는 유용한 기능이 많으니 한번쯤 읽어두면 나중에 도움이 될거다.
(sprintf, rand, sleep 조차 Kernel 클래스에 포함되어있다!)

반대로 파이썬에서 루비처럼 예외없이 정수로 바꾸고 싶으면 예외처리를 포함한 간단한 함수를 만들면 된다.
자세한 설명은 생략한다.

## Summary
문자열을 숫자로 바꾸는 작업은 단순한 기능이다.
하지만 모든 언어에서 같은 행동을 하도록 구현되진 않았다.
나중에 삽질하고 싶지 않다면 자신이 쓰는 언어에서는 어떤식으로 행동하는지 기억하자.

## Test Code
* [repo](https://github.com/if1live/libsora.so/tree/master/content/development/make-int-from-invalid-value-python-and-ruby)
