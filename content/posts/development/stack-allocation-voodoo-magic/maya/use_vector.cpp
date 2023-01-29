#include <vector>
#include <stdexcept>

void vector_ver()
{
	int i = 1234;
	std::vector<int> data(2);

	for(i = 0 ; i <= 2 ; ++i) {
		int &val = data.at(i);
		val = 0;
	}
}

int main()
{
	try {
		vector_ver();
	} catch(std::out_of_range e) {
		printf("range exception\n");
	}
	return 0;
}
