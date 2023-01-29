#include <stdio.h>

char calc_grade(int score) {
	if(score >= 90) { return 'A'; }
	if(score >= 70) { return 'B'; }
	if(score >= 40) { return 'C'; }
	if(score >= 0)  { return 'D'; }
	return '?';
}

int main(void) {
	int values[] = {100, 90, 89, 70, 69, 40, 39, 0};
	for(int i = 0 ; i < sizeof(values) / sizeof(values[0]) ; i++) {
		int v = values[i];
		printf("%d = %c\n", v, calc_grade(v));
	}
	return 0;
}
