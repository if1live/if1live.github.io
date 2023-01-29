package main

import "fmt"

type Program struct{}

func (prog *Program) Run() {
	for i := 0; i < 1; i++ {
		fmt.Println("indent - golang")
	}
}

func main() {
	prog := &Program{}
	prog.Run()
}
