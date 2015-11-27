CC	= clang
CCFLAGS	= -W -Wall -O3
SIMPLE_VER_FLAGS	= $(CCFLAGS) -DUSE_SIMPLE_VER
VARIABLE_VER_FLAGS	= $(CCFLAGS) -DUSE_VARIABLE_VER
FUNCTION_VER_FLAGS	= $(CCFLAGS) -DUSE_FUNCTION_VER

all:
	make simple
	make variable
	make function
	make simple_asm
	make variable_asm
	make function_asm


simple_asm:
	$(CC) -o $@.s -S main.c $(SIMPLE_VER_FLAGS)

simple:
	$(CC) -o $@ main.c $(SIMPLE_VER_FLAGS)
	./$@

variable_asm:
	$(CC) -o $@.s -S main.c $(VARIABLE_VER_FLAGS)

variable:
	$(CC) -c variable_is_macro.c $(VARIABLE_VER_FLAGS)
	$(CC) -c main.c $(VARIABLE_VER_FLAGS)
	$(CC) -o $@ main.o variable_is_macro.o $(VARIABLE_VER_FLAGS)
	./$@

function_asm:
	$(CC) -o $@.s -S main.c $(FUNCTION_VER_FLAGS)

function:
	$(CC) -c function_is_macro.c $(FUNCTION_VER_FLAGS)
	$(CC) -c main.c $(CCFLAGS) $(FUNCTION_VER_FLAGS)
	$(CC) -o $@ main.o function_is_macro.o $(FUNCTION_VER_FLAGS)
	./$@

clean:
	rm -rf *.o
	rm -rf simple
	rm -rf variable
	rm -rf function
	rm -rf *.s
	rm -rf tmp.c
