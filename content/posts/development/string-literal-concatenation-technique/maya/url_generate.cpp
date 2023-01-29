#include <stdio.h>
#include <stdlib.h>

#if _DEBUG
  #define HOST "127.0.0.1"
#else
  #define HOST "libsora.so"
#endif


#define URL_A_FORMAT "http://%s/kasugano"
#define URL_B_FORMAT "http://%s/kasugano/sora"

char url_a[1024];
char url_b[1024];

int length_url_a = sprintf(url_a, URL_A_FORMAT, HOST);
int length_url_b = sprintf(url_b, URL_B_FORMAT, HOST);

int main()
{
  printf("%s %d\n", url_a, length_url_a);
  printf("%s %d\n", url_b, length_url_b);
  return 0;
}
