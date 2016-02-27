#include <stdio.h>
int main()
{
  printf("%6s%6s%6s\n", "name", "score", "code");

  #define LINE_FORMAT "%6s%6.1f%6d\n"
  printf(LINE_FORMAT, "foo", 24.5, 10);
  printf(LINE_FORMAT, "bar", 12.4, 5);
  printf(LINE_FORMAT, "spam", 89.2, 8);
  return 0;
}
