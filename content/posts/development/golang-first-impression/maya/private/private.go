package private

import "fmt"

type Sample struct{}

func (s *Sample) PublicMethod() {
	fmt.Println("this is public")
}

func (s *Sample) privateMethod() {
	fmt.Println("this is private")
}

func (s *Sample) CallPrivate() {
	s.privateMethod()
}
