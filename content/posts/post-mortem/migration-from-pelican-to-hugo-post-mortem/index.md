---
title: 블로그를 Pelican에서 Hugo로 이사하기
subtitle: 파이썬 탈주 포스트모템 #1
tags: [pelican, hugo, static site generator, postmortem]
slug: migration-from-pelican-to-hugo-post-mortem
author: if1live
date: 2016-08-20
url: /posts/migration-from-pelican-to-hugo-post-mortem
---

# 개요

pelican을 이용하는 블로그를 hugo 기반으로 바꾸기

* 기간 : 2016/02/?? ~ 2016/03/??, 2016/07/31 완료
* 주요 원인
  * pelican은 느리다
  * 파이썬 2.x 는 도태된거같다
  * 파이썬 3.x 의 호환성을 믿을 수 없다

## Timeline

pelican으로 만들어진 블로그를 hugo로 엎는거니까 pelican을 쓰는 동안의 기록을 정리해봤다.
3년동안 pelican 썻더니 재밌는 커밋이 많이 보이더라.

### 2013년 2월 28일
libsora.so 도메인을 등록했다. 블로그와 위키를 동시에 필요했기 때문에 dokuwiki를 사용했다.

### 2013년 5월 19일
기존에 사용하던 정책을 버리고 정적 사이트 생성기로 이사했다. 서버를 직접 굴리는게 귀찮더라.
[if1live.github.io](https://github.com/if1live/if1live.github.io) 저장소에 최초로 의미있는 커밋이 등장한다.
[Commit](https://github.com/if1live/if1live.github.io/commit/e3804c3c38ff1d734c08b947daf7cb277f1e46c1)
해당 저장소에 `_blog`라는 디렉토리를 만들어서 여기에는 정적 사이트 생성기를 돌리는데 필요한 정보를 집어넣었다.
(최초에는 단순하게 만드려고 정적 사이트 생성기와 생성된 결과물을 하나의 저장소의 하나의 브렌치에서 관리했다)

당시에는 정적 사이트 생성기하면 [Jekyll](https://github.com/jekyll/jekyll)가 유명했지만 당시의 나는 파이썬 웹개발로 일하고 있었기때문에 jekyll를 고르지 않았다.
대신 파이썬 기반이며 템플릿 엔진이 [jinja](http://jinja.pocoo.org/)인 [Pelican](http://blog.getpelican.com/)를 선택했다.

requirements.txt가 없지만 어떤 버전의 pelican을 사용했는지는 추정해볼수 있다.
2013년 4월 24일에 pelican 3.2.0가 릴리즈되었고  2013년 9월 24일에는 pelican 3.3.0가 릴리즈 되었다는 기록을 참고할때 pelican 3.2.0을 이용했을거다.
[Pelican 3.2 Changelog](http://docs.getpelican.com/en/3.6.3/changelog.html#id7)를 보면 pelican은 3.2부터 python 3를 지원했다.
하지만 2013년의 나는 파이썬3를 쓰기에는 너무 이르다고 생각해서 파이썬 2.7을 선택했다.

### 2013년 8월 31일
정적 사이트 생성에 필요한 데이터를 저장할 저장소를 따로 만들었다.
[Commit](https://github.com/if1live/libsora.so/commit/13878d706d388837fee7287719a5d70385e7fdaf)
생성기와 생성 결과물을 다른 저장소에서 관리하기 시작했다.

### 2014년 1월 27일
requirements.txt가 등장한다. pelicna 3.3.0으로 업그레이드 되었다.
[Commit](https://github.com/if1live/libsora.so/commit/dd34515028f5d42583ecaed6dd4cf5392d5addca)
같은 날, travis-ci 연동을 시도했다. requirements.txt를 만든 이유는 travis-ci일거다.
[Commit](https://github.com/if1live/libsora.so/commit/d6fdb579b04cea455919dc33e2d26b7f04275b93)

### 2015년 9월
파이썬 2.7을 버리고 파이썬 3.x로 갈아탈 생각을 하기 시작했다.
파이썬 3.x를 지원하는 라이브러리가 많다고 정리한 자료를 보니까 이제 파이썬 3.x로 넘어가도 될거같다는 생각이 들었다.

* [PYTHON 3 WALL OF SUPERPOWERS](https://python3wos.appspot.com/)
* [Python 3 Readiness](http://py3readiness.org/)

### 2015년 9월 17일
파이썬 3.5, pelican 3.6.3으로 업그레이드 했다. 나도 이제 파이썬 3 유저.
[Commit](https://github.com/if1live/libsora.so/commit/22044518ae5706dd763128d8ef7dcdc8748cc26c)

### **2015년 10월 7일**
블로그 빌드가 깨졌다. [Build Log](https://travis-ci.org/if1live/libsora.so/builds/84113267)
빌드가 깨진 원인은 [pelican-plugins](https://github.com/getpelican/pelican-plugins)에 들어있는 플러그인 중 하나인 `code_include`였다.
파이썬2와 파이썬3에서는 module import 관련 정책이 바뀌어서 돌아가지 않는게 원인이었다.

[Python 3 Readiness](http://py3readiness.org/)같은게 보여주듯이 주요 라이브러리는 파이썬3를 대응하고 있다.
**하지만 플러그인은? 개인이 짠 작은 라이브러리도 나중에 파이썬3를 지원할까? 그럴리가 없다.**
파이썬 3 호환성 문제는 사용하다가 밟은 사람이 직접 나서서 고쳐야한다.
`code_include`에서 발생한 호환성 문제를 내가 직접 고친다고 치자. 다음에도 이런 문제가 또 발생하면? 그때도 내가 고쳐야하나? 내가 그 작업을 하긴 귀찮다.
그렇다고 파이썬 2.x에 계속 남아있는 것도 잘못된 선택같다. 그래서 그냥 파이썬 자체를 탈출하기로 했다.

(참고 : code_include가 python 3에서 작동하지 않는 문제는 고쳐진 상태이긴 했다. 일주일전에 고쳐졌을줄은 몰랐지.
[Commit](https://github.com/getpelican/pelican-plugins/commit/350cb270171310b990a72ba69e98bf21e4cec360))

## Why Hugo?

pelican 대신 사용할 정적 사이트 생성기를 선택할때 검토한 내용은 다음과 같다.

* 빌드가 빨라야한다. pelican의 경우 플러그인과 문서가 많아서 그런지 빌드하는데 10초 정도가 걸렸다. 수정한 내용을 브라우저로 확인하는 시간을 줄이고 싶다.
* 사용자가 많아야한다. 쓰는 사람이 별로 없으면 문제 생겼을때 피곤하다.
* markdown을 지원한다. 설마 markdown을 지원하지 않는 정적 사이트 생성기는 없겠지만.
* 템플릿 엔진이 쓸만해야한다. jinja2 정도로 쓰기 편하면 좋겠지만 거기까진 바라지 않는다. [mustache](https://mustache.github.io/)보다는 좋아야한다.
* 생태계가 안정적이어야한다, 과도한 변화가 없어야한다. pelican으로 3년쓴거 보면 새로 선택한 물건으로도 3년은 쓸거다. 앞으로 3년정도 사이에 대격변이 없어야한다.

정적 사이트 생성기를 선택할때는 [StaticGen](https://www.staticgen.com/)을 참조했다.
Jekyll은 한번 써본적이 있는데 성능 문제로 탈락했다. 루비나 파이썬이나 성능은 거기서 거기더라.
node 기반의 정적 사이트 생성기는 생태계를 믿을수 없어서 버렸다. grunt가 멸망하고 webpack, glup가 떠오른걸 지켜봤기 때문에 고를수 없었다.
node 기반을 물건을 쓰면 3년안에 빌드 스크립트 엎을일이 생길거같은 기분이 들더라.

[hugo](http://gohugo.io/)의 경우 별 문제가 없어보였다.

* pelican기반의 정적 사이트 생성기를 golang으로 다시 짜서 수십배 빠른걸 직접 확인해봤다. hugo는 golang기반이니까 파이썬기반이 pelican보다는 확실히 빠를거다.
* Hugo 저장소의 star 갯수를 보니 유저수가 충분히 많다.
* markdown을 지원하지 않는 정적 사이트 생성기가 설마 있겠어?
* golang의 [html/template](https://golang.org/pkg/html/template/)를 직접 써봤다. jinja2급은 아니지만 못쓸 물건은 아니었다.
* golang 정도면 언어가 폭망하는 일이 당분간 없지 않을까? 몇년된 언어니까 설마 생태계 측면에서 통수 치겠어?
  * 하지만 나중에 [통수치는 일이 발생했다]({{< ref golang-package-management-failure-learned-from-github-kr-text >}})

## 과정
pelican에서 hugo로 갈아탈 생각을 한 김에 미뤄놨던 작업도 같이 하기로했다.
문서 포맷의 통합(reStructedText, Markdown 문서가 섞여있었다), Less대신 Sass로 갈아타기, 나중에 마음이 바뀌었을때 다시 탈주할수 있도록 설계하기,...

hugo로 갈아타는 과정이 빨리 끝날수도 있지만 내가 게으르기때문에 그럴리가 없다.
그래서 hugo로 갈아타는 작업을 하면서 동시에 pelican을 사용하는것이 가능하도록 했다.
이를 위해서 다음과 같은 순서로 작업했다.

1. pelican의존적인 문서를 플랫폼 독립적인 문서로 변경
2. pelican으로 생성한 html과 hugo로 생성한 html이 똑같도록 만들기
3. hugo로 바꾸기
4. hugo로 바꾸는김에 개선하고 싶은 부분을 개선

### reStructedText -> Markdown

pelican 사용하던 시절에는 문서를 2가지 포맷(markdown, reStructuredText)으로 작성했다.
hugo로 옮겨가려면 두가지 규격을 섞는것보다 하나로 통일하는게 편하겠더라.
호환성을 생각해서 rst를 markdown으로 변환하기로 했다.
A문법으로 작성된 문서를 B문법으로 변환해주는 프로그램인 [Pandoc](http://pandoc.org/)을 이용해서 간단하게 문서를 변환했다.

### Pelican Plugin 제거

글 쓸때 글 안에 코드를 그대로 집어넣는건 불편하다. 만약 코드를 별도의 cpp 파일로 만들고 이를 문서에 그대로 넣을수 있으면 편하지 않을까?
이 역할을 해주는게 [code_include](https://github.com/getpelican/pelican-plugins/tree/master/code_include) 라는 pelican plugin이다.
이런 식으로 글쓰는걸 편하게 하려고 몇가지 플러그인을 사용했는데 이것들은 pelican 의존적이다. hugo로 옮기려면 이것들을 대신할 무언가가 필요하다.

그래서  markdown 전처리기인 [maya](https://github.com/if1live/maya)를 개발했다.

**markdown 문서에 html을 집어넣는게 가능하잖아? 그러면 markdown문서에 특별한 문법을 추가하고 전처리기가 이 부분만 html로 바꿔주면 되잖아?**

기존에 사용하던 플러그인을 maya로 다시 구현했다. 동시에 몇가지 기능도 추가로 구현했다.

* [code_include](https://github.com/getpelican/pelican-plugins/tree/master/code_include)
* [pelican-gist](https://github.com/streeter/pelican-gist)
* [pelican_youtube](https://github.com/kura/pelican_youtube)

전처리기의 출력물이 markdown이기 때문에 hugo가 아니라 다른 사이트 생성기를 쓸때도 그대로 이용할수 있다.
몇년뒤에 hugo 대신 다른 정적 사이트 생성기로 갈아타더라도 markdown만 유지한다면 그대로 쓸수있다는 점이 좋더라.

자세한 내용은 [Maya (Markdown 전처리기) 포스트모템]({{< ref maya-markdown-preprocessor-post-mortem >}})에서 다룬다.

### Less -> Sass

2013년부터 Less를 쓴 이유는 bootstrap 2.x였다.
bootstrap자체가 좋은 less 예제니까 이를 보고 배울게 많다고 생각했다.
하지만 bootstrap 4부터는 sass로 바꿨더라.
bootstrap팀이 한가한것도 아닐텐데 less를 버리고 sass로 갔다는건 less에는 없는 무언가가 sass에 있다는거 아닐까?
나도 less 대신 sass를 쓴다면 less에서는 찾지 못하는 무언가를 볼수 있지 않을까? 라고 생각해서 바꿔봤다.

내 경우 문법은 SCSS를 선택했다. 중괄호가 사라진 Sass는 적응하기 어렵더라.
이미 작성된 Less를 그대로 옮겨오기에는 중괄호가 있는 SCSS쪽이 낫겠더라.

mixin를 제외하면 less와 sass는 비슷한 느낌이다.

SCSS로 작성한 간단한 mixin이다.
```scss
@mixin border-radius($radius) {
  border-radius: $radius;
}
.box { @include border-radius(10px); }
```

Less로 작성한 간단한 mixin이다.
```less
.border-radius(@radius) {
  border-radius: @radius;
}
.button { .border-radius(10px); }
```

Sass의 경우 mixin을 쓸때 앞에 `@include`를 붙인다. 그래서 한눈에 이것은 mixin이라는걸 알수있다.
하지만 Less에서는 `.`로 시작한다. 그래서 내 경우 css class selector랑 한눈에 구분되지 않더라.

```less
.border-radius(@radius: 5px) {
  border-radius: @radius;
}

#header {
  // mixin syntax
  .border-radius;
  // css class selector syntax
  .border-radius { border-radius: 5px; }
}
```
sass를 컴파일할때는 [wellington](https://github.com/wellington/wellington)를 이용했다.
sass는 원래 ruby기반으로 개발되었지만 sass 컴파일하겠다고 루비를 깔긴 귀찮다.
그래서 golang 기반인 wellington을 사용했다.

### Markdown Post-Processing
어떤 정적 사이트에서도 쓸수있는 html로 찍어내는 부분(youtube 동영상 embed, 파일 embed)은 maya로 처리할수 있다.
하지만 정적 사이트 내부 링크는 어떻게 처리할까?
pelican과 hugo모두 고유한 문법을 이용해서 문서간 링크를 표현한다.

* pelican : \[link\](\{filename\}/category/article1.rst)
* hugo : \{\{< relref "post.md" >\}\}

이것까지 maya에서 처리해준다면 maya가 정적 사이트 생성기에 의존하게 되어버린다.
만약 정적 사이트 생성기에서 쓰는 문법이 바뀌면 maya도 바꿔야할거다.
(실제로 pelican 3.2까지는 |filename|를 사용했지만 3.3부터는 \{filename\} 을 사용한다)
그래서 링크 변환은 maya대신 쉘스크립트를 사용했다.

```bash
function sanitize_hugo_markdown() {
	find ./content -name "*.md" | xargs -I{} sed -i -e  's/{attach}/\/posts\//g' {}
	find ./content -name "*.md" | xargs -I{} sed -i -e  's/{filename}/\/posts\//g' {}
	find ./content -name "*.md" | xargs -I{} sed -i -e  's/{article}/\/posts\//g' {}
	find ./content -name "*.md" | xargs -I{} sed -i -e  's/\.md//g' {}
}
```

### Build Process

1. maya가 처리가능한 문법으로 markdown 문서 작성 (`/content/article_slug/article.mkdn` 작성)
2. 작성된 markdown 문서를 전처리 (`/content/article_slug/article.mkdn`로 `/content/article_slug.md` 생성)
3. 생성된 markdown 문서를 hugo로 복사 (`/content/article_slug.md`를 `/hugo/content/article_slug.md` 로 복사)
4. sass를 컴파일해서 블로그 테마 생성 (wt이용)
5. markdown + sass => published html
6. published html를 github repo로 commit + push

### Build time

빌드시간을 비교할떄 사용할 샘플을 선택했다

* [pelican을 이용한 빌드](https://travis-ci.org/if1live/libsora.so/builds/111211412)
  * `pip install -r requirements.txt -q` : 22.78s
  * `npm install -g less` : 4.87s
  * `make publish` : 8.43s
* [hugo를 이용한 빌드](https://travis-ci.org/if1live/libsora.so/builds/148617649)
  * `go get -u github.com/if1live/maya` : 2.19s
  * `go get -u github.com/spf13/hugo` : 38.80s
  * `go get -u github.com/wellington/wellington/wt` : 48.04s
  * `./bin/build_theme_hugo.sh` : 0.06s
  * `/bin/publish_hugo.sh` : 2.01s

|Type         | Setup Site Generator | Setup CSS Preprocessors | Setup Misc | Build |
|-------------|----------------------|-------------------------|------------|-------|
|Pelican Base | 22.78s               | 4.87s                   | x          | 8.43s |
|Hugo Base    | 38.80s               | 48.04s                  | 2.19s      | 2.07s |

정적 사이트 생성기, CSS 전처리기를 설치하는데 걸리는 시간이 길어졌다.
이전에 pelican만 쓸때는 필요없던 새로운 프로그램을 설치하게 되었다.
다만 실제 환경에서는 매번 hugo, wellington, maya를 설치하지 않기때문에 별 문제 없다.
블로그 내용을 수정했을떄 travis-ci에서 수정된후 실제 블로그에 반영될때까지 오래 걸릴뿐이다.

빌드 시간 자체는 pelican을 쓸때보다 hugo를 쓸때 빨라졌다.
hugo에서 홍보하는 만큼 빨라지지 않은 이유는 markdown 전처리 과정(maya, find, sed등)이 추가되어있기 때문이다.

## 삽질로 느낀점

* hugo는 빠르다. 매우 빠르다.
* pelican의 jinja2가 그립다. golang의 템플릿 엔진이 아무리 좋아봤자 jinja2를 이길순 없다.
* 완벽한 언어는 없다. 파이썬에 문제가 있듯이 golang에도 문제가 있다. 파이썬에 2 vs 3 버전 문제가 있다면 golang에는 다른 의미의 버전 문제가 있다.
* markdown preprocessor라는 개념에 도달하는데 왜 3년이나 걸렸을까? 진작에 이걸 생각해냈으면 좋았을텐데
* less vs sass. 지금의 나는 sass를 고를거다.
