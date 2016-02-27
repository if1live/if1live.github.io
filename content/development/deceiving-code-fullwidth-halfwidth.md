Title: 코드에 반각/전각문자 넣어서 낚시하기
Subtitle: 반각/전각 문자로 알아보는 고난도 낚시
Tags: 전각, 반각, halfwidth, fullwidth, 낚시
Slug: deceiving-code-fullwidth-halfwidth
Author: if1live
Date: 2015-04-27

## 개요

> 프로그래머 머리 터지게 하는 방법
> 작업중인 코드를 몰래 열어 세미콜론(;) 하나를 그리스어 물음표(;)로 바꾼다
> [tweet](https://twitter.com/hyekkim/status/591110501288968192)

세미콜론(;) 하나를 그리스어 물음표(;)로 바꾼다면 트윗대로 프로그래머를 엿먹일수 있다.
하지만 그리스어 물음표를 볼 일이 얼마나 있을까? 우리는 그리스어를 쓰지 않고 그리스 근처의 나라도 아닌데?
악의를 가지고 코드를 수정하지 않는 이상 위의 코드를 볼 일은 없다.
그렇다면 이건 어떨까?

> 프로그래머 머리 터지게 하는 방법
> 작업중인 코드의 정규식을 몰래 열어 반각 공백문자 2개(  )를 전각 공백문자 1개(　)로 바꾼다
> [tweet](https://twitter.com/if1live/status/591252041479008256)

## 반각/전각
여기에서는 간단하게 설명한다. 자세한 설명은 [위키피디아][wiki]를 참고.

![반각/전각]({attach}deceiving-code-fullwidth-halfwidth/KoreanDOSPrompt.png)

1. 일반적으로 사용하는 알파벳, 숫자는 가로폭이 좁다. 
2. 한글, 한자, 일본어는 가로폭 넓다.
3. 고정폭 폰트를 사용하면 화면에 보이는 글자가 격자에 딱 맞게 들어가야한다.
4. 한글 한글자의 가로폭을 알파벳 2글자 크기가 맞추면 되지 않을까?

## 실제 사례로 보는 버그

### 버그
[Happy Birthday, Waifu][birthday_service]를 만들때 낚인 사례이다.
레일즈를 공부할겸 캐릭터 생일을 보여주는 간단한 웹서비스를 만들었다.
개발 당시 캐릭터의 생일DB가 깨끗하게 정리된 곳을 찾지 못했다.
그래서 DB는 [Anime Characters Database][animecharacterdb]를 크롤링하기로 했다.
크롤러는 캐릭터 페이지를 파싱해서 필요한 정보(영어 이름, 일본어 이름, 생일 등)을 읽는다.

최초 개발시에 샘플 HTML로 [카스가노 소라 정보][character_sora]를 이용했다.
Anime Characters Database에 등록된 카스가노 소라의 일본어 이름은 `春日野 穹 （かすがの そら）`이다.

샘플 HTML이 한개쯤 더 있으면 좋을거 같아서 임의로 하나의 페이지를 선택했다.
[시라쿠사 이부키][character_ibuki]가 선택되었다.
DB에 등록된 시라쿠사 이부키의 일본어 이름은 `白草 いぶき （しらくさ いぶき）`이다.

DB에 등록된 카스가노 소라와 시라쿠사 이부키의 이름을 보니 다음과 같은 형식으로 DB에서 관리하는거 같더라.
`[한자 이름][공백][（][히라가나 이름][）]`
전각 문자를 어떻게 입력하는지 몰라서 카스가노 소라의 이름을 복사한 다음에 살짝 고쳐서 정규식을 만들었다.

```ruby
regexp = /(?<kanji>.+)（(?<kana>.+)）/
```

이후 가끔씩 시간나면 수동으로 크롤러 돌려보고 문제없는지 확인하고 캐릭터를 추가했다.
4월 3일까지 아이마스의 아마미 하루카를 추가해달라는 요청이 있어서 아이마스 캐릭터를 넣어보기로 했다.
그런데 파서가 뭔가 이상하게 작동했다. 이름 파싱이 제대로 안되더라. 
파싱 문제가 발생한건 [키사라기 치하야][character_chihaya]였다.
DB에 등록된 키사라기 치하야의 이름은 `如月千早 (きさらぎ　ちはや)` 였다.

겉보기에는 같은 형식처럼 생겼는데 정규식이 돌아가지 않는걸까?


### 원인

* 카스가노 소라의 일본 이름 `春日野 穹 （かすがの そら）`
* 시라쿠사 이부키의 일본 이름 : `白草 いぶき （しらくさ いぶき）`
* 키사라기 치하야의 일본 이름 : `如月千早 (きさらぎ　ちはや)`

| 소라 | 반각/전각 | 이부키 | 반각/전각 | 치하야 | 반각/전각 |
|------|-----------|--------|-----------|--------|-----------|
| 春日野 穹 | 전각+공백 | 白草 いぶき | 전각+공백 | 如月千早 | 전각(+공백) |
| (space) | 반각 | (space) | 반각 | (space) | 반각 |
|（ | **전각** |（ | **전각** | ( | **반각** |
| かすがの | 전각 | しらくさ | 전각 | きさらぎ | 전각 |
| (space) | **반각** | (space) | **반각** | (space='　') | **전각** |
| そら | 전각 | いぶき | 전각 | ちはや | 전각 |
| ） | **전각** |） | **전각** | ) | **반각** |


* 카스가노 소라와 시라쿠사 이부키의 규격은 같다. 적당히 고른 시라쿠사 이부키의 규격이 같을 줄이야.
* 소라의 괄호는 전각문자[（）]인데 치하야는 괄호가 반각문자[()]
* 소라의 히라가나 이름에는 반각 공백문자[ ]가 들어가는데 치하야의 히라가나 이름에는 전각 공백문자[　]가 들어감
* 겉보기에는 같은 규격처럼 보이지만 반각/전각 문자가 똑같지 않다.

## fullwidth to halfwidth

현실에서는 모든 전각문자를 반각문자로 바꿀 필요가 없다.
자주 쓰기는 특수문자(공백, [, ], +, -,...)가 전각문자일때 이를 반각문자로 바꿔도 위와 같은 버그는 피할 수 있을 것이다.
아스키 코드에 있을 정도로 뻔한 문자 한정으로 전각문자를 반각문자로 바꿔보자.

[ASCII Code][ascii]와 [전각문자 유니코드][wiki]를 비교하면 어떤식으로 전각문자를 반각문자로 교체할수 있는지 확인할 수 있다.
아스키 코드에서 0x20(space)부터 0x7E(~)는 유니코드의 U+FF00(　)부터 U+FF5E(～)에 같은 순서로 배치되어있다.
즉, 코드값만 바꾸면 순서대로 1:1 대응시킬수 있다. 다음은 이를 구현한 간단한 파이썬 코드이다.

```python
#-*- coding: utf-8 -*-

from __future__ import unicode_literals
from __future__ import print_function

def convert_full_to_half_char(ch):
    codeval = ord(ch)
    if 0xFF00 <= codeval <= 0xFF5E:
        ascii = codeval - 0xFF00 + 0x20;
        return unichr(ascii)
    else:
        return ch
    
def convert_full_to_half_string(line):
    output_list = [convert_full_to_half_char(x) for x in line]
    return ''.join(output_list)

input_str = """！＂＃＄％＆＇（）＊＋，－．／
０１２３４５６７８９：；＜＝＞？
＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ
ＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿
｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ
ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～"""
output_str = convert_full_to_half_string(input_str)
print(input_str)
print(output_str)

# Output
# python full_to_half.py 
# ！＂＃＄％＆＇（）＊＋，－．／
# ０１２３４５６７８９：；＜＝＞？
# ＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ
# ＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿
# ｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ
# ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～
# !"#$%&'()*+,-./
# 0123456789:;<=>?
# @ABCDEFGHIJKLMNO
# PQRSTUVWXYZ[\]^_
# `abcdefghijklmno
# pqrstuvwxyz{|}~
```

[C#의 경우는 전각문자열을 반각문자열로 바꿔주는 함수][stackoverflow_cs]가 있다고 한다. 
자신이 사용하는 언어에 유사한 함수가 있는지 먼저 찾아보자.


## 추가

일본에서는 전각문자를 상당히 많이 사용한다. 
일본 사이트를 돌아다니면서 자세히 보면 우리와 다른 특수문자를 쓰는걸 볼 수 있다. 
나처럼 일본 사이트 또는 일본과 관련된 자료를 파싱할 때는 낚이지 않게 조심하자.

* `!` != `！`
* `?` != `？`

일본에서는 반각/전각문자를 섞어쓰는게 일상이다보니 게임 금지어도 영향을 받았다.
예를 들어 반각문자 `sex`와 전각문자 `ｓｅｘ` 각각을 금지어로 추가하기도 한다.


[wiki]: http://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms
[ascii]: http://www.asciitable.com/

[animecharacterdb]: http://www.animecharactersdatabase.com/
[birthday_service]: http://birthday.libsora.so/
[character_sora]: http://www.animecharactersdatabase.com/character.php?id=13162
[character_chihaya]: http://www.animecharactersdatabase.com/character.php?id=70886
[character_ibuki]: http://www.animecharactersdatabase.com/character.php?id=62894

[stackoverflow_cs]: http://stackoverflow.com/questions/25149458/convert-fullwidth-to-halfwidth