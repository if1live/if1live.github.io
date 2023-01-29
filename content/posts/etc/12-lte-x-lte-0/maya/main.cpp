#include <cstdio>
#include <cassert>

bool check(int x) {
  auto a = (12 <= x <= 0);
  auto b = (12 <= x <= 7);
  return a && b;
}

int main(void) {
  // invalid
  assert(check(12) == false);

  // valid
  assert(check(11));
  assert(check(10));
  assert(check(0));
  assert(check(1));

  return 0;
}
