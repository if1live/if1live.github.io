#include <string.h>
#include <stdio.h>

int main() {
  if(strcmp("foo", "foo") == true) {
    printf("same\n");
  } else {
    printf("different\n");
  }
  return 0;
}
