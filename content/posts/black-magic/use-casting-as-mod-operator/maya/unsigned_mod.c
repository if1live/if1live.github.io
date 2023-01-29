#include <assert.h>

unsigned int mod_256(unsigned int x)
{
  return (int)(unsigned char)x;
}

unsigned int mod_256_simple(unsigned int x)
{
  return x % 256;
}

int main()
{
  assert(mod_256(257) == mod_256_simple(257));
  return 0;
}
