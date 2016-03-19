all:
	make simple
	./a.out || true
	make general
	./a.out || true
	make safe
	./a.out || true
	make member_var
	./a.out || true

simple: simple.cpp
	clang++ $^

general: destructor_recursion.cpp
	clang++ $^

safe: safe_destructor_recursion.cpp
	clang++ $^ -std=c++14

member_var: member_var_dtor_recursion.cpp
	clang++ $^ -std=c++14
