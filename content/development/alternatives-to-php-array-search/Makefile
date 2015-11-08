all:
	make php_buggy
	make php_correct
	make alternative_double_check
	make alternative_exception
	make alternative_invalid_index
	make alternative_zero_is_not_false
	make alternative_iterator

php_buggy: buggy.php
	php $^

php_correct: correct.php
	php $^

alternative_double_check: alternative_double_check.php
	php $^

alternative_exception: alternative_exception.py
	python $^

alternative_invalid_index: alternative_invalid_index.js
	node $^

alternative_zero_is_not_false: alternative_zero_is_not_false.rb
	ruby $^

alternative_iterator: alternative_iterator.cpp
	clang++ $^ -W -Wall -std=c++11
	./a.out

clean:
	rm -rf a.out
