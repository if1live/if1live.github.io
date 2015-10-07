#include <stdio.h>

int g_loop_count = 0;

const int MAX_LOOP_A = 2;
void run_loop_a()
{
	int i = 1234;
	int data[MAX_LOOP_A];

	g_loop_count = 0;
	printf("addr i    : %lx\n", (unsigned long)&i);
	printf("addr data: %lx\n", (unsigned long)data);

	for(i = 0 ; i <= MAX_LOOP_A ; ++i) {
		data[i] = 0;
		printf("curr addr : %lx\n", (unsigned long)&data[i]);

		g_loop_count++;
		if(g_loop_count > MAX_LOOP_A + 10) {
			printf("ERROR: Infinitely Loop!!!\n");
			break;
		}
	}
}

const int MAX_LOOP_B = 4;
void run_loop_b()
{
	int i = 1234;
	int data[MAX_LOOP_B];

	g_loop_count = 0;
	printf("addr i    : %lx\n", (unsigned long)&i);
	printf("addr data: %lx\n", (unsigned long)data);

	for(i = 0 ; i <= MAX_LOOP_B ; ++i) {
		data[i] = 0;
		printf("curr addr : %lx\n", (unsigned long)&data[i]);

		g_loop_count++;
		if(g_loop_count > MAX_LOOP_B + 10) {
			printf("ERROR: Infinitely Loop!!!\n");
			break;
		}
	}
}

int main()
{
	run_loop_a();
	run_loop_b();
	return 0;
}
