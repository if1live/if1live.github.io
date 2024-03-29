---
title: 아름다운 PHP array_search()
subtitle: 잘못된 API 설계를 통해 알아보는 여러가지 대안
tags: [array_search, api, api-design]
slug: alternatives-to-php-array-search
author: if1live
date: 2015-11-08
url: /posts/alternatives-to-php-array-search
---
개요
----

```
떡밥 투척
boolean 검사코드
나는 success==false 파벌
```

<https://twitter.com/if1live/status/662925445990498304>

집에서 뒹굴다가 뜬금없는 떡밥이 생각나서 트위터에 투표를 던졌다. 투표를
던지고 보니 생각보다 많은 사람들이 `if(!success)` 를 선호하더라. 나같은
`success==false` 파벌이 얼마 없는걸 보고 왜 내가 `success==false` 파벌이
되었나 생각을 했다. 그리고 원인을 찾게 되었는데....

PHP array_search()
-------------------

PHP로 간단한 함수를 짜보자. 한자리수 정수를 넣으면 소수인지 아닌지
확인하는 함수이다. 제대로 구현하기 매우 귀찮았기 때문에 소수 목록을
하드코딩했다. 이 함수를 구현하기 위해서 PHP에서 제공하는
`array_search()` 를 이용했다. 그리고 함수가 잘 돌아가나 테스트해보자.

```php
<?php
function check_prime_number_buggy($val) {
  $data = array(2, 3, 5, 7, 11);
  $found = array_search($val, $data);
  if($found) {
    printf("%d is prime number, index=%d\n", $val, $found);
  } else {
    printf("%d is not prime number\n", $val);
  }
}

// normal case
check_prime_number_buggy(5);
// first element
check_prime_number_buggy(2);
// not exist
check_prime_number_buggy(-1);

?>
```

```bash
$ php buggy.php
5 is prime number, index=2
2 is not prime number
-1 is not prime number
```

2는 소수 목록에 있는데도 소수가 아닌거로 처리되었다. 원인은 php
`array_search()` 함수의 잘못된 설계에 있다. 함수의 설명을 뒤지면 다음과
같은 항목이 있다.

<http://php.net/manual/en/function.array-search.php>

```
Returns the key for needle if it is found in the array, FALSE otherwise.
```

배열에서 첫번째 요소를 찾으면 리턴값이 0이다. 배열에서 찾지 못하면
리턴값이 FALSE이다. 근데 0과 FALSE는 모두 if 안에서 false로 처리된다.
그래서 첫번째 요소를 찾았음에도 잘못된 코드가 실행된다.

PHP `array_search()` 함수가 잘못 만들어졌다고 생각하는건 나 혼자가
아니다. [PHP: 잘못된 디자인의
프랙탈](http://noraesae.github.io/PHP-a-fractal-of-bad-design-kr/)

> PHP 코드 중 일부는 버그를 발생시키도록 설계되었습니다.
> array_search, strpos 함수는 문자열의 처음 위치에서 찾으면 0을 반환합니다. 하지만 찾지 못한다면 false를 반환합니다.
>
> C 언어에서는 strpos 같은 함수는 서브스트링을 찾지 못하면 -1을 반환합니다.
> 만약 그런 경우를 체크하지 않는다면 당신은 엉뚱한 메모리 영역을 가리키게 될 것이고 프로그램은 터질 것입니다.
> (아니, 아마도요. 이건 C 언어잖아요. 씨발 대체 누가 알겠냐고. 하지만 최소한 이런 경우를 위한 도구는 있죠.)
>
> 이를테면 파이썬에서 비슷한 역할을 하는 .index 메소드는 찾지 못했을 경우 예외를 발생시킵니다.
> 역시 그런 경우를 체크하지 않으면 프로그램은 터지게 되겠죠.
>
> PHP에서 이러한 함수들은 false를 반환합니다.
> 만약 FALSE를 배열 인덱스값으로 사용하거나 ===로 비교하지 않고 이런 저런 짓을 할 경우에 PHP는 false 값을 0으로 알아서 변환해 버립니다.
> 그럼 당신의 프로그램은 터지지 않겠죠.
> 대신 당신의 프로그램은 strpos 함수를 사용할 때마다 결과값을 일일히 체크하는 보일러플레이트 코드를 넣지 않는 한 일말의 경고도 없이 엉뚱한 짓을 하게 됩니다.
>
> 이건 정말 나쁜 겁니다! 프로그래밍 언어는 나와 같이 일을 하기 위해서 만들어진 도구잖아요.
> 여기, PHP는 개발자가 빠지라고 스스로 찾기 힘든 곳에다가 함정을 파 버립니다.
> 그리고 저는 문자열 처리나 결과값이 같은지 비교하는 등의 따분한 작업을 하면서도 신경을 곤두세워야 합니다. PHP는 그야말로 지뢰밭입니다.

```php
<?php
function check_prime_number_correct($val) {
  $data = array(2, 3, 5, 7, 11);
  $found = array_search($val, $data);

  // do not use implicit comparison
  if($found !== false) {
    printf("%d is prime number, index=%d\n", $val, $found);
  } else {
    printf("%d is not prime number\n", $val);
  }
}

// normal case
check_prime_number_correct(5);
// first element
check_prime_number_correct(2);
// not exist
check_prime_number_correct(-1);

?>
```

```bash
$ php correct.php
5 is prime number, index=2
2 is prime number, index=0
-1 is not prime number
```

최소한의 수정으로 버그없이 의도한대로 작동하게 하려면 `array_search()`
의 리턴값과 false를 직접 비교해야 한다. 이것 이외의 다른 접근법은 뭐가
있을까? 다른 언어였으면 어떤 식으로 `array_search()` 함수를
디자인했을까?

대안. 존재 확인과 인덱스 확인을 분리
------------------------------------

하나의 함수에서 0과 FALSE가 리턴되기 때문에 문제가 생긴다면 0과 FALSE가
같이 리턴될 가능성을 없애는거다. 우선 배열에 값이 있는지 확인한다. 있을
경우에 한해서만 인덱스를 확인한다. 무식한 방법이지만 PHP
`array_search()` 의 리턴값을 정확히 기억하고 있지 않은 유지보수 개발자가
사고칠 가능성을 낮출 수 있다.

```php
<?php
function check_prime_number($val) {
  $data = array(2, 3, 5, 7, 11);
  if(in_array($val, $data) === false) {
    printf("%d is not prime number\n", $val);
  } else {
    $found = array_search($val, $data);
    printf("%d is prime number, index=%d\n", $val, $found);
  }
}

check_prime_number(5);
check_prime_number(2);
check_prime_number(-1);
```

대안. 예외를 사용
-----------------

파이썬의 경우는 예외를 던진다. 배열에 값이 없을 경우 예외에 의해 다른
실행 흐름을 따라가니까 문제를 피할 수 있다.

```python
#!/usr/bin/env python

def check_prime_number(val):
    data = [2, 3, 5, 7, 11]
    try:
        found = data.index(val)
        print("%d is prime number, index=%d" % (val, found))
    except ValueError:
        print("%s is not prime number" % (val,))

check_prime_number(5)
check_prime_number(2)
check_prime_number(-1)
```

대안. 없으면 -1
---------------

자바스크립트, 자바의 경우는 찾지 못했을때 -1을 반환한다. `found >= 0` 을
이용해서 존재 여부를 확인할수 있다.

```js
function check_prime_number(val) {
  var data = [2, 3, 5, 7, 11];
  var found = data.indexOf(val);
  if(found >= 0) {
    console.log(`${val} is prime number, index=${found}`);
  } else {
    // not found = -1
    console.log(`${val} is not prime number`);
  }
}

check_prime_number(5);
check_prime_number(2);
check_prime_number(-1);
```

하지만 `if(found)` 같이 암묵적으로 비교하다 사고칠 가능성에서 완전히
자유롭진 않다. `-1` 은 if 안에서 true로 취급되기 때문이다.

```js
function check_prime_number_buggy(val) {
  var data = [2, 3, 5, 7, 11];
  var found = data.indexOf(val);
  if(found) {
    console.log(`${val} is prime number, index=${found}`);
  } else {
    console.log(`${val} is not prime number`);
  }
}

check_prime_number_buggy(5);
check_prime_number_buggy(2);
check_prime_number_buggy(-1);
```

```bash
$ node buggy_invalid_index.js
5 is prime number, index=2
2 is not prime number
-1 is prime number, index=-1
```

대안. iterator
--------------

C++ STL에는 요소의 인덱스를 찾는 함수가 없다. 이쪽 세계에서는 iterator로
모든 작업을 처리한다. `std::find()` 로 항목을 찾은후 `std::distance()`
로 인덱스를 계산할 수 있다. 항목을 찾지 못한 경우 `last` 를 리턴한다.
덕분에 리턴값 하나만으로 배열에 요소가 존재하는지, 몇번째 요소인지도
확인할 수 있다. 기본 개념은 `in_array()`, `array_search()` 를 같이
사용한 것과 비슷하지만 더 효율적이다.

```cpp
#include <algorithm>
#include <cstdio>

void check_prime_number(int val)
{
  auto data = {2, 3, 5, 7, 11};
  auto found = std::find(data.begin(), data.end(), val);
  if(found == data.end()) {
    printf("%d is not prime number\n", val);
  } else {
    int idx = std::distance(data.begin(), found);
    printf("%d is prime number, index=%d\n", val, idx);
  }
}

int main()
{
  check_prime_number(5);
  check_prime_number(2);
  check_prime_number(-1);
  return 0;
}
```

대안. 0 != false
----------------

위에서 나온 대안은 간단하거나 `PHP: 잘못된 디자인의 프랙탈` 에서 이미
다룬 내용이다. PHP: 잘못된 디자인의 프랙탈 에서 이미 다룬 내용을 가지고
글을 쓰면 재미없다. 뭐라도 하나쯤은 다른 대안이 있어야하지 않겠는가?

루비가 어떻게 문제를 피했는지 확인해보자.

```python
if 0:
    print("access")
```

```ruby
if 0
  puts "access"
end
```

똑같이 생긴 파이썬과 루비 코드이다. if안에 0이 들어갔을때 코드가
실행될까? 겉보기에는 똑같이 생겼지만 결과도 똑같진 않다.

파이썬의 경우는 출력이 없다. 0은 False와 동일한 값으로 취급된다. 이는
파이썬을 포함한 여러 언어에서 볼 수 있는 방식이다. 루비의 경우는
`access` 가 출력된다. 루비에서는 0과 False는 다른 값으로 취급된다.
if안에서의 0은 True이다.

Ruby `#index` 함수는 배열에 항목이 있으면 인덱스를 리턴한다. 만약 항목이
없으면 nil을 리턴한다.

항목을 찾아서 인덱스를 리턴했으면 0보다 크거나 같을것이고 이는 if 안에서
True로 취급된다. 찾지 못했으면 nil을 리턴하는데 이는 if 안에서 False로
취급된다. 그래서 버그가 있는 php와 동일한 구조의 코드임에도 루비에서는
버그가 없다.

```ruby
#!/usr/bin/env ruby

def check_prime_number(val)
  data = [2, 3, 5, 7, 11];
  found = data.index(val)
  if found
    puts "#{val} is prime number, index=#{found}"
  else
    puts "#{val} is not prime number"
  end
end

check_prime_number 5
check_prime_number 2
check_prime_number -1
```

Summary
-------

-   쿨타임 찼다 PHP를 까자.
-   PHP 개발자라면 [PHP: 잘못된 디자인의
    프랙탈](http://noraesae.github.io/PHP-a-fractal-of-bad-design-kr/)
    는 한번쯤 읽어보자.
-   여러 언어, 라이브러리를 구경하면 여러가지 해결책을 찾을수 있다.
    패러다임이 같은 언어라고 하나만 알고 있을 필요는 없다.
-   루비를 구경해보자. 주력 언어로 쓰지 않더라고 괴랄한 아이디어를 많이
    배울 수 있다.

Test code
---------

[Repo](https://github.com/if1live/libsora.so/tree/master/content/development/alternatives-to-php-array-search)
