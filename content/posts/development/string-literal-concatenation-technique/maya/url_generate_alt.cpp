#include <stdio.h>
#include <stdlib.h>

#if _DEBUG
  #define HOST "127.0.0.1"
#else
  #define HOST "libsora.so"
#endif


#define URL_A "http://" HOST "/kasugano"
#define URL_B "http://" HOST "/kasugano/sora"

// -1 => last '\0'
#define LENGTH_URL_A (sizeof(URL_A) - 1)
#define LENGTH_URL_B (sizeof(URL_B) - 1)

int main()
{
  printf("%s %d\n", URL_A, LENGTH_URL_A);
  printf("%s %d\n", URL_B, LENGTH_URL_B);
  return 0;
}
