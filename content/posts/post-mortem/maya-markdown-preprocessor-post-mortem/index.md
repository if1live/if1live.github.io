---
title: Maya (Markdown 전처리기) 포스트모템
subtitle: Template -> Markdown Preprocessor -> Markdown
tags: [markdown, postmortem]
slug: maya-markdown-preprocessor-post-mortem
author: if1live
date: 2016-05-06
url: /posts/maya-markdown-preprocessor-post-mortem
---

## Why?

2013년부터 2016년 2월까지 정적 사이트 생성기로 [pelican][pelican]을 썼다.
파이썬 3.x 시대가 열리면서 pelican에 대한 불만이 생겨서 [hugo][hugo]로 갈아탈 생각을 했다.
(왜 pelican을 버리게 되었는지에 대한 자세한 내용은 여기서는 다루지 않는다)
그동안 pelican에 의존해서 작성된 문서를 hugo에서도 돌아가도록 고칠 도구가 필요해서 만들게 되었다.

* 언어 : golang
* 저장소 : [github.com/if1live/maya](https://github.com/if1live/maya)
* 개발기간 : 2016/02/20 ~ 2016/03/11
* 적용사례 : [libsora.so](https://if1live.github.io)

## Feature

### 문서 메타데이터를 지원해야한다

Markdown는 일반 텍스트 문서의 양식을 편집하는 문법이다.
원래 일반 텍스트 문서를 다루는 용도라서 작성일, 작성자, 태그, 분류등의 정보가 들어갈 자리가 없다.
그런데 정적 사이트 생성기에서는 작성일, 작성자, 태그, 분류등의 메타데이터를 문서의 상단에 집어넣는다.
그리고 메타데이터는 마크다운 공식 문법이 아니라서 표준이 없다.
pelican, hugo 등등 정적 사이트 생성기마다 문법이 다르다.

pelican의 메타데이터는 다음과 같이 생겼다.

```
Title: first_post
Date: 2014-12-13 18:32
Category: Test

TODO - write content
```

hugo의 메타데이터는 다음과 같이 생겼다.
(TOML이라는 문법을 기본값으로 사용하지만 YAML, JSON을 선택할수도 있다)

```
---
date = "2016-02-14T16:11:58+05:30"
draft = true
title = "this is title"
---

TODO - write content
```

기존 문서의 메타데이터는 pelican 문법이다.
pelican에서 hugo로 갈아타겠다고 문서의 메타데이터까지 고치긴 귀찮더라.
만약 전처리기가 pelican의 문법으로 읽어서 hugo용으로 출력할 수 있다면 메타데이터를 고치지 않아도 될거다.


### 코드를 문서에 포함시킬수 있어야한다
기존에 [code_include][code_include] 라는 Pelican용 ReStructuredText용 플러그인을 썼다.
이 플러그인을 쓰고 특정 규격으로 문서에 포함될 파일을 명시하면 정적 사이트를 생성할때 해당 내용이 포함된다.
똑같은 기능을 전처리기에서 수행한다면 플러그인 의존성을 제거할 수 있을것이다.

```
.. code-include:: main.c
    :lexer: c
```

```c
#include <stdio.h>
int main() {
  printf("hello world, maya!\n");
  return 0;
}
```


### 커맨드라인 실행 결과를 문서에 포함시킬수 있어야한다.

샘플 코드를 작성하고 작성한 코드를 문서에 넣는 것만으로는 불충분하더라.
코드의 실행 결과를 문서에 포함시키는 작업을 자동화시키고 싶었다.
만약 전처리기가 커맨드라인 명령을 실행시킨후 stdout/stderr의 내용을 문서에 포함시킬수 있다면 귀찮은 작업이 줄지 않을까?

```bash
$ clang main.c && ./a.out
hello world, maya!
```

### Gist를 문서에 포함시킬수 있어야한다

기존에 [pelican-gist][pelican-gist]라는 Pelican용 Markdown 플러그인을 쓰고있었다.
특별한 문법으로 포함시킬 gist 정보를 채워두면 실제 gist로 교체된다.
똑같은 기능을 전처리기에서 수행한다면 플러그인 의존성을 제거할수 있을것이다.

```
[gist:id=3ee77ed16a900b74922b,file=fibonacci_with_signal.c]
```

<div class="maya-gist">
<script src="https://gist.github.com/3ee77ed16a900b74922b.js?file=fibonacci_with_signal.c"></script>
</div>

### Youtube 동영상을 문서에 포함시킬수 있어야한다

기존에 [pelican_youtube][pelican_youtube]라는 Pelican용 ReStructuredText 플러그인을 쓰고 있었다.
아래와 같은 내용을 문서에 넣으면 youtube 동영상이 문서에 들어간다.
전처리기에서 똑같은 기능을 제공하면 좋겠다.

```
.. youtube:: cNG2TCmUCQc
```

<div class="maya-youtube">
<iframe width="640" height="480" src="//www.youtube.com/embed/cNG2TCmUCQc" frameborder="0" allowfullscreen></iframe>
</div>

### 새로운 명령의 추가가 쉬워야한다

지금은 별로 관심없지만 나중에 특정 트윗을 문서에 때려박거나 자바스크립트 데모를 문서에 때려박고 싶다는 생각이 들지 모른다. 새로운 명령의 추가가 쉬웠으면 좋겠다.
(코드 포함시키기, 실행결과 포함시키기, gist, youtube를 문제없이 구현했다면 어느정도의 확장성은 보장될 것이다.)


## Markdown Preprocessor

처음 구현을 시작했을때는 내가 만들고 싶은걸 뭐라고 불러야하는지 몰랐다. 그래서 검색을 하지 못했다.
어느정도 구현하다보니 만들고 있는게 일종의 Markdown Preprocessor 라는걸 깨닫고 검색해봤다.
몇가지 구현체가 이미 있더라.

* [markdown-pp][markdown-pp]
* [gitdown][gitdown]
* [markedpp][markedpp]

몇가지 이유로 직접 구현했다.

1. 내가 필요로 하는 기능이 100% 구현된 물건이 없었다
2. 포크 떠서 내가 필요로 하는 기능을 모두 구현한 다음에 Pull Request를 보내도 Merge될 가능성이 없다.
표준화된 메타데이터 문법을 제외한 기능은 구현해서 PR 보내면 가능성이 있는데 메타데이터는 답이 없더라.
메타데이터는 마크다운 전처리기에서는 쓸모없는 기능이니까
3. Golang 공부해보자. 이럴때 아니면 언제 golang 써보겠나?

## 문법

처음에는 pelican-gist 의 영향을 받아서 `[maya:<cmd>=<params>]` 형태의 문법을 채택했다.
나중에 code include 플러그인과 동일한 기능을 구현할때 문제가 있더라.
인자의 갯수가 많거나 길어지면 읽기 어렵다. 그래서 이 문법은 버렸다.

다음으로 생각한 문법은 JSON이다. gitdown이 JSON을 쓴다.
문서에 `{"gitdown": "variable", "name": "nameOfTheVariable"}`같은 JSON을 때려박으면 전처리기가 이를 변환한다.
문서에서 JSON을 어떻게 찾아낼것인가? gitdown에서는 정규식을 썻더라.

```js
// https://github.com/gajus/gitdown/blob/master/src/parser.js#L84
outputMarkdown = outputMarkdown.replace(/({"gitdown"(?:[^}]+}))/g, (match) => {
```

JSON을 이용하는것도 나쁘진 않은 방법같은데 몇가지 문제가 있어서 생각하는 것을 그만두었다.
첫번째 문제는 `_` 이다. JSON안에 파일명을 적다보면 `_` 를 피할 수 없다.
근데 에디터가 gitdown 문법을 못 알아먹으니 JSON안에 들어간 `_` 를 _이탤릭체 문법_ 으로 인식해서 syntax highlight가 이상하더라.
두번째 문제라면 개인적으로 JSON 자체를 좋아하지 않는다.
trailing comma, 문자열을 따옴표로 표시하기, 여러줄에 걸쳐있으면 어떻게 될까 등등을 생각하기 귀찮아서 버렸다.

다음 대안으로는 뭘 쓸까 생각하다 마크다운에 내장된 코드 표현법을 응용하기로 했다.
마크다운 문법중 코드를 때려박는 \`\`\` ~ \`\`\`가 있다.
이것과 비슷한 형태를 쓰면 에디터에서 적당히 알아먹어서 예쁘게 syntax highlight 해주지 않을까?
그래서 지금까지 한번도 입력한적 없는 ~~~ 를 골랐다.
생각보다 괜찮더라

```
\는 실제로 사용할때는 무시

\~~~maya:view
file=main.c
start_line=1
end_line=2
\~~~

\~~~maya:execute
cmd=clang main.c && ./a.out
attach_cmd=true
\~~~

\~~~maya:gist
id=3ee77ed16a900b74922b
file=fibonacci_with_signal.c
\~~~

\~~~maya:youtube
video_id=cNG2TCmUCQc
\~~~
```

## 개발/사용 소감
정적 사이트 생성기를 처음 사용한게 2013년인데 마크다운 생성기라는 개념을 2016년에서야 생각해냈다.
왜 이걸 진작에 생각해내지 못한거지? 진작에 생각해냈으면 ReStructuredText 따위는 안봐도 됐을텐데.

Maya 개발 이후 블로그의 문서를 전부 maya 기반으로 바꿨다.
코드를 문서에 때려박는 기능과 커맨드라인 실행 결과를 문서에 때려박는 기능이 제일 유용했다.

특정 구간에서 전처리기를 끄는 기능이 있으면 좋겠는데 귀찮아서 아직 안만들었다.
진짜로 `~~~` 가 필요할때 `\~~~`로 대신 입력해야되는것만 빼면 크게 불편하지 않더라.
나중에 진짜로 필요해지면 gitdown처럼 구간 on/off 를 만들까 생각중이긴하다.

```
Gitdown JSON will be interpolated.
<!-- gitdown: off -->
Gitdown JSON will not be interpolated.
<!-- gitdown: on -->
Gitdown JSON will be interpolated.
```

Maya덕분에 pelican에서 hugo로 블로그를 이전하는건 특별한 문제없이 끝났다.
현재 Maya를 사용중이며 이 글을 포함한 블로그의 모든 내용은 maya를 거친다.

[pelican]: http://docs.getpelican.com/
[hugo]: https://gohugo.io/

[code_include]: https://github.com/getpelican/pelican-plugins/tree/master/code_include
[pelican-gist]: https://github.com/streeter/pelican-gist/tree/master
[pelican_youtube]: https://github.com/kura/pelican_youtube/tree/master

[markdown-pp]: https://github.com/jreese/markdown-pp
[gitdown]: https://github.com/gajus/gitdown
[markedpp]: https://github.com/commenthol/markedpp
