#!/usr/bin/env python

def foo():
    print("foo called")

def bar(a = foo()):
    print("bar called")

print("complete")
