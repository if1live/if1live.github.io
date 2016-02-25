#include <cstdio>
#include <map>
#include <string>

int main()
{
  std::map<std::string, int> foo_dict {
    {"foo", 2},
    {"bar", 4},
  };

  auto it = foo_dict.find("spam");
  if(it != foo_dict.end()) {
    printf("found, find() => %d\n", it->second);
  } else {
    printf("not found\n");
  }
  printf("after using find, size : %lu\n", foo_dict.size());

  return 0;
}
