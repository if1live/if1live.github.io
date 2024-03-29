---
title: DOOM에서 배우는 atan2(y, x) 구현
subtitle: 386, 486 에서도 돌아가는 atan2
date: 2016-05-15
tags: [fixed point, atan2]
slug: atan2-doom-version
author: if1live
url: /posts/atan2-doom-version
---

## 개요

sin, cos가 있으면 각도를 벡터로 변환할 수 있다.
반대로 벡터를 각도로 변환할때는 [atan2][c_atan2]를 쓰면 된다.
DOOM에서는 atan2를 어떻게 구현했나 코드를 뒤져봤다.

아래의 내용을 읽기전에 [DOOM에서 배우는 각도 표현하는 방법과 sin, cos 구현법]({{< ref angle-and-sine-doom-version >}})를 읽어보는 것을 권장한다.

## 어떻게 돌아가는가?
DOOM에서 `atan2(y, x)`는 몇단계에 걸쳐서 계산된다.

1. tangent -> angle 로 변환할수 있는 Lookup table을 준비한다.
2. `atan2(y, x)`를 lookup table의 인덱스로 변환하는 함수를 만든다. 0~45도만 지원하면 된다.
3. 0~45도에서만 돌아가는 `atan2(y, x)`를 이용해서 나머지 각도에서도 돌아가도록 만든다.

## In DOOM Source
### lookup table
https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/tables.h#L74-L84

```c
#define SLOPERANGE	   2048
#define SLOPEBITS	11
#define DBITS	(FRACBITS-SLOPEBITS)

typedef unsigned angle_t;

// Effective size is 2049;
// The +1 size is to handle the case when x==y
//  without additional checking.
extern angle_t			tantoangle[SLOPERANGE+1];
```

tantoangle는 0~45도 구간의 tangent를 angle로 변할할수 있는 lookup table이다.
정확히 45도(x==y) 일때도 대응할수 있도록 크기를 `SLOPERANGE+1`로 잡았다.

https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/tables.c#L1870

```c
angle_t tantoangle[2049] =
{
    0,333772,667544,...
    ...
    536870912
};
```

lookup table에서 `tantoangle[0]` = 0 = 0 Degree 이다.
`tantoangle[2048]` = 536870912 = 0x2000 0000 = 45 Degree 이다.

### atan2(y, x) -> lookup table index
https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/tables.c#L49-L62

0~45도 구간에서 atan2(y, x)를 lookup table에서의 인덱스로 변환하는 함수이다.

```c
int
SlopeDiv
( unsigned  num,
  unsigned  den)
{
    unsigned   ans;

    if (den < 512)
        return SLOPERANGE;

    ans = (num<<3)/(den>>8);

    return ans <= SLOPERANGE ? ans : SLOPERANGE;
}
```

코드를 한줄씩 해석하자.

`SlopeDiv(unsigned num, unsigned den)` :
`atan2(y, x)` num=y, den=x와 같은거다. 아래의 설명에서는 num, den보다 x, y를 사용할거다.
이 함수는 x >= 0, y >= 0, x >= y 상태에서 굴러간다.

`if (den < 512) { return SLOPERANGE; }` :
unsigned는 사실 `fixed_t` 이다. `fixed_t`는 Q16.16 규격의 고정소수점이다.
`fixed_t 512`는 고정소수점 `Q16.16(0x 00 00.01 00)`으로 `2 ** -8 = 0.00390625`이다.
`SLOPERANGE`는 tantoangle lookup table의 마지막 요소의 인덱스이다.

코드를 다시 쓰면 `if(x < 0.00390625) { return tantoangle_LAST_INDEX; }`이다.
x가 매우 작은 경우는 적당히 예외 처리한 것으로 보인다. 하는김에 den=0를 막아서 divided by zero도 막고.

`ans = (num<<3)/(den>>8);` :
실제로 계산하고 싶은 내용은 `ans = (y / x) * (2 ** 11)`이다.
`y/x`로 tangent를 구하고 `2048=2 ** 11`를 곱하면 0~2048사이의 값이 된다. (x >= 0, y >= 0, x >= y라면 0~45도 구간이다. 해당 구간에서 tangent는 0~1이다)
이 값을 lookup table의 인덱스로 이용할수 있다..... 는 이론상 그런거고 현실에서는 다르다.

x, y는 unsigned int이다. 그리고 x >= y이다. 그리고 float는 쓰지 않았다.
x==y를 제외한 모든 경우에서 `y/x = 0`이 되어버린다.
그렇다고 `(y << 11) / x)`를 계산하면 y가 별로 크지 않아도 오버플로우가 발생할수 있다 (32=2**5, 32<<11 = 1<<16, overflow!)
그래서 DOOM은 편법을 썻다.

1. `2 ** 11` = `(2 ** 3) / (2 ** -8)`
2. `den >> 8` : den은 512(2**9)보다 크거나 같다. 그렇기때문에 8비트를 오른쪽으로 shift해도 0이 되진 않을거다. (divided by zero 발생 안함)
3. `num << 3` : num을 너무 큰수로만 쓰지 않으면 오버플로우가 발생하지 않을거다. (`num << 11`보다는 안전하다)

`return ans <= SLOPERANGE ? ans : SLOPERANGE;` : ans가 tantoangle 배열 범위를 벗어나지 않도록 한번더 확인한다.


### 0~45도 이외에서도 atan2(y, x) 돌아가도록

https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/r_main.c#L835-L836

```c
viewx = player->mo->x;
viewy = player->mo->y;
```

viwex, viewy는 계산의 기준점(플레이어의 위치)이다.

https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/r_main.c#L291-L374

```c
angle_t
R_PointToAngle
( fixed_t x,
  fixed_t y )
{
    x -= viewx;
    y -= viewy;

    if ( (!x) && (!y) )
        return 0;

    if (x>= 0) {
        // x >=0
        if (y>= 0) {
            // y>= 0

            if (x>y) {
                // octant 0
                return tantoangle[ SlopeDiv(y,x)];
            } else {
                // octant 1
                return ANG90-1-tantoangle[ SlopeDiv(x,y)];
            }

        } else {
            // y<0
            y = -y;

            if (x>y) {
                // octant 8
                return -tantoangle[SlopeDiv(y,x)];
            } else {
                // octant 7
                return ANG270+tantoangle[ SlopeDiv(x,y)];
            }
        }
    } else {
        // x<0
        x = -x;

        if (y>= 0) {
            // y>= 0
            if (x>y) {
                // octant 3
                return ANG180-1-tantoangle[ SlopeDiv(y,x)];
            } else {
                // octant 2
                return ANG90+ tantoangle[ SlopeDiv(x,y)];
            }
        } else {
            // y<0
            y = -y;
            if (x>y) {
                // octant 4
                return ANG180+tantoangle[ SlopeDiv(y,x)];
            } else {
                // octant 5
                return ANG270-1-tantoangle[ SlopeDiv(x,y)];
            }
        }
    }
    return 0;
}
```

코드는 길지만 별거없다. [360도를 8조각][wiki_octant]으로 나눈것뿐이다.
그리고 각각의 조각에 대해 따로 처리했다.
0~45도에서만 돌아가는 SlopeDiv(y, x)를 구하기 위해서 x, y는 죄다 절대값을 사용하고 일부 각도 구간에서는 x, y축을 뒤집기도 했다.

* octant 0 (0~45deg) : tantoangle[ SlopeDiv(y,x)]
* octant 1 (45~90deg) : ANG90-1-tantoangle[ SlopeDiv(x,y)]
* octant 2 (90~135deg) : ANG90+ tantoangle[ SlopeDiv(x,y)]
* octant 3 (135~180deg) : ANG180-1-tantoangle[ SlopeDiv(y,x)]
* octant 4 (180~225deg) : ANG180+tantoangle[ SlopeDiv(y,x)]
* octant 5 (225~270deg) : ANG270-1-tantoangle[ SlopeDiv(x,y)]
* octant 6 (270~315deg) : ANG270+tantoangle[ SlopeDiv(x,y)]
* octant 7 (315~360deg) : -tantoangle[SlopeDiv(y,x)]


[c_atan2]: http://en.cppreference.com/w/c/numeric/math/atan2
[wiki_octant]: https://en.wikipedia.org/wiki/Octant_%28plane_geometry%29
