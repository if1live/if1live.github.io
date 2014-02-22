Title: Vincent Font Library
Tags: opengl, gl, cg, text, library
Slug: vincent-font-library
Author: if1live
Date: 2013-04-19

Public Domain 8x8 Bitmap Font인 [Vincent Font][vincent]를 적절하게 사용하기 위해서 만든 코드를 정리했다.
앞으로는 새로운 [OpenGL Text Library](|filename|opengl-text-library.md)를 주로 사용하겠지만 왠지 아까워서...

원래 OpenGLES 2.0 환경에서 사용하기 위해서 만들었던거라서 렌더링하는 함수는 따로 없다. (gles 2.0은 쉐이더를 쓰니까 렌더링 부분까지 라이브러리로 넣기 미묘하더라)
라이브러리에서는 폰트를 렌더링 하기위한 Vertex/Texture Coord 까지만 제공하도록했다. 구조는 간단하니까 필요한 사람은 알아서 잘 수정해서 쓸 수 있을것이다.

## [Repo][repo]

데모 스크린샷. 8x8폰트는 상당히 작다.
텍스트라벨의 원점은 왼쪽아래가 0,0이니까 사용할때 참고할 것

![screen shot](|filename|../static/vincent-font-library/screenshot.png)

vincent font 2배 확대한거. 8x8폰트는 그냥보면 상당히 작다.

![vincent](|filename|../static/vincent-font-library/vincent.png)

## 사용 예제 코드
```
void RenderLabel(haruna::gl::Label &label, float x, float y)
{
	glPushMatrix();
	{
		glTranslatef(x, y, 0);
		glVertexPointer(2, GL_FLOAT, sizeof(FontVertex), &label.vertex_list()[0].p);
		glTexCoordPointer(2, GL_FLOAT, sizeof(FontVertex), &label.vertex_list()[0].uv);
		glDrawElements(GL_TRIANGLES, label.index_count(), GL_UNSIGNED_SHORT, label.index_data());
	}
	glPopMatrix();
}
......
//bind font texture + set env
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, g_sysFont->tex_id());

glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//라벨의 왼쪽아래가 0,0이다
RenderLabel(Label(g_sysFont.get(), "This is BaseLine"), 0, SysFont::kFontSize);
RenderLabel(Label(g_sysFont.get(), "SomeText"), 100, 50);

```

## Resource
* [vincent][vincent]
* [Repo][repo]

[vincent]: http://forum.osdev.org/viewtopic.php?f=2&t=22033
[repo]: https://github.com/if1live/libsora.so-src/tree/master/vincent_font
