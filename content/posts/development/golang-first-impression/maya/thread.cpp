#include <thread>
#include <vector>
#include <cstdio>

void foo(int a) {
  printf("thread call : %d\n", a);
}

int main() {
  const int loop_count = 3;
  std::vector<std::thread> threads;

  for(int i = 0 ; i < loop_count ; ++i) {
    std::thread t(foo, i);
    threads.push_back(std::move(t));
  }

  for(auto &t : threads) {
    t.join();
  }
  return 0;
}
