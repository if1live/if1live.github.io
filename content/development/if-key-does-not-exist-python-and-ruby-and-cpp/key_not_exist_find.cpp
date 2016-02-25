#include <cstdio>
#include <map>
#include <string>

int main()
{
  std::map<std::string, int> foo_dict {
    {"foo", 2},
    {"bar", 4},
  };
  printf("curr dict size : %lu\n", foo_dict.size());

  auto it = foo_dict.find("spam");
  if(it != foo_dict.end()) {
    printf("found, find() => %d\n", 1);
  } else {
    printf("not found\n");
  }
  printf("after using find, size : %lu\n", foo_dict.size());

  return 0;
}
