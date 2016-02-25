#include <cstdio>
#include <map>
#include <string>

int main()
{
  std::map<std::string, int> foo_dict {
    {"foo", 2},
    {"bar", 4},
  };

  auto val = foo_dict["spam"];
  printf("[] => %d\n", val);
  printf("after using [], size : %lu\n", foo_dict.size());

  return 0;
}
