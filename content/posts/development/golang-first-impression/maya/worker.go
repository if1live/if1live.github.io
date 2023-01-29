package main

import (
	"fmt"
	"time"
)

func calledFromAsync(a int, ch chan int) {
	time.Sleep(1 * time.Second)
	fmt.Println("async call : ", a)
	ch <- a * 10
}

func main() {
	loopCount := 3
	ch := make(chan int)

	for i := 0; i < loopCount; i++ {
		go calledFromAsync(i, ch)
	}

	fmt.Println("generating async complete")

	for i := 0; i < loopCount; i++ {
		fmt.Println("retval : ", <-ch)
	}
}
