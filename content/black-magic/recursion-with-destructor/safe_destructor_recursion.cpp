#include <cstdio>
#include <string>
#include <memory>

template<typename Runnable>
class DestructorRecursion {
public:
  typedef DestructorRecursion<Runnable> self_type;
public:
  DestructorRecursion(int from, int to, Runnable &&r)
    : _counter_from(from),
      _counter_to(to),
      _counter(from),
      _runnable(r),
      _deleted(false)
    {}

  ~DestructorRecursion() {
    if(_counter < _counter_to) {
      _runnable();
      _counter += ((_counter_from < _counter_to) ? +1 : -1);
      delete(this);
    }
  }

  static void operator delete(void* ptr) {
    self_type *self = reinterpret_cast<self_type*>(ptr);
    if(!self->_deleted) {
      self->_deleted = true;
      ::operator delete(ptr);
      printf("operator delete -> memory free\n");
    }
  }

private:
  const int _counter_from;
  const int _counter_to;
  int _counter;
  Runnable _runnable;

  bool _deleted;
};


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
  auto sp1 = std::make_unique<DestructorRecursion<TrianglePrinter>>(0, 3, TrianglePrinter());;
  return 0;
}
