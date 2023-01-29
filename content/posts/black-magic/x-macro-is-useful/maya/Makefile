all:
    make simple
    ./simple
    make xmacro
    ./xmacro

simple: main_simple.cpp
    clang++ -o $@ $^

xmacro: main_xmacro.cpp
    clang++ -o $@ $^
