#include <stdio.h>

int main()
{
	int x;
	int y;
	int diff = (unsigned long)&x - (unsigned long)&y;
	printf("addr x : %lx\n", (unsigned long)&x);
	printf("addr y : %lx\n", (unsigned long)&y);
	printf("addr diff: %d\n", diff);
	return 0;
}
