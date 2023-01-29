#include <array>

int main()
{
	int i = 1234;
	std::array<int, 2> data;

	for(int &x : data) {
		x = 0;
	}

	return 0;
}
