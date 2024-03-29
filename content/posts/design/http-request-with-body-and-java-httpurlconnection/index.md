+++
title = "HTTP 요청에 body를 붙여서 보내면 어떤 일이 벌어질까? part 2"
subtitle = "Java HttpURLConnection의 경우"
tags = ["http", "design"]
slug = "http-request-with-body-and-java-httpurlconnection"
author = "if1live"
date = "2019-02-09T00:00:00+00:00"
url = "/posts/http-request-with-body-and-java-httpurlconnection"
+++


## 삽질의 시작

이전에 [HTTP GET 요청에 body를 붙여서 보내면 어떤 일이 벌어질까?]({{< ref "http-get-request-with-body-and-http-library" >}})를 썼다.
나중에 자바의 HttpURLConnection의 동작이 안드로이드 버전의 UnityWebRequest와 똑같다는 소리를 들었다.
유니티 개발자들이 HTTP 라이브러리 만들기 귀찮아서 플랫폼별로 내장된 HTTP 라이브러리를 쓴거 아닐까? 하는 망상을 했다.
그래서 HttpURLConnection로 HTTP 요청을 보낼때 body 붙이는 짓을 해봤다. 

## 테스트

```js
const express = require('express');

const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.all('/', (req, res) => {
  res.json({ method: req.method, body: req.body });
});

const port = 3100;
app.listen(port, () => {
  console.log(`server listen - ${port}`);
});
```

아래의 [HTTP request methods][mdn-http-methods]는 표준에 정의되어 있다.

* GET
* HEAD : asks for a response identical to that of a GET request, but without the response body.
* POST
* PUT
* DELETE
* CONNECT : establishes a tunnel to the server identified by the target resource.
* OPTIONS
* TRACE
* PATCH

HEAD와 CONNECT method는 다른 method와 동작이 달라서 보내지 않았다.
나머지 HTTP request method (GET, POST, PUT, DELETE, OPTIONS, TRACE, PATCH)만 테스트해봤다.
(curl로 HEAD, CONNECT 요청을 보내면 재밌는 결과를 볼수있다.)

### curl

```bash
#!/bin/bash

function request_common {
    echo -e "$1 => \c"
    curl -X $1 http://127.0.0.1:3100/ \
        -H 'Content-Type: application/json' \
        -d '{"foo": 1}' -s
    echo ""
}

request_common "GET"
# request_common "HEAD"
request_common "POST"
request_common "PUT"
request_common "DELETE"
# request_common "CONNECT"
request_common "OPTIONS"
request_common "TRACE"
request_common "PATCH"
```

```txt
$ bash curl-demo.sh
GET => {"method":"GET","body":{"foo":1}}
POST => {"method":"POST","body":{"foo":1}}
PUT => {"method":"PUT","body":{"foo":1}}
DELETE => {"method":"DELETE","body":{"foo":1}}
OPTIONS => {"method":"OPTIONS","body":{"foo":1}}
TRACE => {"method":"TRACE","body":{"foo":1}}
PATCH => {"method":"PATCH","body":{"foo":1}}
```

curl을 이용해서 요청을 보낸 경우 서버에서 요청에 붙은 body를 받을 수 있다.
curl로 요청을 보낸 경우 응답이 원하는대로 나온다.
서버는 잘 돌아간다는걸 확인할 수 있다.

### java HttpURLConnection

```java
import java.net.URL;
import java.net.HttpURLConnection;
import java.io.*;
import java.nio.charset.Charset;
import java.util.stream.Collectors;

public class Demo {
    public static void requestCommon(String method) throws Exception {
        String query = "http://127.0.0.1:3100";
        String json = "{\"key\":1}";

        URL url = new URL(query);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setConnectTimeout(5000);
        conn.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
        conn.setDoOutput(true);
        conn.setDoInput(true);
        conn.setRequestMethod(method);

        OutputStream os = conn.getOutputStream();
        os.write(json.getBytes("UTF-8"));
        os.close();

        // read the response
        Charset charset = Charset.defaultCharset();
        InputStream in = new BufferedInputStream(conn.getInputStream());
        String resp = "";
        try (BufferedReader br = new BufferedReader(new InputStreamReader(in, charset))) {
           resp = br.lines().collect(Collectors.joining(System.lineSeparator()));
        }

        System.out.println(resp);

        in.close();
        conn.disconnect();
    }

    public static void main(String[] args) throws Exception {
        String[] methods = {
            "GET",
            // "HEAD",
            "POST",
            "PUT",
            "DELETE",
            // "CONNECT",
            "OPTIONS",
            "TRACE",
            "PATCH",
        };
        for(String method : methods) {
            System.out.print(method + " => ");
            try {
                requestCommon(method);
            } catch(Exception e) {
                System.out.println(e.getMessage());
            }
        }
    }
}
```

```txt
GET => {"method":"POST","body":{"key":1}}
POST => {"method":"POST","body":{"key":1}}
PUT => {"method":"PUT","body":{"key":1}}
DELETE => {"method":"DELETE","body":{"key":1}}
OPTIONS => {"method":"OPTIONS","body":{"key":1}}
TRACE => HTTP method TRACE doesn't support output
PATCH => Invalid HTTP method: PATCH
```

자바의 HttpURLConnection를 사용한 경우의 출력이다.
body가 붙은 HTTP 요청을 보내니까 HTTP request method에 따라서 출력이 다르다.
비슷한거끼리 묶어서 보자.

HTTP request method | desc
------------|-----
GET | POST로 보내진다.
POST, PUT, DELETE, OPTION | 성공
TRACE | HTTP method TRACE doesn't support output
PATCH | Invalid HTTP method: PATCH

## HttpURLConnection?

### GET이 POST로 바뀐 이유

동작이 이상해보일때는 소스를 뜯어보자.
스펙을 뒤지는 사람도 있던데 나는 영어를 못해서 그런거 힘들어.
검색하다보니 스택오버플로우에서 [좋은 답변](https://stackoverflow.com/a/27243207)을 찾았다.

답변에서는 SUN자바의 HttpURLConnection 구현을 찍어주더라.
하지만 접속이 안되더라.
그래서 OpenJDK에서 비슷한 코드를 찾았다.

http://hg.openjdk.java.net/jdk7/jdk7/jdk/file/tip/src/share/classes/sun/net/www/protocol/http/HttpURLConnection.java#l1072

```java
if (method.equals("GET")) {
    method = "POST"; // Backward compatibility
}
if (!"POST".equals(method) && !"PUT".equals(method) &&
    "http".equals(url.getProtocol())) {
    throw new ProtocolException("HTTP method " + method +
                                " doesn't support output");
}
```

Backward compatibility라니!
하위 호환성 문제로 request body가 붙은 GET 요청은 POST로 바꿔친댄다.
그렇다면 하위 호환성이 발생한 이유에 대해 망상해보자.
멍청한 결정을 분석할때는 역사적 배경도 고려해야된다.
지금보면 멍청해보이는 결정도 그때는 그럴싸했을지 모른다.
자바 만든 사람들이 나보다 멍청할리 없잖아?

[HttpURLConnection][java-httpurlconnection]은 오래된 클래스이다.
문서를 보면 JDK 1.1 시절부터 있었다고한다.
JDK 1.1은 1997년 2월 19일에 나왔다고하더라.

[REST][wiki-rest]은 Roy Fielding의 2000년 박사학위 논문에서 소개되었다고 하더라.

HTTP/1.0은 1996년에 나왔다.
RFC1945 [Hypertext Transfer Protocol -- HTTP/1.0][rfc-1945-http10]의 공개일이 1996년 5월이더라.
HTTP/1.0에서 제대로된 HTTP request method는 GET, HEAD, POST 뿐이었다.
RFC에서 PUT, DELETE, LINK, UNLINK라는 HTTP method도 언급하지만 Additional Request Methods라는 해서 부록에 붙어있다.

HTTP/1.1은 1997년에 나왔다.
RFC2068 [Hypertext Transfer Protocol -- HTTP/1.1](https://tools.ietf.org/html/rfc2068)의 공개일은 1997년 1월이다.
HTTP request method는 OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE가 되었다. 지금이랑 크게 다르지 않다.

시간순으로 적당히 나열하자.

1. 1996년 5월: HTTP/1.0
2. 1997년 1월: HTTP/1.1
3. 1997년 2월: JDK 1.1
4. 2000년 : REST

HttpURLConnection 만들던 시절에는 REST라는 개념이 없었다.
JDK 1.1의 생일과 HTTP/1.1의 생일은 1달 밖에 차이나지 않는다.
개발 기간을 생각하면 HttpURLConnection은 HTTP/1.1이 아니라 HTTP/1.0을 보고 만들었을것이다.
HTTP/1.0에는 HEAD, GET, POST뿐이니 request body가 붙은 요청을 POST로 바꾸는게 멍청한 구현이 아니다.
HTTP/1.1이었다면 request body가 붙는 요청이 POST말고도 PUT, PATCH도 있으니까 멋대로 GET을 POST로 바꾸면 안된다.

HTTP 코드랑 구현체의 주석을 보니 초기에 만든 동작을 그대로 유지시켰나보다.

### HTTP method TRACE doesn't support output

wikipedia의 [Hypertext Transfer Protocol][wiki-http]의 summary table을 보면 TRACE method은 body 안붙는게 표준이다.

![summary table](http-summary-table.png)

### Invalid HTTP method: PATCH

HTTP request method중에서 PATCH는 특별하다. 스펙 문서부터 다르다.
PATCH는 RFC5789 [PATCH Method for HTTP][rfc-5789-patch-method]에 정의되어있다.
게다가 PATCH는 request body 규격도 정의되어있다.
RFC6902 [JavaScript Object Notation (JSON) Patch][rfc-6902-patch-notation]를 보면 아래의 코드가 들어있다.

```json
[
  { "op": "test", "path": "/a/b/c", "value": "foo" },
  { "op": "remove", "path": "/a/b/c" },
  { "op": "add", "path": "/a/b/c", "value": [ "foo", "bar" ] },
  { "op": "replace", "path": "/a/b/c", "value": 42 },
  { "op": "move", "from": "/a/b/c", "path": "/a/b/d" },
  { "op": "copy", "from": "/a/b/d", "path": "/a/b/e" }
]
```

POST, PUT 같은건 request body 규격으로 바가지 안긁는다. 상세 구현은 알아서 적당히 하면 된다.
PATCH는 리소스 변경을 목적으로 나왔느니 조금 더 신경쓴 느낌이다.

HttpURLConnection에서 PATCH를 지원하지 않는걸보니 최신 스펙에 맞춰서 유지보수하는 클래스가 아닌가보다.
힙스터가 자바에서 HTTP 통신 사용할일 있으면 HttpURLConnection 쓰면 안되겠다.

## reference

* [MDN HTTP request methods][mdn-http-methods]
* [Hypertext Transfer Protocol -- HTTP/1.0][rfc-1945-http10]
* [Hypertext Transfer Protocol -- HTTP/1.1][rfc-7231-http11]
* [PATCH Method for HTTP][rfc-5789-patch-method]
* [JavaScript Object Notation (JSON) Patch][rfc-6902-patch-notation]
* [Hypertext Transfer Protocol -- HTTP 1.1, 한국전자통신연구소 번역][rfc-2616-kor]

[mdn-http-methods]: https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
[rfc-1945-http10]: https://tools.ietf.org/html/rfc1945
[rfc-7231-http11]: https://tools.ietf.org/html/rfc7231
[rfc-5789-patch-method]: https://tools.ietf.org/html/rfc5789
[rfc-6902-patch-notation]: https://tools.ietf.org/html/rfc6902
[rfc-2616-kor]: RFC2616.pdf
[java-httpurlconnection]: https://docs.oracle.com/javase/8/docs/api/java/net/HttpURLConnection.html
[wiki-rest]: https://en.wikipedia.org/wiki/Representational_state_transfer
[wiki-http]: https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
