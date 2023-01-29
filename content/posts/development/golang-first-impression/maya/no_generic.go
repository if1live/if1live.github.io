package main

import (
	"container/list"
	"fmt"
)

func useArray() {
	arr := []int{
		1,
	}
	for _, x := range arr {
		fmt.Println(x * 10)
	}
}

func useList() {
	arr := list.New()
	arr.PushBack(1)
	arr.PushBack("string")

	for e := arr.Front(); e != nil; e = e.Next() {
		if val, ok := e.Value.(int); ok {
			fmt.Println(val * 10)
		}
	}
}
func main() {
	useArray()
	useList()
}
