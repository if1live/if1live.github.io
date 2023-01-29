#include <stdio.h>
#include <math.h>

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float Q_rsqrt( float number )
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

void print(float n) {
  float result_1 = 1 / n;
  float result_2 = Q_rsqrt(n) * Q_rsqrt(n);
  printf("%f %f\n", result_1, result_2);
}

int main() {
  print(1000.0f);
  print(100.0f);
  print(10.0f);
  print(1.0f);
  print(0.1f);
  print(0.01f);
  return 0;
}
