Title: Flask Project 구조 예제
Tags: flask, project, python
Slug: flask-project-structure
Author: if1live
Date: 2013-01-31

요즘에는 Flask를 주력으로 사용하고 있다. Flask는 가볍게 쓰는것을 목표로 설계된 라이브러리다 보니까 프로젝트 구조를 어떻게 잡을지에 대한 가이드라인조차 딱히 제시하지 않는다.(django에서는 프로젝트 트리를 미리 제시하기 떄문에 이런 고민을 굳이 깊게할 필요가 없다) 그래서 내가 요즘 사용하는 구조를 정리해봤다

## Flask Project Structure
```
ROOT
├── common
│   ├── __init__.py
│   ├── http.py
│   ├── mobile.py
│   ├── db.py
│   ├── testlibs.py
│   └── ...
├── like
│   ├── __init__.py
│   ├── controllers.py
│   ├── models.py
│   ├── views.py
│   ├── templates
│   │   └── like
│   │        ├── index.html
│   │        └── _likehelpers.html
│   └── ....
├── test
│   ├── like
│   │   ├── models_tests.py
│   │   └── controllers_tests.py
│   └── ....
├── static
│   ├── css
│   └── js
├── main.py
├── manager.py
├── settings.py
├── .pylintrc
└── requirements.txt
```

## 설명

### settings.py
프로젝트에서 사용되는 기본 설정이 있는 파일이다. DB접근 경로라든가, sentry dsn이라든가 디버그 옵션이라든가 등등을 여기에 집어넣었다

### main.py
Flask app이 존재하는 파일이다. 실행시킬때는 이것을 사용하면 된다

### manager.py
서버 실행과는 독립된 관리기능이 있는 명령행 스크립트 파일이다. 예를 들어, DB 테이블을 초기화 한다거나 우편번호를 파일에서 읽어서 db에 저장한다든가와 같은 기능을 여기에 구현했다. django의 ```manager.py```의 아이디어를 따서 만들었다

### common
프로젝트 전반적으로 쓰이는 공용 라이브러리는 여기에 전부 집어넣었다. 짜잘한 함수까지 모듈레벨로 올려버리면 모듈의 갯수가 너무 많아지는거같더라.

#### common/db.py
웹기반 프로젝트면 당연히 DB는 붙어야하지 않겠는가? DB와 관련된 기능을 여기에 집어넣었다. 나는 ORM으로 sqlalchemy를 쓰는데 이 파일에 관련 설정을 집어넣었다. sqlalchemy를 사용하면 자주쓰게되는 session객체와 Model 클래스를 여기에서 선언했다.
```python
import settings
from sqlalchemy import (
    create_engine,
    )
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import *
from sqlalchemy.orm import (
    sessionmaker,
    relationship
    )

engine = create_engine(settings.SQLALCHEMY_DB_URI, echo=False)
Model = declarative_base()

Session = sessionmaker(bind=engine)
session = Session()
```

그리고 다른 모듈에서 sqlalchemy가 필요할때는 아래와 같은 형태로 사용했다.
```python
from common import db
q = db.session.query(SomeModel).....
```

## test
유닛테스트를 각각의 모듈안에 집어넣지 않고, 독립된 디렉토리에 집어넣었다. django에서 제시하는 형태인 모듈과 같은 위치에 테스트케이스를 집어넣는 방식을 사용하면 유닛테스트가 분산되어서 관리하기가 어렵더라. 그래서 java에서 일반적으로 사용하는 방식인 소스와 유닛테스트를 같은 형태의 프로젝트 트리를 유지하면서 위치만 다른곳에 두기로 했다.

자바에서는 유닛테스트가 잇으면 다음과 같은 형태의 프로젝트 트리를 사용한다. 아래와 같은 형태의 프로젝트 트리의 장점은 소스가 위치한 경로를 알면, src만 test로 바꿔서 테스트케이스 파일을 찾을수있다. 이 장점을 flask 프로젝트 구조에 적용하고 싶었다. 다만, src가 있으면 프로젝트 폴더 구조에 깊이 1레벨에 더 추가되니까 이건 별로 마음에 들지 않아서 src는 사용하지 않았다.(맨 위에 나오는거처럼 test라는 폴더만 분리하고 테스트케이스를 전부 밀어넣었다)
```
ROOT
├── src
│   └── SomeClass.java
└── test
     └── SomeClassTest.java
```

테스트케이스 파일은 파일명이 ```<orig_filename>_tests.py```의 형태로 했다. 자바의 코딩스타일과 마찬가지로, 구현체 파일과 테스트케이스 파일의 파일명을 다르게 함으로써 파일명으로 내가 지금 보고있는 코드가 테스트케이스인지 구현체인지를 확인할수 있도록 했다. 이전에는 파일이 존재하는 디렉토리만으로 테스트케이스/구현체를 구분했는데 자주 헷갈렸는데 방식을 바꾸고나니 헷갈리는 일은 없어졋다.
