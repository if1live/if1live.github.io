Title: Python 2.x 한글 인코딩 관련 정리
Tags: python, hangul, encoding
Slug: python-hangul
Author: if1live
Date: 2013-02-03

## 개요
파이썬은 **양놈**이 만든 언어라서 한글을 쓸때 골치아프다 (아니, 한글만 골치아픈게 지옥의 언어 **CJK**(chiness, japanese, korean)를 포함한 멀티바이트 언어는 전부 그렇겠지). 영어만 사용하면 별로 신경쓸게 없지만 한글은 ```euc-kr```, ```utf-8```이 공존하지 않는가?

그냥 골치아픈것도 아니고 unicode와 조합되서도 골치아프다. unicode관련 부분의 경우, 최초부터 지원한게 아니라 파이썬 2.4(엿던가?)부터 지원이 추가되어서 문자열을 의미하는 타입이 ```str```과 ```unicode```가 존재해서 더 골치아프다. 이런 문제는 파이썬 3.X로 가면 거의 없어지지만(그 동네는 unicode로 통일됏다고하더라.) 외부 라이브러리라든가 기타 사정상 파이썬 2.x를 써야되는 환경도 아직 많이 남아있어서 내가 삽질했던걸 한번 정리해봤다.

## python script에 한글 사용하기

```python
print '한글'
```
python 스크립트 파일에 위의 내용을 적고 실행해보자. 아래와 같은 에러가 나온다.

```
SyntaxError: Non-ASCII character '\xc7' in file euckr-error.py on line 1, but no
 encoding declared; see http://www.python.org/peps/pep-0263.html for details
```
파이썬의 기본 인코딩은 ```ascii```이다. 스크립트 파일이 ascii 인코딩일거라고 가정하고 읽어서 파싱할라고 하는데 ascii로 해석이 불가능한 **한글**이라는 단어가 존재하기 떄문에 SyntaxError(문법에러)가 발생한 것이다. 이를 해결하기 위해서는 아래와 같이 스크립트 파일 첫줄에 파일의 인코딩을 명시해주면 된다. 

```python
#-*- coding: utf-8 -*-
print '한글'
```
```python
#-*- coding: euc-kr -*-
print '한글'
```

## str - unicode 변환
```python
s = 'english'
print str(unicode(s))
```
위와 같이 영어 문자열을 Unicode로 바꿧다가 str로 바꾸는 코드를 실행하면 잘 돌아간다. 그러면 한글을 집어넣고 돌려보자.


```python
#-*- coding: utf-8 -*-
s = '한글'
print str(unicode(s))
```
```
    print str(unicode(s))
UnicodeDecodeError: 'ascii' codec can't decode byte 0xc7 in position 0: ordinal
not in range(128)
```
뒤진다. 훗. 그래야 한글답지.
에러를 보면 UnicodeDecodeError(유니코드 해석 에러)가 발생한다. **python script에 한글 사용하기**와는 다른 에러다. 이번에 발생한 에러를 해석하자면 '한글'을 ascii라고 간주하고 유니코드로 변환하려고 시도했으나 해석이 안되서 발생한 에러이다. 이를 해결하기 위해서는 아래의 소스와 같이 명시적으로 문자열을 utf-8로 간주하고 unicode로 변환하도록 할수있다.

```python
#-*- coding: utf-8 -*-
s = '한글'
print s.decode('utf-8').encode('utf-8')
```

하지만 위의 소스는 만능이 아니다. 외부 라이브러리를 만든 사람이 문자열을 유니코드로, 유니코드를 문자열로 변환할때 인코딩을 명시적으로 적지 않앗다고 가정하자. 양놈이 그런코딩을 했으면 자신의 환경에서는 문제없이 잘 작동한다. (아래코드가 예시)
```python
foo = unicode(s)
...
bar = str(foo)
```
하지만, 양놈이 만든 라이브러리를 우리가 가져다 사용하는데 함수의 인자로 한글을 사용하게 된다면 어떨까? 위에서 이야기한것과 같이 UnicodeDecodeError가 발생하고 망한다. 이를 해결하겟다고 라이브러리에서 유니코드 관련 인코딩/디코딩하는 지점에 전부 인코딩을 명시적으로 적어주는건 사람이 할 짓이 아니다. 만약 Unicode로 인코딩/디코딩할때 사용할 기본 인코딩을 ```ascii```대신 ```utf-8```로 바꿔주면 쉽게 해결할수 있지 않을까? ```sys.setdefaultencoding```가 바로 이럴 때 사용하는 함수이다.

```python
#-*- coding: utf-8 -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

s = '한글'
print str(unicode(s))
```
위와 같이 기본 인코딩을 ascii대신 utf-8로 바꿔주는 내용을 스크립트 파일을 최초 진입부에 집어넣으면 된다. ```#-*- coding: utf-8 -*-```과 달리 저것은 함수라서 호출된 이후, 계속 유지된다. 적절한곳에 한번만 넣어주면 된다.
참고로 ```reload(sys)```를 하지 않으면 ```sys.setdefaultencoding```함수를 사용할 수 없다. 

## Reference
* [Why we need sys.setdefaultencoding(“utf-8”) in a py script?](http://stackoverflow.com/questions/3828723/why-we-need-sys-setdefaultencodingutf-8-in-a-py-script)
* [파이썬 유니코드 하우투](http://coreapython.hosting.paran.com/hangul/Unicode%20HOWTO.htm)
* [파이썬 한글 인코딩에 대해](http://harebox.tistory.com/entry/%ED%8C%8C%EC%9D%B4%EC%8D%AC-%ED%95%9C%EA%B8%80-%EC%9D%B8%EC%BD%94%EB%94%A9%EC%97%90-%EB%8C%80%ED%95%B4)
