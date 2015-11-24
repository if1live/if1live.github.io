CXX	= clang++
CXXFLAGS	= -W -Wall -std=c++11
SIMPLE_VER_FLAGS	= $(CXXFLAGS) -DUSE_SIMPLE_VER
VARIABLE_VER_FLAGS	= $(CXXFLAGS) -DUSE_VARIABLE_VER
FUNCTION_VER_FLAGS	= $(CXXFLAGS) -DUSE_FUNCTION_VER

all:
	make simple
	make variable
	make function

simple:
	$(CXX) -o $@ main.cpp $(SIMPLE_VER_FLAGS)
	./$@

variable:
	$(CXX) -c variable_is_macro.cpp $(VARIABLE_VER_FLAGS)
	$(CXX) -c main.cpp $(VARIABLE_VER_FLAGS)
	$(CXX) -o $@ main.o variable_is_macro.o $(VARIABLE_VER_FLAGS)
	./$@

function:
	$(CXX) -c function_is_macro.cpp $(FUNCTION_VER_FLAGS)
	$(CXX) -c main.cpp $(CXXFLAGS) $(FUNCTION_VER_FLAGS)
	$(CXX) -o $@ main.o function_is_macro.o $(FUNCTION_VER_FLAGS)
	./$@

clean:
	rm -rf *.o
	rm -rf simple
	rm -rf variable
	rm -rf function
