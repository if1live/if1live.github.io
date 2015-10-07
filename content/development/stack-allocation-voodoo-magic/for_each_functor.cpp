#include <array>
#include <algorithm> // for std::for_each

struct Functor {
	void operator()(int &x) {
		x = 0;
	}
};

int main()
{
	int i = 1234;
	std::array<int, 2> data;

	std::for_each(data.begin(), data.end(), Functor());
	return 0;
}
