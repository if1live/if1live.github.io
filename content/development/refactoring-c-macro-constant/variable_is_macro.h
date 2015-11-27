#ifndef __VARIABLE_IS_MACRO_H__
#define __VARIABLE_IS_MACRO_H__

#define PROJ GlobalVariable::proj
#define NAME GlobalVariable::name
#define VERSION GlobalVariable::version

class GlobalVariable {
public:
  static const char *proj;
  static const char *name;
  static const int version;
};

#endif
