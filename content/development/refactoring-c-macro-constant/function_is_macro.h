#ifndef __FUNCTION_IS_MACRO_H__
#define __FUNCTION_IS_MACRO_H__

#define NAME CommonVariable::name()
#define PROJ CommonVariable::proj()
#define VERSION CommonVariable::version()

class CommonVariable {
public:
  static const char *name();
  static const char *proj();
  static int version();

  static void changeLanguage(int lang);
};

#endif
