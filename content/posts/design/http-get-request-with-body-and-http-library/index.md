---
title: HTTP GET 요청에 body를 붙여서 보내면 어떤 일이 벌어질까?
subtitle: 다양한 결정, 그리고 꽝
tags: [http, design]
slug: http-get-request-with-body-and-http-library
author: if1live
date: 2019-01-15
url: /posts/http-get-request-with-body-and-http-library
---

## 삽질의 시작

요새 유니티로 게임을 만들고 있다. 나는 서버쪽을 작업하고 있다.
서버는 HTTP 기반으로 구현하고 있다. 실시간 통신이 필요없으면 HTTP 쓰는게 편하잖아?
RESTful API 같은 통신 규격으로 구현하고 있다.
GET, POST, DELETE, PUT 으로 행동을 구분하고 인자는 모두 body에 json을 붙여서 넘겼다.
GET의 경우도 특별한 구분없이 POST랑 똑같은 인터페이스를 유지하도록 했다.
이 접근법은 [elasticseach의 검색 API](https://www.elastic.co/guide/en/elasticsearch/reference/current/search-request-body.html)를 보고 배웠다.

```bash
curl -X GET "localhost:9200/twitter/_search" -H 'Content-Type: application/json' -d'
{
    "query" : {
        "term" : { "user" : "kimchy" }
    }
}
'
```

query string을 쓰면 인자의 타입이 문자열로 인식되니 숫자로 변환해서 처리하는게 귀찮다.
게다가 nested 구조는 query string으로 표현하면 깔끔하게 안나온다.
body에 json붙여서 넘기면 위의 두 문제는 간단하게 해결된다.
이 좋은걸 POST에서만 쓰고 GET에서 안쓸 이유가 있나 싶어서 GET에도 도입했다.
(HTTP 스펙상 GET에 body 안붙이는게 맞다는 소리는 알지만 일단 넘어가자. 이론과 현실은 다르다. elasticsearch같은 사례도 있으니)

서버 API를 적당히 짜고 유니티에도 샘플 씬 만들어서 API 호출 잘 되는거 테스트하고 퇴근했더니...

안드로이드 빌드를 뽑은후 돌리면 404 Not Found가 뜬다고 하더라.

왜죠?

그래서 UnityWebRequest를 붙잡고 삽질을 시작하게 되는데


## 테스트 순서

node.js로 GET, POST를 받을 수 있는 간단한 서버를 구현했다.
서버는 요청이 들어오면 HTTP method와 body를 응답으로 던진다.

```js
const express = require('express');

const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
  res.json({ method: 'GET', body: req.body });
});
app.post('/', (req, res) => {
  res.json({ method: 'POST', body: req.body });
});

const port = 3100;
app.listen(port, () => {
  console.log(`server listen - ${port}`);
});
```

몇가지 HTTP 라이브러리 서버에 body가 붙은 POST, GET 요청을 보내본다.
그리고 POST와 GET으로 보냈을때의 차이가 있는지 확인한다.

## curl

[curl](https://curl.haxx.se/)은 HTTP API 테스트하기 좋은 툴이다.
[postman](https://www.getpostman.com/)같이 더 이쁜 물건도 있지만 curl에 익숙해져서 자주쓰게 되더라.

```bash
#!/bin/bash

curl -X POST http://127.0.0.1:3100/ -H 'Content-Type: application/json' -d '{"foo": 1}' -s
echo ""

curl -X GET http://127.0.0.1:3100/ -H 'Content-Type: application/json' -d '{"foo": 1}' -s
```

curl로는 GET 요청시 body를 붙여서 보낼 수 있다.

```txt
$ bash curl-demo.sh
{"method":"POST","body":{"foo":1}}
{"method":"GET","body":{"foo":1}}
```

## python requests

[requests](http://docs.python-requests.org/en/master/)는 파이썬 쓸때 자주 쓰던 HTTP 라이브러리이다.
파이썬 내장 HTTP 라이브러리는 아무리봐도 사용법이 익숙해지질 않아서....

```python
import requests

uri = 'http://127.0.0.1:3100'

r = requests.post(uri, json={'foo': 1})
print(r.text)

r = requests.get(uri, json={'foo': 1})
print(r.text)
```

requests로는 GET 요청시 body를 붙여서 보낼 수 있다.

```txt
$ pipenv run python requests-demo.py
{"method":"POST","body":{"foo":1}}
{"method":"GET","body":{"foo":1}}
```


## C# HttpClient

C#에는 [HttpClient](https://docs.microsoft.com/en-us/dotnet/api/system.net.http.httpclient) 라는 HTTP 라이브러리가 내장되어있다.
의존성 늘어나는거 보기 싫을때 쓰면 좋을거다.

### .NET Core

```csharp
﻿using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;

[DataContract]
internal class SimpleReq
{
    [DataMember]
    public int foo;
}

class Program
{
    private static readonly HttpClient client = new HttpClient();
    private static readonly string host = "http://127.0.0.1:3100";

    public static string ToJsonString<T>(T data)
    {
        var stream1 = new MemoryStream();
        var ser = new DataContractJsonSerializer(typeof(T));
        ser.WriteObject(stream1, data);

        stream1.Position = 0;
        StreamReader sr = new StreamReader(stream1);
        var jsonBody = sr.ReadToEnd();

        return jsonBody;
    }

    static async Task<bool> RequestCommon(HttpMethod method)
    {
        var body = ToJsonString(new SimpleReq() { foo = 1 });
        var request = new HttpRequestMessage(method, $"{host}/")
        {
            Content = new StringContent(body, Encoding.UTF8, "application/json"),
        };
        var resp = await client.SendAsync(request);

        Console.WriteLine(await resp.Content.ReadAsStringAsync());
        return true;
    }

    static void Main(string[] args)
    {
        var taskPost = RequestCommon(HttpMethod.Post);
        taskPost.Wait();

        var taskGET = RequestCommon(HttpMethod.Get);
        taskGET.Wait();
    }
}
```

.NET Core의 HttpClient를 쓰면 GET 요청시 body를 붙여서 보낼 수 있다.

```txt
$ cd httpclient-dotnet-core
$ dotnet run
{"method":"POST","body":{"foo":1}}
{"method":"GET","body":{"foo":1}}
```



### Unity

내가 진짜로 하고 싶은 작업은 콘솔에서 http 통신하는게 아니다.
게임에서 http 통신 하는게 목적이다.
.NET Core에서 돌려본 코드를 Unity 2018.3.1f1 로 그대로 옮겨봤다.

```csharp
using UnityEngine;

using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;

[DataContract]
internal class SimpleReq
{
    [DataMember]
    public int foo;
}


public class Main : MonoBehaviour
{
    private static readonly HttpClient client = new HttpClient();
    private static readonly string host = "http://127.0.0.1:3100";

    async void Start()
    {
        await RequestCommon(HttpMethod.Post);
        await RequestCommon(HttpMethod.Get);
    }

    public static string ToJsonString<T>(T data)
    {
        var stream1 = new MemoryStream();
        var ser = new DataContractJsonSerializer(typeof(T));
        ser.WriteObject(stream1, data);

        stream1.Position = 0;
        StreamReader sr = new StreamReader(stream1);
        var jsonBody = sr.ReadToEnd();

        return jsonBody;
    }

    static async Task<bool> RequestCommon(HttpMethod method)
    {
        var body = ToJsonString(new SimpleReq() { foo = 1 });
        var request = new HttpRequestMessage(method, $"{host}/")
        {
            Content = new StringContent(body, Encoding.UTF8, "application/json"),
        };
        var resp = await client.SendAsync(request);

        Debug.Log(await resp.Content.ReadAsStringAsync());
        return true;
    }
}
```

유니티에서 HttpClient 쓸때 GET 요청에 body를 붙이면 예외가 발생한다.
.NET Core랑 Unity의 HttpClient는 이름만 같고 다른 물건인가보다.

```txt
{"method":"POST","body":{"foo":1}}
ProtocolViolationException: Cannot send data when method is: GET
```

## C# RestSharp

C# 내장 라이브러리를 사용하니 .NET Core와 유니티에서의 동작이 달라졌다.
외부 라이브러리를 쓴다면 .NET 런타임에 따른 문제를 피할 수 있을 것이다.
대충 검색하니 [RestSharp](http://restsharp.org/)를 많이 쓰는거같더라.

```csharp
﻿using System;
using RestSharp;
using System.Threading.Tasks;

// RestSharp -Version 106.6.6

class SimpleReq
{
    public int foo;
}

class Program
{
    private static readonly string host = "http://127.0.0.1:3100";

    static async Task<bool> RequestCommon(Method method)
    {
        var client = new RestClient(host);
        var request = new RestRequest("/", method);
        request.AddJsonBody(new SimpleReq() { foo = 1 });

        var resp = await client.ExecuteTaskAsync(request);
        Console.WriteLine(resp.Content);
        return true;
    }
    static void Main(string[] args)
    {
        var taskPOST = RequestCommon(Method.POST);
        taskPOST.Wait();

        var taskGET = RequestCommon(Method.GET);
        taskGET.Wait();
    }
}
```

RestSharp를 쓰면 GET 요청시 body가 전달되지 않는다.

```txt
$ cd restsharp-demo
$ dotnet run
{"method":"POST","body":{"foo":1}}
{"method":"GET","body":{}}
```

## UnityWebRequest

Unity에는 [UnityWebRequest](https://docs.unity3d.com/ScriptReference/Networking.UnityWebRequest.html)라는 HTTP 라이브러리가 내장되어있다.
이 글을 쓰게 된 계기이다.
얼마나 재밌는 물건인지 보자.

```csharp
﻿using System.Collections;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Text;
using UnityEngine;
using UnityEngine.Networking;

[DataContract]
internal class SimpleReq
{
    [DataMember]
    public int foo;
}


public class Main : MonoBehaviour
{
    private static readonly string host = "http://192.168.200.20:3100";

    void Start()
    {
        StartCoroutine(BeginStart());
    }

    IEnumerator BeginStart()
    {
        yield return RequestCommon(UnityWebRequest.kHttpVerbPOST);
        yield return RequestCommon(UnityWebRequest.kHttpVerbGET);
    }


    IEnumerator RequestCommon(string method)
    {
        var body = ToJsonBinary(new SimpleReq() { foo = 1 });
        var www = new UnityWebRequest(host);
        www.method = method;
        www.uploadHandler = new UploadHandlerRaw(body);
        www.uploadHandler.contentType = "application/json";
        www.downloadHandler = new DownloadHandlerBuffer();
        yield return www.SendWebRequest();

        Debug.Log(www.downloadHandler.text);
    }

    public static byte[] ToJsonBinary<T>(T data)
    {
        var stream1 = new MemoryStream();
        var ser = new DataContractJsonSerializer(typeof(T));
        ser.WriteObject(stream1, data);

        stream1.Position = 0;
        StreamReader sr = new StreamReader(stream1);
        var jsonBody = sr.ReadToEnd();

        byte[] byteArray = Encoding.UTF8.GetBytes(jsonBody);
        return byteArray;
    }
}
```

에디터에서 실행하면 GET 요청에 body를 붙일 수 없다.
C#의 RestSharp와 같은 식으로 굴러가나보다.

```txt
{"method":"POST","body":{"foo":1}}
{"method":"GET","body":{}}
```

하지만 안드로이드 빌드를 뽑은후 돌리면 에디터일때와 동작이 바뀐다.
GET 요청시 body를 붙이면 똑똑한 UnityWebRequest가 POST로 보낸다.
body가 붙으면 GET이 아니라 POST로 취급하게 구현했나보다.

```txt
{"method":"POST","body":{"foo":1}}
{"method":"POST","body":{"foo":1}}
```


## 다양한 결정

여러가지 구현체에서 같은 기능이 다르게 동작하는 것을 봤다.
개발하는 애들이 돌대가리는 아닐테니 각각의 결정에는 이유가 있었을거다.

클라이언트에서 GET 요청시 body를 붙일 수 있도록 한 경우를 보자.
curl, requests, .NET Core HttpClient가 이를 선택했다.
HTTP 라이브러리는 아무 생각없이 프로그래머가 시키는대로 돌아간다.
프로그래머가 인자를 잘못 입력해도 크게 신경쓰지 않는다.
GET 요청에 붙어서 날아간 body를 사용할지는 서버 구현에 달려있다.
서버한테 역할을 떠넘겨서 클라이언트가 간단해졌다고 볼 수 있지 않을까?

HTTP 스펙만 따지고 잘못된 구현이라고 하는 사람도 있을 수 있다.
나는 스펙은 일을 편하게 하라고 있는거지 일을 귀찮게 만드려고 존재하는게 아니라고 생각한다.
스펙을 지키고 싶으면 서버에서만 지켜도 되지 클라와 서버 양쪽에서 검증할 필요가 있나 싶다.
HTTP 라이브러리에서 자유도를 보장해주면 elasticsearch같은 물건에서 편하게 쓸 수 있잖아?

elasticsearch의 [문서](https://www.elastic.co/guide/en/elasticsearch/reference/current/common-options.html#_request_body_in_query_string)에 재밌는게 있더라.
request body를 POST가 아닌 요청에 못붙이는 라이브러리에서는 query string을 대신 써도 되도록 했더라.
HTTP 라이브러리에서 요청 보내는 것을 허용하지 않아서 서버의 일이 귀찮아졌다고 볼 수 있지 않을까?

GET 요청시 body가 있으면 무시하는 경우도 생각해보자.
RestSharp가 이것을 선택했다.
잘못된 입력이 들어왔을때 이를 무시하고 기본값을 사용했다고 볼 수 있다.
괜찮은 설계라고 생각한다.
이것을 선택하면 스펙대로 동작하면서 밖에서 봤을때의 인터페이스는 하나로 유지할 수 있다.
위의 샘플 코드를 보면 GET 요청 코드랑 POST 요청 코드를 똑같이 유지할 수 있다.
다만 이런 구현을 선택하는 경우 로그를 넣어야한다고 생각한다.
잘못된 인자를 사용했다는 경로 로그가 없으면 프로그래머는 API를 잘못 쓰고있다는걸 인식하기 어렵다.

GET 요청시 body가 있으면 예외를 던지는 경우도 생각해보자.
Unity의 HttpClient 구현체가 이것을 선택했다.
잘못된 입력이 들어왔을때 예외를 던지는 방식이라고 볼 수 있다.
코드를 잘못 짰을때 개발자가 이를 즉시 알수 있다는 점에서 좋다고 생각한다.
예외를 처리하기 위한 코드가 추가되거나 GET 요청 함수와 POST 요청 함수를 분리해야된다는 단점이 있지만 그 정도는 감수할만하다.

GET 요청시 body가 붙어있으면 POST로 보내는 경우도 생각해보자.
UnityWebRequest가 이것을 선택했다.
개인적으로 이것은 오답이라고 본다.

![하지만 오답은 있단다](programming-and-wrong-answer.jpg)

HTTP GET 요청에 body를 붙이는 것을 다른 관점에서 보자.
HTTP 요청 함수에 HTTP Method와 body를 인자로 넣는다고 볼 수 있다.
각각의 인자 (HTTP Method, body)는 같은 중요도를 갖지 않는다고 생각하다.
body에 비해서 HTTP Method가 중요하다고 생각한다.

body가 붙은 GET 요청을 스펙에 가깝게 처리하고 싶었다면
"body가 붙었으니 프로그래머가 POST를 GET으로 오타냈을 것이다" 보다
"프로그래머는 GET 요청을 보내고 싶었으나 실수로 body를 붙였다" 쪽으로 생각하는게 맞는거 아닐까?

GET 요청에 body를 붙이면 동작이 이상해지는걸 뜯게된 원인은 UnityWebRequest였다.
**GET /user/mydata** 에 body를 붙였더니 UnityWebRequest가 **POST /user/mydata** 로 요청을 보냈다.
그래서 **404 Not Found** 가 발생해서 삽질을 시작했다.
차라리 **GET /user/mydata** 에 body 없이 요청을 보내서 **400 Bad Request** 가 발생했으면 문제를 잡기 쉬웠을거같다.

## 다양한 구현체

C#의 HttpClient가 .NET Core와 유니티에서 동작이 다른 것을 위에서 봤다.
이것과 관련된 이슈가 있다.

[Why does HttpClient in Core allow GET requests with bodies, while Framework version does not?](https://github.com/dotnet/corefx/issues/28135)

C# HttpClient의 동작이 바뀌는 버전을 누가 테스트해봤더라.
유니티는 Mono 기반이고 Mono는 근본이 .NET framework니까 .NET framework의 동작을 따라갔나보다.

* In .NET Core (tested with 1.0 and 2.0), the above executes successfully.
* In .NET framework (tested with 4.7.1, 4.6.1, 4.5), the above throws a ProtocolViolationException with the message Cannot send a content-body with this verb-type on the SendAsync call.

똑같은 코드라고 해도 그것이 돌아가는 구현체는 다를수 있으니 동작이 달라지는건 어쩔수 없는 문제라고 생각한다.
같은 코드를 누가 어디서 어떻게 돌릴지 고정할 방법은 없으니까.


하지만 유니티는 좀 까야한다.
UnityWebRequest는 이름처럼 유니티에서만 돌아가는 라이브러리이다.
유니티에서 직접 만들었을것이다.
그리고 유니티를 쓰는 시점에서 에디터 환경과 안드로이드 환경은 고정된다.
유니티로 빌드를 뽑은 다음에 프로그래머가 .NET 런타임을 갈아끼운다?
유니티 애들은 그딴거 신경 안써도 된다. 에디터, 안드로이드에서 돌아가는 .NET 런타임은 자신들이 통제할 수 있다.
그런데도 에디터랑 안드로이드 빌드에서의 동작이 다르니 뭔가 잘못 구현한 것으로 보인다.
## summary

* API에서 예외 처리 구현할때는 생각하고 만들자.
* 내 생각은 남 생각과 다르다. 생각이 다양하니까 구현도 다양하게 나올 수 있다.
* 유니티는 까야한다.

