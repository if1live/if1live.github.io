#include <cstdio>
#include <string>
#include <memory>
#include <iostream>

template<int From, int To, typename Runnable>
class DestructorRecursion {
public:
  static const int counter_from = From;
  static const int counter_to = To;

  ~DestructorRecursion() {
    if(_counter < counter_to) {
      _runnable();
      _counter += ((From < To) ? +1 : -1);
      delete this;
    }
  }

private:
  static int _counter;
  static Runnable _runnable;
};

template<int From, int To, typename Runnable>
int DestructorRecursion<From, To, Runnable>::_counter = From;

template<int From, int To, typename Runnable>
Runnable DestructorRecursion<From, To, Runnable>::_runnable;

class TrianglePrinter {
public:
  void operator()() {
    line += "*";
    printf("%s\n", line.data());
  }
private:
  std::string line;
};


int main()
{
  DestructorRecursion<0, 3, TrianglePrinter> foo;
  return 0;
}
