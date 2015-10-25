HTML textarea의 개행문자는 무엇일까?
====================================

:subtitle: LF vs CRLF vs 상황에 따라 다르다 vs 충격과 공포
:tags: bug, html, textarea, newline
:slug: what-is-textarea-newline
:author: if1live
:date: 2015-10-25

개요
----

내가 요즘에 만드는 것은 HTTP기반의 모바일 게임이다.
서버와 클라는 HTTP로 통신을 하고 클라는 서버의 요청을 예쁘게 그려주는 역할을 수행한다.
간단히 말하면 서버는 그냥 웹서버고 클라는 특별한 브라우저라고 할 수 있다.

런칭 직전에 게임에서 이상한 버그를 발견했다.
관리도구에서 열어보면 ``{first-line}{newline}{second-line}`` 으로 보이는데
실제 게임에 보면 ``{first-line}{newline}{newline}{second-line}`` 으로 보이는 버그였다.
의도하지 않게 개행이 하나 더 들어갔다.

찾기는 어려웠지만 원인은 생각보다 간단했다.
의도하지 않은 ``CR`` 이 DB에 저장되어 있고
클라가 ``CR`` 을 제대로 처리를 하지 않아서 두줄짜리 개행으로 보인거였다.
(찾기 어려운건 ``CR`` 이 눈에 보이는 글자가 아니기 때문)

의도하지 않은 ``CR`` 은 급한대로 사용한 rails_admin의 textarea 때문에 DB에 저장된 것이었다.
그렇다면 여기서 문제, textarea의 개행문자는 뭘까?
textarea는 항상 ``CRLF`` 를 개행으로 쓰는가?
내가 윈도우 크롬에서 HTTP 요청을 날렸기 때문에 ``CRLF`` 가 저장된걸까?
아니면 textarea가 뭐를 서버한테 보내든지 상관없이 rails가 개행을 ``CRLF`` 로 치환한걸까?

`@if1live <https://twitter.com/if1live/status/657422182235119616>`_ ::

	어제 제대로 낚인 버그의 근원이 갑자기 궁금해졌다
	Q: textarea에다가 다음을 넣고 submit했다. 서버에서는 무엇이 도착하는가?
	"foo
	bar"
	A: "foo\r\nbar"
	B: "foo\nbar"
	C: 브라우저에 따라서 다르다

Test
----

HTML textarea의 개행문자가 무엇인지 확인하려고 다음의 테스트를 준비했다.

* ``LF`` , ``CRLF`` 을 섞어서 HTML textarea에 집어 넣는다.
* HTML textarea를 submit하고 서버에 어떤 값이 도착하는지 확인한다.
* wireshark를 이용해서 해석

테스트 코드는 여기에서 볼 수 있다.
`Repo <https://github.com/if1live/libsora.so/tree/master/content/development/what-is-textarea-newline>`_

6가지 경우의 개행문자의 조합을 테스트했다.

* ``\n`` 만 사용하면 어떻게 되는가?
* ``\r`` 만 사용하면 어떻게 되는가? (예외 처리 테스트)
* ``\n`` 을 연속으로 사용하면 어떻게 되는가?
* ``\r`` 을 연속으로 사용하면 어떻게 되는가? (예외 처리 테스트)
* ``\r\n`` (CRLF)를 사용하면 어떻게 되는가?
* ``\n\r`` (LFCR)을 사용하면 어떻게 되는가? (예외 처리 테스트)


POST, GET일때 기본적인 내용은 동일하더라.
그래서 POST 일때만 정리했다.

..  figure:: {filename}../static/what-is-textarea-newline/browser-input.png
	:alt: POST Request in browser
	:align: center
	:figwidth: 100%

	간단하게 구현한 POST 요청을 보내는 HTML이다.
	겉보기에는 평범한 개행이 들어있는 textarea이지만 실제로는 각각의 줄은 다른 개행문자를 쓰고있다.

..  figure:: {filename}../static/what-is-textarea-newline/wireshark-input.png
	:alt: POST Response in browser
	:align: center
	:figwidth: 100%

	서버에서 생성한 테스트용 개행문자가 섞인 HTML은 클라에 잘 도착했다.
	나머지는 브라우저의 역할이다.

..  figure:: {filename}../static/what-is-textarea-newline/browser-output.png
	:alt: POST HTML in wireshark
	:align: center
	:figwidth: 100%

	클라가 submit 한 결과를 그대로 출력했다. ``CRLF`` 만 보인다.
	각각의 줄의 결과는 아래에서 분석한다.

..  figure:: {filename}../static/what-is-textarea-newline/wireshark-output.png
	:alt: POST Response in wireshark
	:align: center
	:figwidth: 100%

	클라가 보낸 요청을 wireshark 로 열어본 것이다.
	form의 내용을 열어보면 ``CRLF`` 만 보인다.

Result
######

wireshark로 확인해 본 결과 브라우저로 진입하는 순간에는 ``LF``, ``CRLF`` 를 포함한 여러가지 개행문자 조합이 존재하지만
브라우저에서 나올때는 ``CRLF`` 만 남아있다. (글에서는 POST만 다루었지만 실제로는 GET, POST 요청의 결과가 동일하다)

6가지 개행문자를 이용해서 테스트한 결과를 정리하면 다음과 같다.

* LF만 사용하는 경우 : ``only-LF\n`` -> ``only-LF\r\n``
   * CRLF로 치환
* CR만 사용하는 경우 : ``only-CR\r`` -> ``only-CR\r\n``
    * CRLF로 치환
* 연속된 LF를 사용하는 경우 : ``many-LF\n\n`` -> ``many-LF\r\n\r\n``
    * 각각의 LF를 CRLF로 치환
* 연속된 CR을 사용하는 경우 : * ``many-CR\r\r`` -> ``many-cr\r\n\r\n``
    * 각각의 CR을 CRLF로 치환
* CRLF를 사용하는 경우 : ``CRLF\r\n`` -> ``CRLF\r\n``
   * 변경 없음
* LFCR을 사용하는 경우 : ``LFCR\r\n`` -> ``LFCR\r\n\r\n``
    * CRLF가 2개로 바뀐다. 첫번째 LF에 의해서 CRLF가 나오고 두번째 CR에 의해서 CRLF가 나온거로 보인다

즉, textarea에는 어떤 개행을 넣든지간에 ``CRLF`` 만 나온다.

끝?
---

과면 이걸로 끝날까?
wireshark를 이용해서 브라우저로 진입하는 순간과 브라우저에서 나오는 순간은 관측했지만
브라우저 내부에서는 어떻게 처리되는지를 확인하지 못했다.
서버에서 보낸 여러 개행문자가 섞인 문자열이 textarea에 올바르게 들어가있는지 확인해보자.
테스트용으로 사용한 HTML에서 다음 자바스크립트를 실행시키면 내용을 볼 수 있다.
CR, LF에 대응되는 문자를 읽을수 있는 문자열로 바꾸는 함수이다.

.. code:: javascript

  function convertReadableString(text) {
    var readable = "";
    for(var i = 0 ; i < text.length; ++i) {
      var code = text.charCodeAt(i);
      if(code == 10) {
        readable += "<LF>";
      } else if(code == 13) {
        readable += "<CR>";
      } else {
        readable += text[i];
      }
    }
    return readable;
  }
  var node = document.getElementsByTagName('textarea')[0];
  convertReadableString(node.value);


..  figure:: {filename}../static/what-is-textarea-newline/textarea-js.png
	:alt: where is CR?
	:align: center

	only-LF<LF>only-CR<LF>many-LF<LF><LF>many-cr<LF><LF>CRLF<LF>LFCR<LF><LF>

**CR이 전부 사라졌다!!!**
게다가 **LF만 있는데 어떻게 클라의 요청에는 CRLF가 있던거지???**

이 질문의 답을 찾으려면 HTML 스펙 문서를 뒤져야한다.

왜 CR이 전부 LF가 되었는가?
###########################

HTML5 스펙문서의 "4.10.11 The textarea element" 에서 이것과 관련된 내용을 찾을수 있다.

http://www.w3.org/TR/html5/forms.html#concept-textarea-raw-value

간단하게 요약하면 textarea의 value를 이용해서 값에 접근할때 CR, CRLF가 있으면 LF로 치환한 결과를 보여준다는거다.
그래서 자바스크립트로 얻은 textarea의 value에는 CR이 없다.

왜 LF가 요청에서는 CRLF로 보이는가?
###################################

form를 submit하는 과정을 정의한 스펙문서를 찾으면 이유를 알 수 있다.
스펙 문서의 두군데를 참고한다.

HTML5 스펙의 "4.10.19.6 Form submission"를 보면
form 태그의 enctype 속성의 기본값이 application/x-www-form-urlencoded 라는 것을 알 수 있다.
(HTML을 잘 아는 사람은 이미 아는 내용일거다)

..

	The invalid value default for these attributes is the application/x-www-form-urlencoded state.
	The missing value default for the enctype attribute is also the application/x-www-form-urlencoded state.
	(There is no missing value default for the formenctype attribute.)

http://www.w3.org/TR/html5/forms.html#attr-fs-enctype

form의 enctype 속성이 application/x-www-form-urlencoded 일때 form이 어떻게 submit 되는지는
HTML4 스펙의 "17.13.4 Form content types" 에서 찾을수 있다. 개행문자를 CRLF로 바꾼다고 한다.

..

	Forms submitted with this content type must be encoded as follows:
	...
	Line breaks are represented as "CR LF" pairs (i.e., %0D%0A).

http://www.w3.org/TR/html4/interact/forms.html#h-17.13.4.1

HTML5에서도 동일한 내용을 찾았지만 HTML4쪽의 내용이 읽기 쉽더라.
HTML5 스펙 문서의 "4.10.22.4 Constructing the form data set"

http://www.w3.org/TR/html5/forms.html#constructing-form-data-set

풀어쓴 내부 구조
----------------

textarea가 하나 있다. 여기에 개행문자가 섞인 문자열을 입력하고 submit했을때 벌어지는 과정을 풀어쓰면 다음과 같다.

1. 태초에 form 태그가 있었다. form의 enctype은 application/x-www-form-urlencoded 였다. (기본값)
2. form 태그의 child로 textarea가 있었다.
3. textarea에 개행문자가 섞인 문자열이 textarea에 들어있다. (사용자가 입력했을수도 있고 서버에서 내용을 채웠을수도 있다)
4. form을 submit한다.
    1. textarea의 value에 접근한다. CRLF, CR이 LF로 바뀌어있는 문자열을 얻는다.
    2. textarea에서 얻은 문자열의 LF는 CRLF로 치환된다.
    3. 이를 적절히 submit 한다.
5. 서버에는 개행문자가 전부 CRLF인 문자열이 도착한다.

Summary
-------
* 겉으로 보이는 것만 봐서는 숨겨진 의미를 알 수 없다.
    * 자바스크립트로 textarea.value를 보기전 까지는 브라우저 내부에서 CRLF로 모든걸 처리하는줄 알았다.
* 서버로 전달되는 textarea 값은 CRLF다. CR을 주의하지 않으면 사고친다.
* **HTML 스펙문서를 읽자**
