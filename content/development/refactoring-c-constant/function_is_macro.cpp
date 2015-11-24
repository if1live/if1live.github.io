#include "function_is_macro.h"

static int s_lang = 0;

void CommonVariable::changeLanguage(int lang)
{
  s_lang = lang;
}

const char *CommonVariable::name()
{
  switch(s_lang) {
  case 0:
    return "春日野 穹";
  case 1:
  default:
    return "kasugano sora";
  }
}

const char *CommonVariable::proj()
{
  return "use function and macro";
}

int CommonVariable::version()
{
  return 3;
}
