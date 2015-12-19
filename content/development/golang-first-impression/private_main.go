package main

import "./private"

func main() {
	s := private.Sample{}
	s.PublicMethod()
	// compile error
	// s.privateMethod()
	s.CallPrivate()
}
