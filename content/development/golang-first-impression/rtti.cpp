/*
  http://webcompiler.cloudapp.net/
  ## vc++'s output
  class Sample
  class Sample *

  ## clang++'s output
  6Sample
  P6Sample
 */

#include <typeinfo>
#include <cstdio>

class Sample {
public:
  Sample() : a(1), b('a') {}
  int a;
  char b;
};

int main()
{
  Sample s1;
  const std::type_info& info1 = typeid(s1);
  printf("%s\n", info1.name());

  Sample *s2 = &s1;
  const std::type_info& info2 = typeid(s2);
  printf("%s\n", info2.name());

  return 0;
}
