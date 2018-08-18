#include <assert.h>

int mod_256(int x)
{
  return (int)(unsigned char)x;
}

int mod_256_union(int x)
{
  union container_t {
    int a;
    unsigned char b[4];
  };
  union container_t c;
  c.a = x;
  return (int)c.b[0];
}

int main()
{
  assert(mod_256(257) == 257 % 256);
  assert(mod_256_union(257) == 257 % 256);
  return 0;
}
