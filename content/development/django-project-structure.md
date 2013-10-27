Title: Django Project 구조 예제
Tags: django, project, python
Slug: django-project-structure
Author: if1live
Date: 2013-01-30

최근에 Django로 코딩한적이 있다. django는 기본적으로 프로젝트 트리에 적절한 제약이 걸려있기 떄문에 굳이 신경안쓰고 django 가이드라인에서 제공하는대로 짜도 별 문제는 없을것이다. django 에서 제시하는 가이드라인에 일부 참고자료를 덧붙여서 내 나름대로 사용했던 프로젝트 구조를 정리해봤다. 일단 목표한 요구사항은 다음과 같다.

* 외부라이브러리를 막 집어넣어도 문제가 생기지 않을것
* 템플릿을 한곳에 모아서 관리하기. 템플릿 파일은 개발자가 아니라 디자이너가 편집하게 될 파일이니까 따로 모아놓는쪽이 관리하기 편할것이다.

## Django Project Structure

```
ROOT
├── apps
│   ├── __init__.py
│   ├── common
│   │   ├── __init__.py
│   │   ├── http.py
│   │   ├── mobile.py
│   │   ├── orm.py
│   │   ├── testlibs.py
│   │   └── ...
│   ├── like
│   │   ├── __init__.py
│   │   ├── admin.py
│   │   ├── models.py
│   │   ├── views.py
│   │   ├── urls.py
│   │   └── tests.py
│   └── ....
├── libs
│   ├── __init__.py
│   └── <some_library>
├── templates
│   ├── css
│   └── js
├── static
├── <proj_name>
│   ├── __init__.py
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
├── manage.py
└── requirements.txt
```

## 기본 설명
### apps
내가 만든 Application은 여기에 집어넣는다. apps 자체를 하나의 모듈로 만들어서 다른 프로젝트에서도 갖다쓸수있도록 했다. (그래서 ```ROOT/apps/__init__.py``` 가 존재한다) 프로젝트 내부에서 apps안의 모듈을 참고할때는 다음과 같이 썻다.
```python
from apps.common import http, mobile
import apps.like.constants
'''
```
내가 만든 라이브러리는 apps라는 모듈 안에 들어가기 떄문에 외부 라이브러리와의 이름 충돌 가능성은 존재하지 않는다.

### apps/common
여러 어플리케이션에서 공유할 라이브러리의 모음을 여기에 집어넣었다. 

### libs
외부 라이브러리를 집어넣고 사용했다. path를 조정함으로써 라이브러리를 import할때 libs가 앞에 붙이 않더라도 작동할수 있도록 했다. (apps처럼 apps를 명시적으로 붙여야 import가 되도록한 경우, 대부분의 라이브러리에서 자기자신을 import못하는 불상사가 발생하더라)

아래의 코드를 ```settings.py```에다가 적절히 집어넣으면 ```manage.py```를 돌릴때 항상 실행되니까 이후에는 라이브러리 경로를 신경쓰지 않아도 된다.
```python
#libs를 통째로 라이브러리 패스로 설정하기
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"../libs")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)
```

## 사용후기
### template를 한폴더에 모아놓기
template은 디자이너가 자주 편집하는거니까 한 폴더에 격리해놓으면 유지보수하기 편할거야 라는 생각은 진행하면서 사라졌다. 템플릿은 디자이너만 편집한다고 하기 이전에 ```views.py```와 강결합이 될수밖에 없다. (```views.py```에서 넘긴 변수가 템플릿에 찍히니까) 그렇다면 굳이 ```views.py```와 템플릿을 다른 폴더에 배치함으로써 얻는 이득이 존재하는가? 별로 없는거같다.

### apps의 존재의미?
내가 만든 어플리케이션은 apps 모듈안에 들어가기 때문에 외부 라이브러리와의 이름충돌은 발생하지 않는다. 다만, 이것때문에 다른곳에서 import해서 사용할때 apps를 붙여줘야한다. 일일이 apps를 붙여주는 대가를 치르면서 이름충돌의 가능성을 제거했는데 그렇다면 실제로 이름 충돌이 발생할 가능성이 있었는가? 

작업하고 나서 보니, 내가 사용한 라이브러리 중에서는 직접 만든 어플리케이션과 이름 충돌이 발생한것이 없었다. 자작 어플리케이션ㅇ느 타자치기 귀찮으니까 대부분이 한단어로 구성된다. 외부 라이브러리의 경우는 혹시나 이름충돌의 가능성이 있을까봐 한단어로는 잘 짓지 않는거같더라.(적어도 prefix는 붙여주더라) 결과적으로 어차피 이름충돌이 발생하지 않았으니 굳이 apps 모듈로 묶을 이유도 없게되었다.

### libs의 존재의미
```settings.py```에서 ```libs```가 path상에서 먼저 나오게 등록을 해주기 때문에 같은 이름의 라이브러리가 이미 컴퓨터에 깔려있는 상태에서 libs에 똑같은 라이브러리를 집어넣으면 libs 안에 존재하는 라이브러리가 로딩된다. 덕분에 외부라이브러리를 약간 수정해서 사용해야되는 경우 편했다.

## Reference
* [Django Project Conventions](http://blog.zacharyvoase.com/2010/02/03/django-project-conventions/)
* [Django Project Structure](http://www.deploydjango.com/django_project_structure/index.html)
