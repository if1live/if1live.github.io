#include <cstdio>
#include <map>
#include <string>

int main()
{
  std::map<std::string, int> foo_dict {
    {"foo", 2},
    {"bar", 4},
  };

  if(foo_dict.count("spam")) {
    printf("count() => %d\n", foo_dict["spam"]);
  } else {
    printf("not found\n");
  }

  printf("after using count, size : %lu\n", foo_dict.size());

  return 0;
}
