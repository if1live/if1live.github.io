---
title: 반복문, 재귀없이 터미널에 다이아 그리기
subtitle: 배열을 이용한 흑마법
tags: [black magic, array, loop]
slug: double-loop-with-array
author: if1live
date: 2016-03-21
url: /posts/double-loop-with-array
---

## 간단한 문제

[극악의 c언어 문제](http://www.todayhumor.co.kr/board/view.php?table=programmer&no=12810)

```
-----

  *
 ***
*****
 ***
  *
-----

for문을 이용해서
별을 출력하는 문제인데
주변에 프로그래밍좀 한다는 친구들도
이 문제는 못풀더군요.
과연 오유 여러분의 실력은 어느정도 될런지..
```

[delete this 이용해서 터미널에 삼각형 그리기]({article}recursion-with-destructor)를 쓰다 예전에 봤던 문제를 다시 꺼내봤다.
그리고 이 문제를 반복문, 재귀호출 없이 풀었던게 생각나서 다시 풀어봤다.

## step 1. 반복문 1개로 삼각형 출력하기
```c
  *
 ***
*****
```

다이아의 위쪽을 먼저 그러보자.
`*****`를 루프없이 그리는 여러가지 방법이 있지만 가장 간단한 `std::string`의 생성자를 이용했다.
공백의 갯수, 별의 갯수를 받아서 출력하는 간단한 함수를 만들고 그것을 이용해서 삼각형을 그렸다.

```cpp
#include <cstdio>
#include <string>

void print_space_and_star(int space, int star) {
  std::string space_str(space, ' ');
  std::string star_str(star, '*');
  printf("%s%s\n", space_str.data(), star_str.data());
}

int main()
{
  const int size = 3;

  for(int i = 1 ; i <= size ; i++) {
    int space_count = size - i;
    int star_count = i * 2 - 1;
    print_space_and_star(space_count, star_count);
  }
  return 0;
}
```

```bash
$ clang++ upper_triangle_one_loop.cpp && ./a.out
  *
 ***
*****
```

## step 2. 반복문 2개로 다이아 그리기 출력하기

```
  *
 ***
*****
 ***
  *
```

step 1에서 반복문 1개로 위쪽 삼각형을 그렸으니까 반복문을 하나 더 이용하면 비슷한 방법으로 아래쪽 삼각형을 그릴수 있을것이다.
그리고 둘을 합쳐놓으면 다이아를 그릴수 있다.

```cpp
#include <cstdio>
#include <string>

void print_space_and_star(int space, int star) {
  std::string space_str(space, ' ');
  std::string star_str(star, '*');
  printf("%s%s\n", space_str.data(), star_str.data());
}

int main()
{
  const int size = 3;

  for(int i = 1 ; i <= size ; i++) {
    int space_count = size - i;
    int star_count = i * 2 - 1;
    print_space_and_star(space_count, star_count);
  }

  for(int i = 1 ; i <= size - 1 ; i++) {
    int space_count = i;
    int star_count = (size - i) * 2 - 1;
    print_space_and_star(space_count, star_count);
  }

  return 0;
}
```

```bash
$ clang++ diamond_two_loop.cpp && ./a.out
  *
 ***
*****
 ***
  *
```

`-N` ~ `+N` 과 절대값을 이용하면 두개의 루프를 하나로 합칠수 있지만 정공법인 관계로 흑마법에서는 다루지 않는다.

## step 3. 반복문 없이 다이아 그리기 출력하기

배열을 루프에 쓴다는 컨셉을 [선구자](https://kldp.org/node/99915#comment-543746)한테 배운적이 있다.
남이 만든 컨셉을 그냥 재탕하면 재미없으니 이것을 조금더 개선해보았다.

Q: 객체로 크기가 N인 배열을 만들면 어떤 일이 벌어지는가?

* 객체의 생성자가 N번 호출된다.
* 객체의 소멸자가 N번 호출된다.

생성자와 소멸자도 결국은 함수 호출이니까 N회 루프 2개를 돌릴수 있다는거 아닌가?
그렇다면 아래의 두 코드는 사실상 같은 역할 아닌가?

```cpp
class Sample {
public:
    Sample() { foo(); }
    ~Sample() { bar(); }
};

Sample spam[3]
```

```cpp
for(int i = 0 ; i < 3 ; i++) {
    foo();
}
for(int i = 0 ; i < 3 ; i++) {
    bar();
}
```

이점을 이용하면 step 2의 코드에서 사용된 2개의 for를 제거할수 있다.

```cpp
#include <cstdio>
#include <string>

void print_space_and_star(int space, int star) {
  std::string space_str(space, ' ');
  std::string star_str(star, '*');
  printf("%s%s\n", space_str.data(), star_str.data());
}

const int size = 3;

int ctor_counter = 1;
int dtor_counter = 1;

class DiamondLine {
public:
  DiamondLine() {
    int i = ctor_counter;
    int space_count = size - i;
    int star_count = i * 2 - 1;
    print_space_and_star(space_count, star_count);
    ctor_counter++;
  }
  ~DiamondLine() {
    int i = dtor_counter;
    if(i != size) {
      int space_count = i;
      int star_count = (size - i) * 2 - 1;
      print_space_and_star(space_count, star_count);
    }
    dtor_counter++;
  }
};

int main()
{
  DiamondLine lines[size];
  return 0;
}
```

```bash
$ clang++ diamond_no_loop.cpp && ./a.out
  *
 ***
*****
 ***
  *
```



## 이것을 왜 기억해야하는가?

파티클 시스템을 만든다고 가정하자.
객체지향을 매우 좋아해서 객체지향적으로 파티클을 만들었다.
파티클 입자 각각을 객체로 만들었다.

```cpp
class Particle {
public:
    Particle() { ... }
    ~Particle() { ... }
};

```

그런데 파티클이 좀 화려해서 터졌다하면 기본 만개 단위로 터진다.

```cpp
Particle particles[10000];
```

만약 파티클의 생성자/소멸자 안에 특정 함수 호출이 있었다면 해당 함수는 만번씩 호출될것이다.
이런 문제때문에 파티클 시스템의 경우는 파티클 입자 각각을 객체로 만들지 않고 전부 묶어서 취급한다.

[CCParticleSystem.h](https://github.com/cocos2d/cocos2d-x/blob/8280f06c1f43bb6bcc493c24970f1a64769ddbb7/cocos/2d/CCParticleSystem.h#L43-L138)

객체로 크기가 큰 배열을 만들때는 생성자/소멸자에서 어떤 일을 하는지, 그것의 부하가 어느정도인지, 객체로 꼭 만들어야하는지를 생각해봐야할 것이다.
