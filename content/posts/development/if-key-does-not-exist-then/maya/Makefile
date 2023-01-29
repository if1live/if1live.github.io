all: key_not_exist_bracket key_not_exist_find key_not_exist_count
	./key_not_exist_bracket
	./key_not_exist_find
	./key_not_exist_count

key_not_exist_bracket: key_not_exist_bracket.cpp
	clang++ $^ -o $@ -std=c++11

key_not_exist_find: key_not_exist_find.cpp
	clang++ $^ -o $@ -std=c++11

key_not_exist_count: key_not_exist_count.cpp
	clang++ $^ -o $@ -std=c++11

clean:
	@rm -rf key_not_exist_bracket
	@rm -rf key_not_exist_find
	@rm -rf key_not_exist_count
