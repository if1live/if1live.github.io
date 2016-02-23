#!/usr/bin/env python
print("str -> int")
print(int("123"))

print("none -> int")
try:
    print(int(None))
except Exception as e:
    print(repr(e))
