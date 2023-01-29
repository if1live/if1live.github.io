#include <assert.h>

int mod_256(int x)
{
  if (x >= 0) { return (int)(unsigned char)x; }
  else { return -(int)(unsigned char)-x; }
}

int main()
{
  assert(mod_256(257) == 257 % 256);
  assert(mod_256(-257) == -257 % 256);
  return 0;
}
