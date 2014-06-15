Title: Uber Shader
Tags: cg, shader
Slug: uber-shader
Author: if1live
Date: 2013-01-27

기본 개념은 한 개의 쉐이더 소스를 짜고 ifdef/ifndef 질을 해서 쉐이더 코드를 여러 형태로 변화시키는 것이다.
이것이 의미가 있는 이유는 여러 종류의 광원모델을 기반으로 쉐이더를 짠다고 하더라도 똑같은 로직/속성의 반복이 등장하기 떄문이다.
(즉, 쉐이더 코드의 중복을 줄일 수 있다.)

## 간단한 예시 

하나의 프래그먼트 쉐이더로 색을 그대로 칠하는것과 텍스쳐와 색을 조합해서 칠하는것을 처리하는 예제코드이다.
다음의 예제코드의 맨 앞부분에 매크로를 집어넣고 쉐이더를 컴파일하면 그 결과물은 각각 다른것이 나온다.

```
void main()
{
	vec4 color = vec4(1, 1, 1, 1);
#if USE_TEXTURE == 1
	gl_FragColor = texture2D(tex, texcoord) * color;
#else
	gl_FragColor = color;
#endif
}
```

### USE_TEXTURE 1

#### input
```
#define USE_TEXTURE 1
// input original source
// ...
```
#### output
```
void main()
{
	vec4 color = vec4(1, 1, 1, 1);
	gl_FragColor = texture2D(tex, texcoord) * color;
}
```

### USE_TEXTURE 0

#### input
```
#define USE_TEXTURE 0
// input original source
// ...
```
#### output
```
void main()
{
	vec4 color = vec4(1, 1, 1, 1);
	gl_FragColor = color;
}
```

## 대체 이걸 어떻게/언제 쓰는가?

내가 간단하게 사용한 방식은 다음과 같다

1. 다음의 속성을 전부 때려박은 광원 모델을 계산하는 쉐이터를 만든다
    * Ambient Color
    * Diffuse Color
    * Diffuse Map
    * Specular Color
    * Specular Map
2. 각각의 속성에 따른 빛 계산 부분을 블럭으로 묶고 위와 같이 ifdef...endif로 잡아준다
3. 어떤 라이팅 모델이 필요할지에 따라서 코드 맨 위에 define을 걸어준다. 만약 Ambient, DiffuseColor, SpecularColor만 사용하고 싶으면 다음과 같은 문자열과 쉐이더 코드를 합쳐서 쉐이더 코드로 사용한다

```
#define USE_AMBIENT_COLOR 1
#define USE_DIFFUSE_COLOR 1
#define USE_DIFFUSE_MAP 0
#define USE_SPECULAR_COLOR 1
#define USE_SPECULAR_MAP 0

// Real Uber-Shader Code
//....
```

기타 광원 효과도 나중에 블럭으로 만들어서 집어넣음으로써 확장이 가능하다.

## 장점
핵심 라이팅 모델은 쉐이더 코드 하나만 유지보수함으로써 끝난다.
쉐이더를 많이 짤 일이 없는 경우는 Uber shader 몇개로 어지간한건 다 떄울수 있겟더라. 
[dEngine](http://fabiensanglard.net/dEngineSourceCodeRelease/index.php)의 경우가 크고 아름다운 UberShader와 몇개의 간단한 쉐이더로
필요한 쉐이더를 전부 때웠다.

## 단점
전처리기 명령이 떡칠되다보니까 쉐이더 코드 유지보수하기가 어렵다.

## Reference
* [내 맘대로 엔진 제작기 - 10(셰이더 Shader)](http://kindtis.tistory.com/202)
* [dEngine: A iOS 3D renderer source code](http://fabiensanglard.net/dEngineSourceCodeRelease/index.php)
