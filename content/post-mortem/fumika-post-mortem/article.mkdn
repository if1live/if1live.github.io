---
title: Project Fumika, 중고책 처분 프로세스 개발기
subtitle: 중고책 팔아요
tags: [development note]
slug: fumika-development-note
author: if1live
date: 2017-05-02
url: /posts/fumika-development-note
---

## 개요
* 개발 기간 : 2017/04/15 - 2017/05/30
* 개발 언어 : golang, java(android)
* 목표 : 가장 높은 값에 책을 중고로 처분하자
* 저장소 : [fumika][repo-fumika], [fumika-scanner][repo-fumika-scanner]

나이먹고 집에 빌붙어서 사니까 슬슬 바가지를 긁더라.
가출을 해야겠다는 생각이 들기 시작했다.
가출 계획을 세우다보니 책을 어떻게 처분해야되나 싶어더라.
책이라는 물건이 생각보다 무겁고 공간을 많이 잡아먹으니까.

그래서 책의 수를 줄이기로 했다.

첫번째 방법은 스캔이었는데 귀찮고 비용이 생각보다 많이 깨지더라.
책의 내용을 유지할수 있지만 모든 책을 스캔하는건 현실적으로 무리더라.
이 방식으로 진행하려면 스캔할 책과 스캔하지 않을 책을 분류할 목록이 필요하더라.

두번째 방법은 중고책 서점에 파는거였다.
샘플로 책 몇권 골라서 yes24, 알라딘에서 중고 매각가을 검색해보니 꽤 다르더라.
중고 매각가가 다른것부터 시작해서 중고책을 받아줄지 여부도 다르더라.
그리고 가격을 확인하려고 yes24앱, 알라딘앱으로 바코드 일일히 찍어보는게 귀찮더라.

세번째 방법은 직접 중고책을 파는거였다.
그럴라면 처분 가능한 책 목록을 만들어서 외부에 공유해야 하잖아?
일단 책 목록이 필요하더라.

이런저런 생각을 하다보니 손으로 직접 하긴 귀찮고 자동화시켜서 처리하고싶더라.
그래서 프로젝트 fumika를 시작했다

## 중고책 매입가 검색하는 API 만들자

[알라딘][bookstore-aladin], [Yes24][bookstore-yes24]에서 중고책을 매입해주더라.
앱으로 바코드를 찍으면 중고책 매입가를 알수있지만 책 한권의 가격을 비교하기 위해 앱 두번 켜고 바코드로 찍는건 귀찮더라.
이를 자동화하고 싶었다. ISBN을 넣으면 중고책 가격을 조회해주는 API가 있으면 좋겠더라.
그래서 "중고책 매각가 검색 API", [fumika][repo-fumika]를 만들었다.
중고책 매입가 검색 페이지의 HTML 파싱해서 가격 정보만 뽑았다. 특별한건 없다.

다만 전부 만들고 찾아보니 "중고책 구매/매각가 검색 앱" [UsedBookApp][repo-used-book-app]가 있더라.
구글 검색에서 안잡히고 "매입가"로 검색해도 검색 결과가 없길래 라이브러리가 없는줄 알았다.
만들다보니 "매각가"라는 단어를 알게되어서 github에서 검색해보니 있더라.
내가 생각하는건 남들도 똑같이 생각하고 내가 필요로 하는건 남들도 필요로 한다는걸 확인했다.

## ISBN 바코드를 찍자 - 유니티

ISBN으로 중고책 매입가를 검색하는 API를 만들었다.
이제 ISBN 목록을 구성하면 된다.
그런데 책이 한두권도 아니니 ISBN을 손으로 입력하는건 할짓이 아니다.
바코드 스캐너를 짜기로 했다.

정석적인 방법은 안드로이드 앱으로 바코드 스캐너를 만드는거다.
근데 내가 안드로이드 개발을 마지막으로 해본건 3년전, 게다가 안드로이드 자바는 좋아하는 개발환경이 아니다.
안드로이드로는 짜고싶지 않았다.

안드로이드 개발은 3년동안 안했지만 그동안 개발을 아예 안한건 아니다.
게임개발하면서 유니티는 좀 익숙해졌다.
유니티로 바코드 찍는게 가능하다면 안드로이드로 개발 안해도 되잖아?
찾아보니 [UnityBarcodeScanner][repo-unity-barcode-scanner]라는게 있더라.
이걸 기반으로 바코드 스캐너를 만들기로 했다.

## ISBN 바코드를 구글 스프레드시트에 업로드하자 - 유니티

구글 스프레드시트에 ISBN을 집어넣는 과정은 크게 2개로 나눌수 있다.
첫번째 과정은 인증이다. 두번째 과정은 데이터를 추가하는 것이다.

이전에 [Google Drive for Unity3D][repo-unity-googledrive]를 사용해본적이 있어서 이를 인증에 이용하기로 했다.
인증 자체는 구글 드라이브건 구글 스프레드시트건 OAuth2를 쓴다.
구글 스프레드시트에 접근할수 있도록 권한 Scope를 다르게 설정해서 인증을 통과하고
Access token을 확보하면 나머지는 어떻게든 될거같더라.
수정가능한 scope 기능을 만들어서 [풀리퀘](https://github.com/midworld/unity-googledrive/pull/17) 넣은 적이 있으니 될거같았다.

이전에 해봤던 작업의 연장선이니까 간단하게 끝날줄 알았는데 생각보다 삽질을 많이 했다.

첫번째 삽질은 PC에서 사용하는 인증 과정과 안드로이드에서 굴러가는 인증 과정이 달라서 발생했다.
PC(유니티 에디터 환경)에서는 Google API Console에서 기타 유형을 생성한 OAuth 클라이언트 ID가 필요하다.
클라이언트 ID, 클라이언트 보안 비밀을 이용해서 인증 과정을 진행한다.
하지만 안드로이드에서는 Android 유형으로 OAuth 클라이어트 ID를 만들어야한다.

OAuth 클라이언트 ID 만드는것이 끝이 아니라 apk signing 과정도 필요하다는걸 몰라서 두번째 삽질을 했다.
apk signing 과정을 거치지 않으니까 플러그인 안쪽에서 예외가 발생하더라.

그것으로 삽질은 끝이 아니었다. 하드코딩된 scope가 플러그인 코드에 남아있어서 세번째 삽질을 했다.
scope를 변경하는 기능을 만들고 테스트했지만 이건 PC환경에서만 돌아가는 코드였다.
안드로이드에서는 인증 과정이 다르니까 scope설정하는 부분도 다른곳에 있었다.

[GoogleDrivePlugin.java](https://github.com/midworld/unity-googledrive/blob/def2c16676194762699b16ed781f7f6a5e8a4c16/NativePlugins/Android/GoogleDrivePlugin/src/com/studio272/googledriveplugin/GoogleDrivePlugin.java#L47-L49)
```java
credential = GoogleAccountCredential.usingOAuth2(activity,
    "https://www.googleapis.com/auth/drive.file",
    "https://www.googleapis.com/auth/drive.appdata");
```

자바 코드를 고쳐야하다니! 빌드하는 방법도 모르는데! 게다가 자바 코드 쓰기 싫어서 유니티로 시작한건데!

당연히 우회했다. 왜 내가 이 삽질을 더 해야하는가?
구글 스프레드시트에 데이터를 올리는 과정은 둘로 나눌수 있다고 했다.
인증, 그리고 데이터 추가.
PC에서 인증과정을 거치는건 문제가 없다.
그렇다면 PC에서 인증해서 Access Token을 얻은후 이걸 하드코딩하면 된다.
그래서 세번째 삽질은 우회로 대응했다.

유니티에서 HTTP 요청하는 방법은 잘 몰라서 옛날에 본 코드를 참고했다.
[유니티 Speech-to-Text](https://www.assetstore.unity3d.com/kr/#!/content/69399)와
[수정된 코드](https://pastebin.com/bB6m9AcD)를 섞어서 구현했다.
삽질해본게 하도 많으니까 옛날에 본 코드 섞어도 대충 굴러가더라.

```csharp
public IEnumerator BeginAppendValue(string val) {
  var range = "A1";
  var uri = string.Format("https://sheets.googleapis.com/v4/spreadsheets/{0}/values/{1}:append", SheetID, range);

  // Query parameters
  var qs = new QueryStringBuilder();
  qs.Add("valueInputOption", "USER_ENTERED");
  qs.Add("insertDataOption", "INSERT_ROWS");
  qs.Add("includeValuesInResponse", "true");
  qs.Add("responseValueRenderOption", "FORMULA");
  qs.Add("responseDateTimeRenderOption", "FORMATTED_STRING");
  var querystring = qs.ToString();

  JSONObject row = new JSONObject(JSONObject.Type.ARRAY);
  row.Add(val);

  JSONObject values = new JSONObject(JSONObject.Type.ARRAY);
  values.Add(row);

  JSONObject requestJSON = new JSONObject();
  requestJSON.AddField("values", values);

  var url = uri + "?" + querystring;
  UnityWebRequest www = new UnityWebRequest(url, UnityWebRequest.kHttpVerbPOST);

  www.SetRequestHeader("Authorization", "Bearer " + storage.AccessToken);

  byte[] bytes = Encoding.UTF8.GetBytes(requestJSON.ToString());
  UploadHandlerRaw uH = new UploadHandlerRaw(bytes);
  uH.contentType = "application/json";
  www.uploadHandler = uH;

  www.downloadHandler = new DownloadHandlerBuffer();
  www.SetRequestHeader("Content-Type", "application/json");

  yield return www.Send();

  while(!www.isDone) {
    yield return null;
  }

  var resp = new APIResponse(www);
  LastResponse = resp;
}
```

토큰 직접 넣어서 API 호출하니 문제없이 스프레드시트에 데이터가 들어가더라.


## 유니티 버전 바코드 스캐너의 실패

바코드를 스캔할수도 있고 스캔한 내용을 구글 스프레드시트에 올리는것도 된다.
책의 ISBN을 스캔하기 시작했다.

스캔을 시작하니 생각보다 ISBN이 안찍히더라?
ISBN을 찍으려고 가까이해면 카메라가 흐릿해지고 멀리하면 바코드가 잘 안찍혔다.
아무래도 유니티에서 카메라를 쓰면 자동 초점 기능이 꺼져있는거 같다.
Auto focus를 활성화시키고 바코드를 찍으면 될거같다.

찾아보니 auto focus는 유니티에서 즉시 못 쓰더라.
안드로이드 코드를 짜서 유니티 프로젝트에 집어넣어야 하더라.
또는 20달러짜리 에셋을 사거나.

자바쓰기 싫어서 유니티로 개발시작했는데 자바를 피하는게 불가능하다니.
플러그인 형태로 유니티에 집어넣어야하니 편한것도 아니다.
심심풀이로 시작한 프로젝트에 현질해서 라이브러리 쓰는건 왠지 패배하는 기분이더라.

그래서 자바로 다시 짜기로했다.

## ISBN 바코드를 찍자 - 안드로이드

내가 안드로이드 개발하던 시절에는 이클립스 플러그인와 안드로이드 스튜디오가 공존했다.
지금은 이클립스 플러그인 죽었더라. 그래서 안드로이드 스튜디오 적응하느라 삽질했다.

바코드 스캐너 관련 튜토리얼과 예제코드를 몇개 찾아봤는데 마음에 드는게 없었다.
그러던중 구글에서 직접 만든 예제를 찾았다.
[Sample code for the Android Mobile Vision API][repo-android-vision] 안에 barcode-scanner가 있더라.
필요한 기능이던 Auto Focus도 사용할수 있더라.
이걸 기반으로 원하는걸 만들기로 했다.

## ISBN 바코드를 구글 스프레드시트에 업로드하자 - 안드로이드

샘플코드 보고 구현했다.
Google Sheets API V4 의 [Android Quickstart](https://developers.google.com/sheets/api/quickstaroid) 예제 코드를 뜯어다 붙였다.
apk signing 을 안하면 `GoogleAuthIOException`가 발생한다는 것도 이미 당해봐서 알고있으니 큰 삽질은 하지 않았다.

ISBN 바코드 찍어보니까 잘 찍히더라.
스캐너 구현은 끝났다.

## 책 정보를 입력하자

ISBN으로 책 제목 찾는 기능은 [fumika][repo-fumika]에도 있으니 후미카를 그대로 사용하려고 했다.
하지만 인터넷 서점의 데이터를 믿을수 없어서 그렇게 하지 않았다.
예를 들어 제로의 사역마 3권의 경우 ISBN이 2개이다.

* ISBN [9788926325032](http://www.kyobobook.co.kr/product/detailViewKor.laf?barcode=9788926325032), 2011년 07월 15일 출간
* ISBN [9788953279551](http://www.kyobobook.co.kr/product/detailViewKor.laf?barcode=9788953279551), 2007년 07월 10일 출간

ISBN 9788926325032로는 Yes24, 알라딘에서는 검색 가능하지만 ISBN 9788953279551로는 검색되지 않더라.
인터넷 서점을 ISBN 검색기로 쓸 수 없었다.

도서검색이라면 역시 아마존이지! 하고 아마존에서 ISBN 9788926325032 를 검색해봤는데 책 제목이 영어로 뜨더라.
한국에 출판된 책을 검색하려면 한국 서비스를 쓰는게 정답이겠더라.
그래서 [네이버 책 검색 API](https://developers.naver.com/docs/search/book/)를 이용해서 책 정보를 얻었다.

구글 스프레드시트의 정보에 접근할때는 golang 라이브러리 [spreadsheet][spreadsheet]를 이용했다.
지금까지는 Google API Console에서 "OAuth 2.0 클라이언트 ID"만 사용했는데 이 라이브러리는 "서비스 계정 키"가 필요하더라.
서비스 계정키에는 권한을 어떻게줘야 하나 몰라서 삽질을 했다.
stackoverflow에서 [스프레드시트를 공개로 바꾸면 권한 신경 안써되되지!](http://stackoverflow.com/questions/38949318/google-sheets-api-returns-the-caller-does-not-have-permission-when-using-serve/43432036#43432036)라는 답변을 보고 감동받았다.
이것이 발상의 전환이구나!

그래서 이걸 응용했다.
서비스 계정 키에는 email주소가 있잖아?
스프레드시트의 공유 목록에 email주소 추가하면 되는거 아냐?
해보니까 되더라. 야매 해결책 같긴한데 어쨌든 되니까 신경 끄기로 했다.


## 결과물

![screenshot](https://raw.githubusercontent.com/if1live/fumika-scanner/master/document/final-result.jpeg)

실제로 굴러가는 [스프레드시트][sheet-book]이다.
목록은 만들었으니 어떤걸 중고로 팔지 결정하면 될거같은데 그건 시간날때 진행해도 되겠지.


[bookstore-aladin]: https://www.aladin.co.kr/home/welcome.aspx
[bookstore-yes24]: https://yes24.com

[repo-fumika]: https://github.com/if1live/fumika
[repo-fumika-scanner]: https://github.com/if1live/fumika-scanner
[repo-used-book-app]: https://github.com/lyvius2/UsedBookApp
[repo-spreadsheet]: https://github.com/Iwark/spreadsheet

[repo-unity-googledrive]: https://github.com/midworld/unity-googledrive
[repo-unity-barcode-scanner]: https://github.com/kefniark/UnityBarcodeScanner

[repo-android-vision]: https://github.com/googlesamples/android-vision

[sheet-book]: https://docs.google.com/spreadsheets/d/13IXCsO7FPjhUmOG0bp08xfZXlusdkSER6b33xPMvV9M/edit?usp=sharing
