---
title: 파이썬 엑셀 쓰기 라이브러리 비교
tags: [python, library, excel, xlsx]
slug: python-excel-library
author: if1live
date: 2014-02-23
url: /posts/python-excel-library
---
subtitle: 무엇을 골라야 후회하지 않는가?

리눅스에서 굴러가는 파이썬 기반의 통계 도구를 만든적이 있다. 리눅스에서도 굴러가는 엑셀 라이브러리를 몇개 찾아봤고 직접 써본 다음에 정리했다.
찾아본 엑셀 라이브러리는 다음과 같다.

* [xlwt][xlwt]
* [OpenPyXL][openpyxl]
* [XlsxWriter][xlsxwriter]
* [PyExcelerate][pyexcelerate]

이중에서 내가 직접 사용해본것은 xlwt, OpenPyXL, XlsxWriter이다. (사용해본 순서로 적었다.)

## supoort xlsx

xls와 xlsx의 가장 큰 차이는 행, 열의 최대 갯수가 65535라는거다. 몇만건의 데이터를 다룰 가능성이 있다면 xlsx 지원하는 라이브러리를 사용해야 나중에 또 짜는 삽질은 안한다.

Library        | support xlsx
---------------|--------------
xlwt           | x
OpenPyXL       | o
XlsxWriter     | o
PyExcelerate   | o

xlwt만 지원하지 않는다. python excel 이라고 치면 제일 먼저 나오고 도메인도 python-excel.org를 갖고있는 라이브러리지만 낚이지 말자.

## 문서화

엑셀을 단순히 데이터를 테이블로 표시하기 위해서 사용할수도 있지만 상황에 따라서는 그것보다 더 많은 기능이 필요하다. 예를 들면 특정 행에 색깔을 넣고 싶다거나 글자 서식을 바꾸고 싶을수 있다. 또는 차트를 넣어야 될 수도 있다. 이런 추가기능을 제대로 쓰기 위해서는 라이브러리에서 어떤식으로 지원하는지 확인하기 위해서 문서를 찾아볼 필요가 있다. 즉, 문서가 없다면 아무리 라이브러리가 좋아봤자 제대로 쓸 수 없다는거다.

xlwt의 경우는 [문서][xlwt-doc]가 한없이 부실하다. 처음봤을 때는 문서가 문서인줄도 몰랐다. 대신 [예제코드][xlwt-example]를 제공한다. 샘플코드를 쭉 읽어보면 어떻게 쓰는 라이브러리 인지는 파악할 수 있다.

OpenPyXL은 xlwt보다는 [문서][openpyxl]를 잘 썻다. API 함수 설명은 좀 부실하지만 라이브러리 기본 사용 예제가 문서에 있어서 읽어보면 쓰는데는 문제 없다.

XlsxWriter는 [문서][xlsxwriter-doc]를 제일 잘 썻다. 라이브러리 사용 예제 코드도 충분히 많고 API 함수의 설명도 자세하게 써놧다. 게다가 해당 함수를 쓰면 어떤식으로 엑셀파일에 보이는지 친절하게 스크린샷까지 찍놨다(!)

PyExcelerate는 [문서][pyexcelerate]가 부실하다. ```README.md```가 문서의 전부다. 게다가 xlwt처럼 샘플코드를 제공하는것도 없다. 데이터를 엑셀파일에 쓰는것만 사용하면 별 문제가 없겠지만 그 이상의 기능을 쓰고싶으면 라이브러리 코드를 읽으면서 사용해야될것이다.

문서화 수준으로 점수를 매기면 다음과 같다.

1. XlsxWriter
2. OpenPyXL
3. xlwt
4. PyExcelerate

## 성능

테스트하기 귀찮은 관계로 PyExcelerate의 벤치마크 결과를 갖다쓴다.

```
TEST_NAME, NUM_ROWS, NUM_COLS, TIME_IN_SECONDS

pyexcelerate value fastest, 1000, 100, 0.47
pyexcelerate value faster, 1000, 100, 0.51
pyexcelerate value fast, 1000, 100, 1.53
xlsxwriter value, 1000, 100, 0.84
openpyxl, 1000, 100, 2.74
pyexcelerate style cheating, 1000, 100, 1.23
pyexcelerate style fastest, 1000, 100, 2.4
pyexcelerate style faster, 1000, 100, 2.75
pyexcelerate style fast, 1000, 100, 6.15
xlsxwriter style cheating, 1000, 100, 1.21
xlsxwriter style, 1000, 100, 4.85
openpyxl, 1000, 100, 6.32

* cheating refers to pregeneration of styles
```

성능은 PyExcelerate가 제일 빠르다. 다음은 XlsxWriter다. OpenPyXL이 제일 느리다. xlwt의 경우는 벤치마크에 포함되지 않았다.

xlwt의 경우는 xlsx를 지원하지 못한다. 즉, xlwt를 사용해서 대용량 데이터를 처리한다는 것은 말이 안된다. 그렇기 때문에 xlwt는 성능이 그렇게 치명적이지는 않다. (그리고 직접 써보면 그렇게 빠른편은 아니다)

## 나는 어떻게 사용했는가?

엑셀파일을 처음 사용해야되는 일이 생겼을때 xlwt를 사용했다. 구글검색을 돌리면 제일 먼저 뜨기 때문이다. 처음에는 끽해야 1000행 정도 되는 데이터를 다뤄서 xlwt로도 별 문제없이 사용했다.

프로젝트가 어느정도 진행되니까 8만행 정도 되는 통계 자료를 뽑아야되는 일이 생겼다. xlwt로는 xlsx가 안된다는걸 알고 버렸다. [파이썬에서 엑셀 파일 다루기…](http://www.jiniya.net/wp/archives/7692)를 읽어서 openpyxl를 선택했다. (리눅스 서버에서 굴러가는 함수라서 COM은 선택할수 없었다)

근데 openpyxl로 몇만행씩 써대니까 속도가 많이 느리더라. 당연히 계산관련 기능이 느릴거라고 생각해서 열심히 최적화 했는데 아무것도 달라진게 없더라. 성능 측정을 다시 해보고 OpenPyXL이 그냥 느리다는걸 깨달았다.

openpyxl의 대체용 라이브러리를 찾다가 XlsxWriter와 PyExcelerate를 발견다. 성능보다 문서가 중요하다고 판단해서 XlsxWriter를 선택했고 지금까지 쓰고 있다. 현재까지는 XlsxWriter가 그렇게 느리다고 느껴지지 않고 문서가 좋아서 개발할때 만족하면서 사용하고 있다.


## 정리

문서가 중요하면 [XlsxWriter][xlsxwriter]를 추천한다. 성능이 중요하면 [PyExcelerate][pyexcelerate]를 추천한다. xlsx 파일 지원이 필요하면 [xlwt][xlwt]는 고르지 마라.

나는 [XlsxWriter][xlsxwriter]를 쓴다. 어지간하면 무난한 선택이다.


[xlwt]: http://www.python-excel.org/
[xlwt-doc]: https://secure.simplistix.co.uk/svn/xlwt/trunk/xlwt/doc/xlwt.html?p=4966
[xlwt-example]: https://github.com/python-excel/xlwt/tree/master/xlwt/examples
[openpyxl]: http://openpyxl.readthedocs.org/en/latest/
[xlsxwriter]: https://github.com/jmcnamara/XlsxWriter
[xlsxwriter-doc]: https://xlsxwriter.readthedocs.org/
[pyexcelerate]: https://github.com/kz26/PyExcelerate
