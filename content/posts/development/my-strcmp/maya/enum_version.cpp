#include <cstdio>
#include <cstring>
#include <cassert>

typedef enum {
  MyOrderedAscending = -1,
  MyOrderSame,
  MyOrderedDescending
} MyComparisonResult;

MyComparisonResult my_strcmp_enum(const char * str1, const char * str2) {
  int val = strcmp(str1, str2);
  if(val < 0) {
    return MyOrderedAscending;
  } else if(val > 0) {
    return MyOrderedDescending;
  } else {
    return MyOrderSame;
  }
}

bool operator==(MyComparisonResult a, int b) {
  return ((int)a == b);
}

bool operator==(MyComparisonResult a, bool b) {
  return !((bool)a == b);
}

bool operator!=(MyComparisonResult a, int b) { return !(a == b); }
bool operator!=(MyComparisonResult a, bool b) { return !(a == b); }

// compile error
// error: conversion function must be a non-static member function
// operator bool(MyComparisonResult a) { return true; }

int main()
{
  assert(my_strcmp_enum("foo", "foo") == 0);
  assert(my_strcmp_enum("foo", "foo") == true);

  assert(my_strcmp_enum("foo", "bar") != 0);
  assert(my_strcmp_enum("foo", "bar") != true);

  // compatible with strcmp
  assert(my_strcmp_enum("1", "1") == strcmp("1", "1"));
  assert(my_strcmp_enum("1", "2") == strcmp("1", "2"));
  assert(my_strcmp_enum("2", "1") == strcmp("2", "1"));

  // if(my_strcmp...)
  printf("same : %d\n", my_strcmp_enum("foo", "foo"));
  printf("different : %d\n", !my_strcmp_enum("foo", "foo"));
  return 0;
}
