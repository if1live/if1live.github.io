#include <cstdio>
#include <cstring>
#include <cassert>

class ComparisonResult {
public:
  enum {
    ASC = -1,
    EQUAL,
    DESC
  };

  ComparisonResult(int val) : val(val) {}

  bool operator==(int x) {
    return ((int)val == x);
  }

  bool operator==(bool x) {
    return !((bool)val == x);
  }

  bool operator!=(int x) { return !(*this == x); }
  bool operator!=(bool x) { return !(*this == x); }

  operator bool() {
    return (val == EQUAL);
  }

  static ComparisonResult strcmp(const char *str1, const char *str2) {
    int raw = ::strcmp(str1, str2);
    if(raw < 0) {
      return ComparisonResult(ASC);
    } else if(raw > 0) {
      return ComparisonResult(DESC);
    } else {
      return ComparisonResult(EQUAL);
    }
  }
private:
  int val;
};

ComparisonResult my_strcmp(const char *str1, const char *str2)
{
  return ComparisonResult::strcmp(str1, str2);
}

int main()
{
  assert(my_strcmp("foo", "foo") == 0);
  assert(my_strcmp("foo", "foo") == true);
  assert(my_strcmp("foo", "foo"));

  assert(my_strcmp("foo", "bar") != 0);
  assert(my_strcmp("foo", "bar") != true);
  assert(!my_strcmp("foo", "bar"));

  // compatible with strcmp
  assert(my_strcmp("1", "1") == strcmp("1", "1"));
  assert(my_strcmp("1", "2") == strcmp("1", "2"));
  assert(my_strcmp("2", "1") == strcmp("2", "1"));
  return 0;
}
