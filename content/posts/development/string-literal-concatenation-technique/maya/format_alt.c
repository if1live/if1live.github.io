#include <stdio.h>
int main()
{
  #define NAME_COLUMN_FMT "%6s"
  #define NAME_VAL_FMT "%6s"
  #define SCORE_COLUMN_FMT "%6s"
  #define SCORE_VAL_FMT "%6.1f"
  #define CODE_COLUMN_FMT "%6s"
  #define CODE_VAL_FMT "%6d"

  printf(NAME_COLUMN_FMT
         SCORE_COLUMN_FMT
         CODE_COLUMN_FMT "\n", "name", "score", "code");

  #define LINE_FORMAT NAME_VAL_FMT SCORE_VAL_FMT CODE_VAL_FMT "\n"
  printf(LINE_FORMAT, "foo", 24.5, 10);
  printf(LINE_FORMAT, "bar", 12.4, 5);
  printf(LINE_FORMAT, "spam", 89.2, 8);
  return 0;
}
