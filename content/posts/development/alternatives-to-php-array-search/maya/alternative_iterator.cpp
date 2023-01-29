#include <algorithm>
#include <cstdio>

void check_prime_number(int val)
{
  auto data = {2, 3, 5, 7, 11};
  auto found = std::find(data.begin(), data.end(), val);
  if(found == data.end()) {
    printf("%d is not prime number\n", val);
  } else {
    int idx = std::distance(data.begin(), found);
    printf("%d is prime number, index=%d\n", val, idx);
  }
}

int main()
{
  check_prime_number(5);
  check_prime_number(2);
  check_prime_number(-1);
  return 0;
}
