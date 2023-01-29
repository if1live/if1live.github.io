// Ŭnicode please
#include "example.h"
#include <cstdio>

Example::Example(SampleObj *obj)
  : ptr_obj(obj) {}

Example::~Example() {
  printf("example dtor called\n");
}
