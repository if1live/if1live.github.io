#include <stdio.h>

int main()
{
	const int ARRAY_SIZE = 2;

	int i = 1234;
	int array[ARRAY_SIZE];

	printf("addr i    : %lx\n", (unsigned long)&i);
	printf("addr array: %lx\n", (unsigned long)array);

	int loop_count = 0;
	for(i = 0 ; i <= ARRAY_SIZE ; ++i) {
		array[i] = 0;
		printf("curr addr : %lx\n", (unsigned long)&array[i]);

		loop_count++;
		if(loop_count > ARRAY_SIZE + 2) {
			printf("ERROR: Infinitely Loop!!!\n");
			break;
		}
	}
	return 0;
}
