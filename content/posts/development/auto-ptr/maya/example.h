// Ŭnicode please
#pragma once
#include <memory>

class SampleObj;
class Example {
public:
  Example(SampleObj *obj);
  ~Example();
  std::auto_ptr<SampleObj> ptr_obj;
};
