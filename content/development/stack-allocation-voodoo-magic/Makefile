CXX_FLAGS	= -W -Wall -Wno-unused-variable -std=c++11
C_FLAGS	= -W -Wall -Wno-unused-variable
EXECUTE_CMD	= ./a.out

all:
	@make simple
	@make inf_loop_bug
	@make for_each_functor
	@make for_each_lambda
	@make enhanced_for_loop
	@make use_vector
	@make custom_array

simple:
	gcc simple.c $(C_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang simple.c $(C_FLAGS);	\
	$(EXECUTE_CMD)

inf_loop_bug:
	gcc inf_loop_bug.c $(C_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang inf_loop_bug.c $(C_FLAGS);	\
	$(EXECUTE_CMD)

for_each_functor:
	g++ for_each_functor.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang++ for_each_functor.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD)

for_each_lambda:
	g++ for_each_lambda.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang++ for_each_lambda.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD)

enhanced_for_loop:
	g++ enhanced_for_loop.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang++ enhanced_for_loop.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD)

use_vector:
	g++ use_vector.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang++ use_vector.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD)

custom_array:
	g++ custom_array.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD);	\
	clang++ custom_array.cpp $(CXX_FLAGS);	\
	$(EXECUTE_CMD)

clean:
	rm -rf a.out
