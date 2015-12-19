#!/bin/bash

function thread_cpp {
	echo "thread :: cpp"
	clang++ -o thread thread.cpp -std=c++11 -lpthread -W -Wall
	./thread
}

function worker_cpp {
	echo "worker :: cpp"
	clang++ -o worker worker.cpp -std=c++11 -lpthread -W -Wall
	./worker
}

function worker_golang {
	echo "worker :: golang"
	go run worker.go
}

function no_generic_java {
	echo "no generic :: java"
	javac no_generic.java
	java NoGeneric
}

function no_generic_golang {
	echo "no generic :: golang"
	go run no_generic.go
}

function indent_csharp {
	echo "indent :: c#"
	mcs indent.cs
	mono ./indent.exe
}

function indent_golang {
	echo "indent :: golang"
	go run indent.go
}

function rtti_cpp {
	echo "rtti :: cpp"
	clang++ -o rtti rtti.cpp -std=c++11
	./rtti
}

function rtti_golang {
	echo "rtti :: golang"
	go run rtti.go
}

function unused_c {
	echo "unused :: c without -W -Wall"
	clang -o unused unused.c
	echo "unused :: c with W -Wall"
	clang -o unused unused.c -W -Wall
}

function unused_golang {
	echo "unused :: golang"
	go run unused.go
}

function private_python {
	echo "private :: python"
	python private.py
}

function private_ruby {
	echo "private :: ruby"
	ruby private.rb
}

function private_golang {
	echo "call by xxx :: golang"
	go run private_main.go
}

function call_by_xxx_java {
	echo "call by xxx :: java"
	javac call_by_xxx.java
	java CallByXXX
}

function call_by_xxx_csharp {
	echo "call by xxx :: c#"
	mcs call_by_xxx.cs
	mono ./call_by_xxx.exe

}

function call_by_xxx_golang {
	echo "call by xxx :: golang"
	go run call_by_xxx.go
}

function data_structure_csharp {
	echo "data structure :: c#"
	mcs data_structure.cs
	mono ./data_structure.exe
}

function data_structure_golang {
	echo "data structure :: golang"
	go run data_structure.go
}

function broken_goroutine {
	echo "broken goroutine :: golang"
	go run broken_goroutine.go
}

function clean {
	rm -rf a.out
	rm -rf worker
	rm -rf thread
	rm -rf *.class
	rm -rf indent.exe
	rm -rf rtti
	rm -rf unused
	rm -rf call_by_xxx.exe
	rm -rf data_structure.exe
}

thread_cpp

broken_goroutine

# data structure
data_structure_csharp
echo ""
data_structure_golang
echo ""

# call by xxx
call_by_xxx_java
echo ""
call_by_xxx_csharp
echo ""
call_by_xxx_golang
echo ""

# private / public
private_python
echo ""
private_golang
echo ""
private_ruby
echo ""

# unused_c
unused_c
echo ""
unused_golang
echo ""

# rtti
rtti_cpp
echo ""
rtti_golang
echo ""

# worker
worker_cpp
echo ""
worker_golang
echo ""

# no generic
no_generic_java
echo ""
no_generic_golang
echo ""

# indent
indent_csharp
echo ""
indent_golang
echo ""

clean
