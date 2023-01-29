package main

import "fmt"

type Simple struct {
	a int
}

func main() {
	// slice
	dummySlice := []Simple{{1}}
	dummySlice = append(dummySlice, Simple{2})
	for i, v := range dummySlice {
		fmt.Println(i, v)
	}

	// map
	dummyMap := map[string]Simple{
		"foo": {1},
	}
	dummyMap["bar"] = Simple{2}
	for k, v := range dummyMap {
		fmt.Println(k, v)
	}
}
