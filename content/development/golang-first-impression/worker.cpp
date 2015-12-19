#include <cstdio>
#include <future>
#include <vector>
#include <unistd.h>

int called_from_async(int a) {
  sleep(1);
  printf("async call : %d\n", a);
  return a * 10;
}

int main()
{
  const int loop_count = 3;
  std::vector<std::future<int>> results;

  for(int i = 0 ; i < loop_count ; ++i) {
    std::future<int> result(std::async(called_from_async, i));
    results.push_back(std::move(result));
  }

  printf("generating async complete\n");

  for(auto &result : results) {
    printf("retval : %d\n", result.get());
  }
  return 0;
}
