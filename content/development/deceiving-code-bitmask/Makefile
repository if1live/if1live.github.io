all:
	make buggy
	make alternative_warning
	make alternative_operator_precedence
	make alternative_strongly_typed

buggy: buggy.c
	gcc $^
	./a.out

alternative_warning: buggy.c
	gcc $^ -W -Wall
	./a.out

alternative_operator_precedence: alternative_operator_precedence.py
	python alternative_operator_precedence.py

alternative_strongly_typed: alternative_strongly_typed.cs
	gmcs $^
	./$@.exe

clean:
	rm -rf *.o
	rm -rf a.out
	rm -rf *.exe
