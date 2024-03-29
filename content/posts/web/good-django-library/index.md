---
title: Django용 적절한 라이브러리 소개
subtitle: 쉽게 붙일 수 있는 적절한 라이브러리
tags: [django, library]
slug: good-django-library
author: if1live
date: 2014-08-10
url: /posts/good-django-library
---

2년 전, 신규 웹 프로젝트를 django로 시작했지만 django의 몇가지 문제
때문에 flask로 갈아탔다. 프로젝트 시작한지 얼마되지 않은 시점이어서
코드가 별로 없었기 때문에 프레임웍을 바꾸는것이 별 부담이 되지 않았고
django에서 어떻게든 문제를 해결하려고 삽질할 수도 있었겠지만 flask로
선택하면 바로 해결되기 때문에 삽질하기 귀찮아서 django를 버렸다.

지금 시점에서 django와 flask 중 하나를 선택하라고 어떻게 할까? 지금은
django로 여러가지 삽질을 해서 과거에 해결하지 못했던 문제를 해결할 수
있는 라이브러리를 알고 있다. 그래서 이번 기회에 내가 알고있는 django를
flask만큼 좋게 만드는 몇가지 라이브러리를 소개한다.

## URL Routing

Django의 urls.py 는 정규식을 이용해서 URL Routing을 설정한다. 근데
정규식이라는게 사람이 읽기 좋은 물건이던가? 아래는 `/year/2014/may/` 와
같이 연도/월을 표현한 URL을 잡아내는 표현이다.

* `^year/(?P<year>\d{4,4})/(?P<month>\w+)/$`
* `/year/<int4:year>/<word:month>`

정규식으로 표현된 위의 표현보다 아래쪽이 더 읽기 쉽다. (아래의 문법은 flask에서 사용하는 방식이다.
[flask-url-route](http://flask.pocoo.org/docs/api/#url-route-registrations))

아래와 같은 표현을 위의 정규식으로 바꿀수 있으면 URL 관리가 더 편하지 않을까?
이때 사용할수 있는 라이브러리가
[smarturls](http://amitu.com/smarturls/) 이다.

```python
# Django default URL
urlpatterns = patterns('',
    ('^year/(?P<year>\d{4,4})/(?P<month>\w+)/$', 'month.view'),
)

# smarturls
urlpatterns = patterns('',
    surl('/year/<int4:year>/<word:month>/', 'month.view'),
)
```

smarturls은 기존 django프로젝트에 끼워넣어도 문제가 생기지 않는다는 점이
좋다.

## Debugger

![image](twdp_0804.png)

Django 개발 도중에 에러가 발생하면 위와 같은 화면을 자주 볼 것이다.
문제가 몇번째 줄에서 어떤식으로 발생했는지를 볼 수 있다. 이것만 있어도
php보다는 정신건강에 좋다. 만약 에러난 지점에서 pdb가 돌아가서 변수값을
직접 확인해볼수 있으면 더 좋지 않을까?

![image](debug-screenshot.png)

위의 화면은 werkzeug의 디버거이다. 문제가 발생한 지점에서 shell을 띄워서
변수값을 직접 확인할수도 있고 함수도 호출할 수 있다. flask의 경우는
werkzeug를 이용하기 때문에 위의 디버거를 그냥 쓸수있지만 django는 그렇지
않다.

하지만 추가 라이브러리를 쓰면 django에서도 이 좋은 디버거를 쓸 수 있다.
[django-extensions](https://github.com/django-extensions/django-extensions)
과 [werkzeug](http://werkzeug.pocoo.org/) 를 깔면 django에서 werkzeug를
쓸 수 있다. 설치한 다음에 `runserver` 대신 `runserver_plus` 를 쓰면
된다. 기존 django 프로젝트에 끼워넣어도 문제없이 작동한다.

## Jinja2

[jinja2](http://jinja.pocoo.org/docs/) 는 매우 적절한 템플릿 엔진이다.
django 내장 템플릿 엔진보다 훨씬 좋다. 내가 django 템플릿을 버리고
jinja2로 갈아타도록 만든 특징은 템플릿에서 함수 호출이 가능하다는 것과
필터에 인자를 여러개 넘기는 것이 가능하다는 점이다.

```html
<p>{{ foo.bar(1, 'asd') }}</p>
<p>{{ "Hello World"|replace("Hello", "Goodbye") }}</p>
```

django에서 템플릿 엔진을 [jinja2](http://jinja.pocoo.org/docs/) 로
바꿔끼우는 직접 사용해본것이 2가지이다.

* [django-jinja](https://github.com/niwibe/django-jinja)
* [coffin](https://github.com/coffin/coffin)

[coffin](https://github.com/coffin/coffin) 의 경우 기존 django 템플릿과
섞어서 사용가능하다. 확장자가 .html이면 django template으로 취급하고
.jinja2이면 jinja2 template로 취급하는 식이다. 그래서 기존의 django
프로젝트에 도입해도 문제가 생기지 않는다. (기본 템플릿이 2벌 되는건 다른
이야기) (django-jinja도 django template과 섞어서 작동하는지는
테스트해보지 않았다. 하지만 아마도 되겠지)

## 정리

2년전에 django를 버리고 flask로 갈아탄 이유는 다음과 같다.

* django template보다 jinja2가 좋더라
* django의 urls.py는 이해하기 어렵다. flask와 같은 url routing이 마음에 든다
* 승리의 Debugger!
* django ORM보다 sqlalchemy가 마음에 든다. 그런데 플젝 하나에 ORM 2개 탑재하는건 좀 그렇잖아?

하지만 지금 과거와 같은 상황이라면 어떤 선택을 할까? 지금이라면 django를
버리지 않을거다.

* jinja2 : [coffin](https://github.com/coffin/coffin), [django-jinja](https://github.com/niwibe/django-jinja)
* URL Routing: [smarturls](http://amitu.com/smarturls/)
* Debugger: [django-extensions](https://github.com/django-extensions/django-extensions)
* ORM : flask+sqlalchemy = 로직 코드 / django ORM = django admin. 하나의 프로젝트, 2개의 ORM.

p.s.: 위의 라이브러리와 [importd](http://amitu.com/importd/) 를 합쳐서 간단한 예제 프로젝트를 만들었다.
[importd-boilerplate](https://github.com/if1live/importd-boilerplate)
당분간은 웹질할 건수가 생기면 이거 기반으로 작업할듯.
