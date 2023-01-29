#include <cstdio>
#include <string>

void print_space_and_star(int space, int star) {
  std::string space_str(space, ' ');
  std::string star_str(star, '*');
  printf("%s%s\n", space_str.data(), star_str.data());
}

const int size = 3;

int ctor_counter = 1;
int dtor_counter = 1;

class DiamondLine {
public:
  DiamondLine() {
    int i = ctor_counter;
    int space_count = size - i;
    int star_count = i * 2 - 1;
    print_space_and_star(space_count, star_count);
    ctor_counter++;
  }
  ~DiamondLine() {
    int i = dtor_counter;
    if(i != size) {
      int space_count = i;
      int star_count = (size - i) * 2 - 1;
      print_space_and_star(space_count, star_count);
    }
    dtor_counter++;
  }
};

int main()
{
  DiamondLine lines[size];
  return 0;
}
