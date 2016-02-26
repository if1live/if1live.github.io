#!/usr/bin/env python
from importd import d

@d("/")
def index(request):
    return d.HttpResponse("Hello World")

if __name__ == "__main__":
    d.main()
