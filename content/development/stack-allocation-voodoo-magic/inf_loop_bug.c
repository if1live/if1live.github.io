#include <stdio.h>

static int g_loop_count = 0;

#define MAX_LOOP_A 2
static void run_loop_a()
{
	int i = 1234;
	int data[MAX_LOOP_A];

	g_loop_count = 0;
	printf("addr i\t\t: %lx\n", (unsigned long)&i);
	printf("addr data\t: %lx\n", (unsigned long)data);

	for(i = 0 ; i <= MAX_LOOP_A ; ++i) {
		printf("curr addr[%d]\t: %lx\n", i, (unsigned long)&data[i]);
		data[i] = 0;

		g_loop_count++;
		if(g_loop_count > MAX_LOOP_A + 10) {
			printf("ERROR: Infinitely Loop!!!\n");
			break;
		}
	}
}

#define MAX_LOOP_B 4
static void run_loop_b()
{
	int i = 1234;
	int data[MAX_LOOP_B];

	g_loop_count = 0;
	printf("addr i\t\t: %lx\n", (unsigned long)&i);
	printf("addr data\t: %lx\n", (unsigned long)data);

	for(i = 0 ; i <= MAX_LOOP_B ; ++i) {
		printf("curr addr[%d]\t: %lx\n", i, (unsigned long)&data[i]);
		data[i] = 0;

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
