// Ŭnicode please
#include <memory>
#include <cstdio>
#include "example.h"

using namespace std;

class SampleObj {
public:
  ~SampleObj() { printf("dtor\n"); }
};

int main() {
  Example *ex = new Example(new SampleObj());
  delete(ex);
  getchar();
  return 0;
}
