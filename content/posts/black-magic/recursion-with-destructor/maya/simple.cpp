#include <cstdio>
#include <string>

int from = 0;
int to = 3;
int counter = from;
class DtorRecursion {
public:
  ~DtorRecursion() {
    if(counter < to) {
      line += "*";
      printf("%s\n", line.data());

      counter += 1;
      delete(this);
    }
  }

private:
  std::string line;
};

int main()
{
  { DtorRecursion loop; }
  return 0;
}
