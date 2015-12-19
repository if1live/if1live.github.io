package main

import "fmt"

type Sample struct {
	val int
}

var dataList = []Sample{
	{1}, {2}, {3},
}

func HandleValue(s Sample, ch chan bool) {
	fmt.Println(s.val)
	ch <- false
}

func HandlePointer(s *Sample, ch chan bool) {
	fmt.Println(s.val)
	ch <- false
}

func CallByValue() {
	ch := make(chan bool)
	fmt.Println("Call by Value + go routine")
	for _, data := range dataList {
		go HandleValue(data, ch)
	}
	for i := 0; i < len(dataList); i++ {
		<-ch
	}
}

func CallByReference() {
	ch := make(chan bool)
	fmt.Println("Call by Reference + go routine")
	for _, data := range dataList {
		go HandlePointer(&data, ch)
	}
	for i := 0; i < len(dataList); i++ {
		<-ch
	}
}

func main() {
	CallByValue()
	CallByReference()
}
