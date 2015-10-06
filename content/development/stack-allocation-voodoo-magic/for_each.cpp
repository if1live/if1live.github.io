#include <array>
#include <algorithm> // for std::for_each

int main()
{
	int i = 1234;
	std::array<int, 2> data;

	std::for_each(data.begin(), data.end(), [](int &x) {
			x = 0;
		});

	return 0;
}
