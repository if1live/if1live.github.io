package main

import "fmt"

type Sample struct {
	a int
}

func callByValue(s Sample) {
	s.a += 1
	fmt.Println("in function : ", s.a)
}

func callByReference(s *Sample) {
	s.a += 1
	fmt.Println("in function : ", s.a)
}

func main() {
	s := Sample{0}

	fmt.Println("before call by value : ", s.a)
	callByValue(s)
	fmt.Println("after call by value : ", s.a)

	fmt.Println("before call by reference : ", s.a)
	callByReference(&s)
	fmt.Println("after call by reference : ", s.a)
}
