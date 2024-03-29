---
title: 3차원 공간에 2차원 객체 렌더링
tags: [opengl, gl, cg, text, billboard]
slug: text-billboard
author: if1live
date: 2013-04-21
url: /posts/text-billboard
---
![screen shot](screenshot.png)

디버깅 용도(예를 들면, 물체의 좌표값, 게임객체의 상태등을 표시)로 3차원 공간상에 2차원 객체(텍스트 포함)를 그냥 출력하고 싶다. 이렇게 사용하기 위해서는 세가지 조건을 만족해야한다.

첫번째는 카메라의 위치가 달라지더라도 빌보드처럼 기울어지지 않아야한다. 카메라 방향따라서 텍스트 방향이 달라지면 기울어지거나 뒤집때도 있을텐데 이걸 어떻게 읽을 수 있겠는가?

두번째는 거리에 상관없이 동일한 크기로 나와야한다. 디버깅 용도인데 멀리있는 텍스트라고 작게나오고 가까이에 있다고 크게 나올 필요가 있는가? 읽기 쉬우려면 멀리있건 가까이에있건 동일한 크기로 나오는편이 좋다

세번째는 3차원 공간상의 Z좌표까지 유지되어야한다는 점이다. Z좌표를 씹고 렌더링하면 텍스트의 깊이감이 사라져서 제대로된 좌표에 렌더링된것인지 확인하기 어렵다. (상황에 따라서는 깊이를 무시하고 렌더링하는게 필요할때도 있겠지만 그것을 기본값으로 하는건 좀 어색하다)

이를 적절히 구현한 예제소스를 정리해서 공개한다.

## Repo
* [Repo][repo]

## 설명

### 준비사항
* Projection / View / Model Matrix를 적절히 관리한다. 좌표계산이 이것이 필요하다.
행렬을 따로 관리하지 않고 고정파이프라인 함수호출로 바로 사용한 경우, ```glGetFloatv(GL_PROJECTION_MATRIX, Persmatrix); glGetFloatv(GL_MODELVIEW_MATRIX, MVmatrix);```를 통해서 행렬을 얻어올수 있지만, 속도가 많이 느려지기 때문에 추천하지 않는다

### 단계별 설명
1. Projection / View / Model Matrix를 합쳐서 Projection-View-Model Matrix(이하 MVP Matrix를 만든다. 좌표계산에서 각각의 행렬을 독립적으로 사용하진 않으니까 묶어놓는게 간단하다)

2. 3차원 좌표를 최종 렌더링 결과 좌표로 바꾼다. MVP Matrix * vec4(x, y, z, 1)을 하면 구할 수 있다.

3. 동차좌표계 변환. w속성은 달고있어봣자 복잡하기만 하다. w=1로 바꿔주기 위해서 변환된 3차원 좌표의 각 속성을 w로 나눠준다.

4. OpenGL 좌표계는 화면에서 나오는 방향이 +Z이다. 그런데 Projection 좌표계에서는 화면에서 나오는 방향이 -Z이다. OpenGL 좌표계를 가지고 2차원 객체를 렌더링할거니까 z값을 뒤집는다

5. Projection 좌표계에서 보여질 영역은 x=-1~+1, y=-1~+1, z=-1~+1이다. 하지만 2차원 객체는 x=0~ScreenWidth, y=0~ScreenHeight의 영역에서 렌더링한다. (그래야 2차원 객체의 크기를 픽셀좌표계로 맞출수있으니까) z값의 경우는 ortho의 NearZ와 FarZ만 -1~1로 Projection 좌표계와 동일한 범위로 유지시켜주면 건드릴 필요 없다.

6. 계산된 Projection Matrix / 좌표는 다음과 같다. 이를 이용해서 적절히 렌더링하면 된다

7. ortho
    * left = 0
    * width = ScreenWidth
    * bottom = 0
    * height = ScreenHeight
    * nearZ = -1
    * farZ = 1
8. pos
    * pos_x = (x + 1) * 0.5 * ScreenWidth
    * pos_y = (y + 1) * 0.5 * screenHeight
    * pos_z = z


## 구현 핵심 코드
```cpp
void RenderBillboardLabel(haruna::gl::Label &label, float x, float y, float z)
{
  mat4 mvp = g_proj_mat * g_view_mat * g_model_mat;

  //billboard 같은 느낌으로 글자 쓰기
  //기울어지는거 없이 항상 글자가 뜨도록 적절히 만들기
  // rendering pipeline통과 후 좌표값
  vec4 cliping_pos = mvp * vec4(x, y, z, 1);
  cliping_pos /= cliping_pos.w;
  cliping_pos.z = -cliping_pos.z; //보정된 좌표계는 z방향 다르다

  // -1~+1로 보정된 좌표를 윈도우좌표로 변환
  vec3 win_coord(
    (cliping_pos.x+1) * kWidth/2.0f,
    (cliping_pos.y+1) * kHeight/2.0f,
    cliping_pos.z
    );

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, kWidth, 0, kHeight, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(win_coord.x, win_coord.y, win_coord.z);

  glVertexPointer(2, GL_FLOAT, sizeof(FontVertex), &label.vertex_list()[0].p);
  glTexCoordPointer(2, GL_FLOAT, sizeof(FontVertex), &label.vertex_list()[0].uv);
  glDrawElements(GL_TRIANGLES, label.index_count(), GL_UNSIGNED_SHORT, label.index_data());

  {
    // restore 3d matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(g_proj_mat));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 modelview = g_view_mat * g_model_mat;
    glLoadMatrixf(glm::value_ptr(modelview));
  }
}
```

[repo]: https://github.com/if1live/libsora.so-src/tree/master/text_billboard
