package main

import "fmt"

type Sample struct {
	a int
	b uint8
}

func main() {
	s1 := Sample{1, 'a'}
	fmt.Printf("%%T  : %T\n", s1)
	fmt.Printf("%%t  : %t\n", s1)
	fmt.Printf("%%v  : %v\n", s1)
	fmt.Printf("%%#v : %#v\n", s1)

	s2 := &s1
	fmt.Printf("%%T  : %T\n", s2)
	fmt.Printf("%%t  : %t\n", s2)
	fmt.Printf("%%v  : %v\n", s2)
	fmt.Printf("%%#v : %#v\n", s2)
}
