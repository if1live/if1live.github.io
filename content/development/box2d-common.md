Title: Box2D Common Module
Tags: box2d
Slug: box2d-common
Author: if1live
Date: 2013-02-12

## Math
### InvSqrt
```cpp
/// This is a approximate yet fast inverse square-root.
inline float32 b2InvSqrt(float32 x)
{
     union
     {
          float32 x;
          int32 i;
     } convert;

     convert.x = x;
     float32 xhalf = 0.5f * x;
     convert.i = 0x5f3759df - (convert.i >> 1);
     x = convert.x;
     x = x * (1.5f - xhalf * x * x);
     return x;
}
```

quake3 이후로 계속 많이 쓰인다는 [Fast Inverse square root](http://en.wikipedia.org/wiki/Fast_inverse_square_root) 함수.
처다보기만해도 정신이 우주로 가능 magic number가 내장되어잇다. box2d는 zlib니까 적절히 갖다쓰면 될듯하다.



### IsValid
```cpp
/// This function is used to ensure that a floating point number is
/// not a NaN or infinity.
inline bool b2IsValid(float32 x)
{
     if (x != x)
     {
          // NaN.
          return false;
     }

     float32 infinity = std::numeric_limits<float32>::infinity();
     return -infinity < x && x < infinity;
}
```

NaN이건 INF이건 둘다 못쓰는 숫자니까 함수 하나로 동시에 검사하는편이 대체로 유용하다

### etc
box2d 안에서 사용하는 행렬 클래스는 열 우선이다. (opengl과 동일한 배치). 다음과 같은 식으로 매칭된다. 행렬은 내부에 데이터로 배열 자체를 가지는게 아니라 벡터를 가진다. 이렇게하면 행벡터를 바로 얻을수잇으니까 일부 연산은 더 간단해진다.

  |
------------- | -------------
a11=ex.x      | a12=ey.x
a21=ex.y      | a22=ey.y

Mat22::Solve => A*x=b 인 방정식을 푸는 함수이다. (A는 행렬, b는 벡터)

각도를 표현하기 위한 클래스로 b2Rot가 잇다. 생성자로 각도를 받아서 sin, cos를 미리 계산해놓는다. 각도 방향을 뜻하는 x-axis, 직각방향인 y-axis를 얻을수 잇다. atan2(sin, cos) 를 사용해서 각도를 다시 얻을수 있다.

box2d body를 가지고 놀때 계속 볼수잇는 b2Transform도 정의되어잇다. 위치, 각도를 저장해서 물체의 회전/이동 변환을 저장한다.

b2Cross 함수가 잇는데 이는 2차원 벡터를 기반으로 외적을 수행한다. 나도 하나 만들어놓으면 은근히 편할듯?

### Timer
그냥 플랫폼별 시간얻는 함수 Wrapper. 크로스 플랫폼 코딩할때 적절히 복붙해서 쓰면 유용할듯?

### Settings
b2Alloc, b2Free 함수를 사용해서 malloc, free를 대신할수 잇도록해놓앗다. box2d사용자가 적절히 할당자를 바꿔치지할수 잇도록햇다.

```cpp
struct b2Version
{
     int32 major;          ///< significant changes
     int32 minor;          ///< incremental changes
     int32 revision;          ///< bug fixes
};
```

## 자료구조
* [BlockAllocator]({filename}box2d-block-allocator.md) 참고
* [StackAllocator]({filename}box2d-stack-allocator.md) 참고
* GrowableStack

```cpp
template <typename T, int32 N>
class b2GrowableStack { .....
```

고정크기 배열과 동적할당을 적절히 섞어서 구현한 스택이다. 스택의 요소가 N보다 작을때는 고정크기 배열을 스택으로 사용한다. 너무 많이 push해서 고정크기 배열로 커버할수 없으면 동적할당으로 이전의 2배 크기의 배열을 잡고 그것을 스택으로 사용한다. box2d는 stl을 쓰지 않앗기 때문에 이거를 만든거같다. stl쓰면 vector로 어느정도 대신할수 잇을듯하다(하지만 vector를 쓰더라도 컴파일시점에 고정크기 배열로 크기를 잡는건 아마 안될거야)
