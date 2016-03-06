#include <stdio.h>
#include <assert.h>
const char *hello(const char *name)
{
  assert(name != NULL);
  static char msg[256];
  sprintf(msg, "Hello, %s", name);
  return msg;
}

int main()
{
  printf("%s\n", hello("(null)"));
  printf("%s\n", hello(NULL));
  return 0;
}
