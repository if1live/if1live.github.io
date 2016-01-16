broken_dtor: broken_dtor.cpp
	clang++ -o $@ $^ -g -W -Wall -std=c++11

run: broken_dtor
	./broken_dtor normal.txt
	./broken_dtor buffer_overruns.txt

clean:
	rm -rf a.out
	rm -rf *.o
	rm -rf broken_dtor
