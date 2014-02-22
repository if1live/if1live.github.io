Title: OpenGL Text Library
Tags: opengl, gl, cg, text, library
Slug: opengl-text-library
Author: if1live
Date: 2013-04-17

OpenGL 자체에는 텍스트 출력 관련된 함수가 없다. 근데 개발과정, 디버깅의 용도로 텍스트 출력이 있었으면 한다.
glut기반으로 개발하고 있다면 glutStrokeString / glutBitmapString 를 사용하면 된다.
하지만 나는 glut를 안쓴다. 혹은 glut를 사용할수 없는 환경(예를 들면 OpenGLES)의 경우에는 어떻게 하면될까?
이 때 선택할 수 있는 방법과 내가 선택한 방법 및 간단하게 만들어본 라이브러리에 대해서 써봣다.

## 요구사항

* 될수있는한 간단하게 만든다
    * 소스를 복붙하는것만으로도 다른 프로젝트에 사용할 수 있으면 좋겠다
    * cpp/h 딱 2개의 파일만 있으면 좋겟다. 파일이 많아지면 복붙해서 관리하기 귀찮다
* 외부 라이브러리 의존성을 최소한으로 줄인다.
    * 개발/디버깅 용도라면 엔진의 밑바닥에 가까운 레이어이다. 낮은 레이어 주제에 외부 라이브러리에 너무 많이 의존하면 다른 프로젝트에 복붙만으로 재사용하기 어렵다.
	* 외부라이브러기 붙으면 linking 신경쓰기 귀찮다
* ASCII만 잘 나오면 된다
    * 개발용 텍스트 라이브러리에 한글출력은 사치다. 그건 나중에..

## 선으로 글자를 그리기

펜으로 글자를 쓰듯이 글자를 그리는거다. 대충 아래 같은 느낌이다.

![glutStrokeString](|filename|../static/opengl-text-library/glutstroke.png)

장점은 구조가 단순하다는거다. 그냥 GL에서 선 렌더링하는게 전부니까.

단점은 글자가 못생겼다는 점(...)과 글자를 그리느라 생각보다 많은 성능을 까먹을 가능성이 있다.

### 구현 참고 자료
글자를 그리기 위한 데이터로 [FreeGLUT][freeglut]를 사용해서 구현해본 적이 있다.
아래 3개의 소스를 잘 복붙하면 만들 수 있다.
나한테는 위의 단점이 크게 느껴져서 한번 해보고 버린 방법이다.

* [fg_font.c][freeglut_func]
* [fg_stroke_roman.c][freeglut_stroke_roman]
* [fg_stroke_mono_roman.c][freeglut_stroke_mono_roman]

## TTF

[FTGLES][ftgles]같은 라이브러리를 통해 TTF폰트를 로딩해서 적절히 렌더링한다.

장점은 어떤크기의 폰트라도 대응할수 있다는 점과 영어 이외의 문자(한글, 일본어, 한자...)도 문제없이 처리할 수 있다는거다.

단점은 외부 라이브러리를 붙여야된다는 점과 폰트파일이 필요하다는거다.

개발초기의 간단한 도구로써 사용하기(예를 들면 FPS/카메라 위치/객체위치 같은 디버깅 정보출력)에는 좀 무리가 있을거 같아서 방법은 채택하지 않았다.

## Bitmap Font (BMP / PNG / ..)

비트맵 폰트 이미지 파일을 텍스쳐로 로딩하고 이것을 적절히 렌더링하는 방법이다. 자세한 설명은 [NEHE Bitmap Fonts][nehe_bitmap_font]를 참고하자.

장점은 ttf로딩보다는 구조가 단순하다는거다.

단점은 이미지 로딩 라이브러리와 비트맵 폰트 이미지가 필요하다는거다. 경로설정 잘못하고 실행하면 이미지파일 못찾는다고 궁시렁대니까 좀 귀찮다. 더 단순한 구조면 좋겟다.

## C Array

이미지를 매우 간단하게 생각하면 픽셀 데이터 배열이다.
그렇다면 이를 C배열로 만들어서 코드에 때려박으면 추가적인 의존성 없이 그냥 갖다쓸 수 있지 않을까?
이런 생각을 가지고 예전에 찾은 헤더파일 하나로 사용할 수 있는 라이센스가 자유로운 폰트로 [Vincent Font][vincent]라는게 있더라.

Vincent Font는 대충 이런느낌으로 생겼다.

![vincent](|filename|../static/opengl-text-library/vincent.png)


작년부터 이 폰트를 가지고 잘 썻는데 폰트의 크기가 8*8 밖에 안되서 너무 작은거같더라.
게다가 폰트가 은근히 가독성이 떨어지고..

그래서 며칠전부터 다른 대안을 찾던 중에 [AntTweakBar][anttweakbar]라는걸 발견했다.
소스를 까보니까 폰트 배열이 있더라. 그리고 가독성이 괜찮더라.
그래서 [AntTweakBar][anttweakbar] 라이브러리에서 폰트부분만 잘라내고 API를 적당히 손봐서 간단한 라이브러리를 만들어봤다.

### AntTweakBar 작은 폰트

![small font](|filename|../static/opengl-text-library/small.png)

### AntTweakBar 중간 폰트

![normal font](|filename|../static/opengl-text-library/normal.png)

### AntTweakBar 큰 폰트

![large font](|filename|../static/opengl-text-library/large.png)

### AntTweakBar 고정폭

![fixed font](|filename|../static/opengl-text-library/fixed.png)

## OpenGL Text Library

적당히 만들어본 라이브러리이다.

* [Repo][textlabel_repo]


![fixed font](|filename|../static/opengl-text-library/demo.png)

데모 소스는 대충 이런 느낌이다. (나중에 소스를 고칠수도 있다. [Repo][textlabel_repo]의 실행예제인 main.cpp를 참고하는게 더 나을거다)

```
// 왼쪽아래를 기준점으로 잡고 렌더링 하는거
g_fontDevice->Set2D(kCoordLeftBottom);
{
	TextLabel label;
	std::string msg1("This is Some Message!");
	TextLabelConfig cfg;
	label.BuildText(msg1, g_DefaultNormalFont.get(), cfg);
	g_fontDevice->DrawText(&label, 0, 0, COLOR32_WHITE, COLOR32_WHITE);
}

//원점이 오른쪽위
{
	TextLabel label;
	std::string msg1("Left-Top Position Move(100, 50)");
	TextLabelConfig cfg;
	cfg.coord = kCoordLeftTop;
	label.BuildText(msg1, normal_font.get(), cfg);
	g_fontDevice->DrawText(&label, 100, 50, COLOR32_WHITE, COLOR32_RED);
}
{
	TextLabel label;
	std::array<std::string, 2> lines;
	lines[0] = std::string("1.Multi---");
	lines[1] = std::string("2.Line---");

	TextLabelConfig cfg;
	cfg.coord = kCoordLeftTop;
	color32 line_colors[] = { COLOR32_BLUE, COLOR32_RED };
	color32 bg_colors[] = { COLOR32_RED, COLOR32_GREEN };
	cfg.line_colors = line_colors;
	cfg.line_bg_colors = bg_colors;
	cfg.bg_width = 100;
	cfg.sep = 10;
	label.BuildText(lines, g_DefaultFixed1Font.get(), cfg);
	g_fontDevice->DrawText(&label, 100, 100, 0, 0);
}
```

## Reference & etc
* [freeglut][freeglut]
* [ftgles][ftgles]
* [anttweakbar][anttweakbar]
* [vincent][vincent]
* [glutstroke_img_ref][glutstroke_img_ref]
* [textlabel_repo][textlabel_repo]
* [nehe_bitmap_font][nehe_bitmap_font]

[freeglut]: http://freeglut.sourceforge.net/
[freeglut_func]: http://freeglut.svn.sourceforge.net/viewvc/freeglut/trunk/freeglut/freeglut/src/fg_font.c?revision=1615&view=markup
[freeglut_stroke_mono_roman]: http://freeglut.svn.sourceforge.net/viewvc/freeglut/trunk/freeglut/freeglut/src/fg_stroke_mono_roman.c?revision=1615&view=markup
[freeglut_stroke_roman]: http://freeglut.svn.sourceforge.net/viewvc/freeglut/trunk/freeglut/freeglut/src/fg_stroke_roman.c?revision=1615&view=markup
[ftgles]: https://github.com/cdave1/ftgles
[anttweakbar]: http://anttweakbar.sourceforge.net/doc/
[vincent]: http://forum.osdev.org/viewtopic.php?f=2&t=22033
[glutstroke_img_ref]: http://sol.cs.hm.edu/rs/jogl-primer/slide0006.html
[textlabel_repo]: https://github.com/if1live/libsora.so-src/tree/master/text_label
[nehe_bitmap_font]: http://nehe.gamedev.net/tutorial/bitmap_fonts/17002/