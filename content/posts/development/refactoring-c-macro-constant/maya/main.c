#include <stdio.h>

#ifdef USE_SIMPLE_VER
#include "simple_macro.h"
#endif

#ifdef USE_VARIABLE_VER
#include "variable_is_macro.h"
#endif

#ifdef USE_FUNCTION_VER
#include "function_is_macro.h"
#endif


int main()
{
  printf("%d\n", FOO + 123);

#ifdef USE_FUNCTION_VER
  change_env(1);
  printf("%d\n", FOO + 123);
#endif
  return 0;
}
