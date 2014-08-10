importd - Django as a micro-framework
=========================================

:tags: importd, django, web
:slug: introduce-importd
:author: if1live
:subtitle: d is for django
:date: 2014-08-11


What is importd?
##################

importd_(importd-repo_) 는 `Amit Upadhyay`__ 라는 사람이 만든 django micro framework이다.
우리나라에는 거의 알려져있지 않은데 `@cedar101`__ 님의 소개로 알게 되어서 사용해봤다.
직접 쓰면서 느낀 importd의 특징을 정리해봤다.

micro-framework
--------------------

*importd Hello World*

.. code:: python

  from importd import d

  @d("/")
  def index(request):
      return d.HttpResponse("Hello World")

  if __name__ == "__main__":
      d.main()

*flask Hello World*

.. code:: python

  from flask import Flask
  app = Flask(__name__)

  @app.route("/")
  def hello():
      return "Hello World!"

  if __name__ == "__main__":
      app.run()

위의 코드는 importd와 flask_ 로 작성한 hello world이다.
소스파일 하나로 서버를 띄울 수 있다. 원본 django에 비하면 매우 단순하다.
``urls.py``, ``views.py`` 로 view가 분리되서 분석하기 어려운 django에 비하면
importd는 flask만큼 흐름을 이해하기 쉽다.


Full compatibility with django
----------------------------------
django 기반으로 만들었기 때문에
django의 기능과 django용 수많은 라이브러리를 거의 그대로 사용할 수 있다.
importd와 django 라이브러리가 충돌해서 importd를 수정해야되는 일이 있을수도 있지만 붙일수있는게 어디냐?



smarurls
----------

django은 URL 분기를 urls.py에 정규식으로 작성한다. 개인적으로는 django의 이런 설계를 싫어한다.

첫번째로 마음에 안드는건 정규식을 쓴다는거다. 정규식은 좀 복잡해지면 내가 이해를 못하겠다.
내 머리로는 아래의 정규식이 어떤 문자열을 받아들이는지 한번에 이해하기 어렵더라.
URL 규칙을 표현하기 위한 특별한 문법을 만들고 그것으로 URL을 나타내면 이해하기 쉽지 않을까?

``^articles/(?P<year>[0-9]{4})/(?P<month>[0-9]{2})/(?P<day>[0-9]{2})/$``

두번째로 마음에 안드는건 ``views.py`` 와 ``urls.py``를 다른 파일에 작성한다는 점이다.
``/articles/2014/08/`` 라는 URL이 어떻게 행동하는지 알고 싶으면
``urls.py`` 에서 해당 URL을 잡아내는 정규식을 찾고
그 정규식에 연결된 함수를 ``views.py`` 에서 찾아야한다.
URL와 view함수를 한곳에 묶어놓으면 흐름을 이해하기 쉽지 않을까?

이 두가지 이유(+etc)때문에 2년 전에는 django대신 flask를 골랐다.
하지만 importd는 위의 문제를 해결했다.

.. code:: python

  @d("/user/<int:userid>/", name="user")
  def user(request, userid):
      return "user.html", {"userid": userid}

  @d("/book/<slug:book_name>")
  def show_book(request, book_name):
      ....


RESTful API
------------------

django에서 RESTful API를 만들어야하면 django-rest-framework_ 가 좋은 선택지 중 하나다.
django를 쓰면 RESTful API의 URL관련 내용은 ``urls.py``, 로직은 ``views.py``로 흩어진다.
하지만 importd_ 와 django-rest-framework_ 를 조합하면 아래같이 URL과 로직을 합칠수 있다.


.. code:: python

  @d('/v1/users/<int:pk>', name='v1.user_detail')
  @d('/v1/users/<int:pk>.<slug:format>', name='v1.user_detail')
  @api_view(['GET'])
  def user_detail(request, pk, format=None):
      pk = int(pk)
      ....


정리
#####

importd에 몇가지 라이브러리를 붙여서 실제로 굴러가는 예제 프로젝트를 작성했다.
importd로 새로운 프로젝트를 시작하기전에 참고하면 도움이 될것이다.

importd-boilerplate_ 에 포함된 라이브러리

* importd_
* `Jinja2 <http://jinja.pocoo.org/>`_ + `django-jinja <https://github.com/niwibe/django-jinja>`_
* `Werkzeug <http://werkzeug.pocoo.org/>`_ + django-extensions
* `django-debug-toolbar <http://django-debug-toolbar.readthedocs.org/en/1.2/>`_
* django-rest-framework_


.. _flask: http://flask.pocoo.org/
.. _amitu: https://github.com/amitu
.. _importd: http://amitu.com/importd/
.. _importd-repo: https://github.com/amitu/importd
.. _importd-boilerplate: https://github.com/if1live/importd-boilerplate
.. _django-rest-framework: http://www.django-rest-framework.org/
.. _cedar101-twitter: https://twitter.com/cedar101
.. _smarturls: http://amitu.com/smarturls/

__ armitu_
__ cedar101-twitter_
