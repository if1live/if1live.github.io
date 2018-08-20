#include <assert.h>

unsigned int mod_power_of_two(unsigned int a, unsigned int b)
{
  //assert(b && (b & (b - 1)) == 0);
  return a & (b - 1);
}

unsigned int mod(unsigned int a, unsigned int b)
{
  return a % b;
}

int main()
{
  assert(mod_power_of_two(17, 1) == mod(17, 1));
  assert(mod_power_of_two(17, 2) == mod(17, 2));
  assert(mod_power_of_two(17, 4) == mod(17, 4));
  assert(mod_power_of_two(17, 8) == mod(17, 8));
  return 0;
}
