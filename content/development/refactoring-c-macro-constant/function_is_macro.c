#include "function_is_macro.h"

static int s_env = 0;

void change_env(int env)
{
  s_env = env;
}

int get_foo()
{
  switch(s_env) {
  case 0:
    return 321;
  case 1:
  default:
    return 4321;
  }
}
