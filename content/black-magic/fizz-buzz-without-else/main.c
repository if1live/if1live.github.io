#include <stdio.h>

int main(void) {
  for(int i = 1 ; i <= 100 ; i++) {
    int found = 0;
    if(!(i % 3)) { found += printf("Fizz"); }
    if(!(i % 5)) { found += printf("Buzz"); }
    if(!found) { printf("%d", i); }
    printf("\n");
  }
  return 0;
}
