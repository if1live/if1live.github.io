---
title: Simple Mesh Library
tags: [opengl, gl, cg, mesh]
slug: simple-mesh
author: if1live
date: 2013-04-20
url: /posts/simple-mesh
---
glut에는 Cube / Sphere / Cone / Teapot...등을 렌더링할 수 있는 함수가 존재한다. 하지만 glut를 안쓰고 glfw를 쓰거나 GLES환경에서는 그런거 없다. 간단한 테스트/디버깅용으로 Cube / Sphere 등을 렌더링할수 있으면 유용하긴한데... 이거 하겟다고 본격적인 Model Loader를 추가하는것은 괜히 규모가 큰거 같아서 간단하게 사용할 수 있는 메시 라이브러리를 만들었던걸 정리해봣다.

## 세부 기능

* 지원하는 형태
    * Cube
    * Sphere
    * Teapot
* 출력 형태
    * Wire Frame
    * Mesh(Solid)
* Vertex Format
    * Position
    * Texture Coord
    * Normal

모델을 렌더링하는데 필요한 Vertex List, Index List를 계산해준다. 원래 GLES에서 작업하던 코드라서 렌더링하는 함수는 따로 없다. 예제코드를 참고해서 쓰면 별 문제 없을 것이다.

## Repo

* [Repo][repo]

## Demo

실행하고 1~6을 누르면 아래중에서 적절히 하나가 렌더링된다.

![solid cube](solid_cube.png)

![solid sphere](solid_sphere.png)

![solid teapot](solid_teapot.png)

![wire cube](wire_cube.png)

![wire sphere](wire_sphere.png)

![wire teapot](wire_teapot.png)

## 사용 예제 코드

```
void Draw(std::vector<DrawCmdData<Vertex_1P1N1UV>> &draw_cmd_list)
{
    auto it = draw_cmd_list.begin();
    auto endit = draw_cmd_list.end();
    for( ; it != endit ; ++it) {
        const DrawCmdData<Vertex_1P1N1UV> &draw_cmd =  *it;
        GLenum mode = DrawModeToGLMode(draw_cmd.draw_mode);
        int stride = sizeof(Vertex_1P1N1UV);
        glVertexPointer(3, GL_FLOAT, stride, &draw_cmd.vertex_list[0].p);
        glTexCoordPointer(2, GL_FLOAT, stride, &draw_cmd.vertex_list[0].uv);
        glNormalPointer(GL_FLOAT, stride, &draw_cmd.vertex_list[0].n);
        glDrawElements(mode, draw_cmd.index_list.size(), GL_UNSIGNED_SHORT, &draw_cmd.index_list[0]);
    }
}
......
auto solid_cube_mesh = SolidCubeFactory(1, 1, 1).CreateNormalMesh();
Draw(solid_cube_mesh);
```

## Resource
* [Qualcomm Teapot Source][teapot_data]

[repo]: https://github.com/if1live/libsora.so-src/tree/master/simple_mesh
[teapot_data]: https://code.google.com/p/virtualwimbledonpro/source/browse/jni/Teapot.h?r=9448d1cb72c71b4addf77c4afcce6fa6f671fd2e
