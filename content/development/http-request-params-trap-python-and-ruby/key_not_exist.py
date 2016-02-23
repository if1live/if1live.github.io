#!/usr/bin/env python

foo_list = [1, 2, 3]
try:
    print(foo_list[100])
except Exception as ex:
    print(repr(ex))

foo_dict = {"foo": 2, "bar": 4}
try:
    print(foo_dict["spam"])
except Exception as ex:
    print(repr(ex))
