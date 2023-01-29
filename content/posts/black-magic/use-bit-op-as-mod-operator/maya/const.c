#include <assert.h>

unsigned int mod_16(unsigned int a)
{
  return a & (16 - 1);
}

unsigned int mod_16_simple(unsigned int a)
{
  return a % 16;
}

int main() { return 0; }
