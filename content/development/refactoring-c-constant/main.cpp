#include <cstdio>

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
  printf("proj: %s\n", PROJ);
  printf("name: %s\n", NAME);
  printf("version: %d\n", VERSION);

#ifdef USE_FUNCTION_VER
  CommonVariable::changeLanguage(1);
  printf("name: %s\n", NAME);
#endif
  return 0;
}
