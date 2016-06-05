#define USE_EXTENDED_LENGTH 1

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

const std::string dirname  = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123";
const std::string filename = "0123456789012345678901234567890123456789012345678901234567890123456789.txt";

int main()
{
  std::string filepath = R"(c:\)" + dirname + R"(\)" + filename;
#if USE_EXTENDED_LENGTH
  filepath = std::string(R"(\\?\)") + filepath;
#endif

  FILE *f = nullptr;
  errno_t err = fopen_s(&f, filepath.data(), "wb");
  if(err == 0) {
    fprintf_s(f, "hello world\n");
    fclose(f);
  } else {
    fprintf(stderr, "error code : %d\n", err);
  }
  return 0;
}
