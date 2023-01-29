#include <stdio.h>

char calc_grade(int score) {
	int offset = (int)((score * score / 1600.0) + 1) / 2;
	return 'D' - offset;
}

int main(void) {
	int values[] = {100, 90, 89, 70, 69, 40, 39, 0};
	for(int i = 0 ; i < sizeof(values) / sizeof(values[0]) ; i++) {
		int v = values[i];
		printf("%d = %c\n", v, calc_grade(v));
	}
	return 0;
}
