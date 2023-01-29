#!/usr/bin/env python

def check_prime_number(val):
    data = [2, 3, 5, 7, 11]
    try:
        found = data.index(val)
        print("%d is prime number, index=%d" % (val, found))
    except ValueError:
        print("%s is not prime number" % (val,))

check_prime_number(5)
check_prime_number(2)
check_prime_number(-1)
