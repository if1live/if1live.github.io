#include <cassert>
#include <array>

template<typename T, std::size_t N>
struct my_array : std::array<T, N> {
	T &operator[](std::size_t n) {
		assert(n >= 0 && n < N);
		return (*static_cast<std::array<T,N>*>(this))[n];
	}
	const T&operator[](std::size_t n) const {
		assert(n >= 0 && n < N);
		return (*static_cast<const std::array<T,N>*>(this))[n];
	}
};

int main()
{
	int i = 1234;
	my_array<int, 2> data;

	for(i = 0 ; i <= 2 ; ++i) {
		data[i] = 0;
	}

	return 0;
}
