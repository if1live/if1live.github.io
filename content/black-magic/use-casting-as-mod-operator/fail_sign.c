#include <stdio.h>

int mod_256(int x)
{
  return (int)(unsigned char)x;
}

int main()
{
  printf("expected : %d\n", -1 % 256);
  printf("actual : %d\n", mod_256(-1));
  return 0;
}
