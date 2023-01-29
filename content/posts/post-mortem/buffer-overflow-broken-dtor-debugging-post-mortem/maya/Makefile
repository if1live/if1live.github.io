main: main.cpp
	clang++ $^ -g -W -Wall -std=c++11

run: main
	./a.out normal.txt
	./a.out long.txt

clean:
	rm -rf a.out
	rm -rf *.o
	rm -rf *.dSYM
