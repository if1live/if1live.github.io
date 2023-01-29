#!/usr/bin/env python

class Sample:
    def public_method(self):
        print("this is public")

    def __private_method(self):
        print("this is private")

    def call_private(self):
        self.__private_method()

if __name__ == "__main__":
    s = Sample()
    s.public_method()
    s.call_private()
    try:
        s.__private_method()
    except:
        print("cannot access private in this scope")
