#include <stdio.h>
int main()
{
  // 한줄에 출력할 내용을 전부 넣기
  printf("== Help ==\n* option 1\n* option 2\n");

  // 변수에 문자열을 넣은 다음에 출력
  char msg[] = \
    "== Help ==\n"\
    "* option 1\n"\
    "* option 2\n";
  printf("%s", msg);
  return 0;
}
