#include <cstdio>
#include <string>

void print_space_and_star(int space, int star) {
  std::string space_str(space, ' ');
  std::string star_str(star, '*');
  printf("%s%s\n", space_str.data(), star_str.data());
}

int main()
{
  const int size = 3;

  for(int i = 1 ; i <= size ; i++) {
    int space_count = size - i;
    int star_count = i * 2 - 1;
    print_space_and_star(space_count, star_count);
  }

  for(int i = 1 ; i <= size - 1 ; i++) {
    int space_count = i;
    int star_count = (size - i) * 2 - 1;
    print_space_and_star(space_count, star_count);
  }

  return 0;
}
