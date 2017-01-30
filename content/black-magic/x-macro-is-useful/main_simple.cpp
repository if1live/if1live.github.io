#include <cstdio>

struct DateSimple {
    int month = 1;
    int day = 1;
};

void reset_date(DateSimple &obj) {
    obj.month = 1;
    obj.day = 1;
}

void print_date(DateSimple &obj) {
    printf("M: %d\n", obj.month);
    printf("D: %d\n", obj.day);
}

int main() {
    DateSimple d = {2, 5};
    print_date(d);

    reset_date(d);
    print_date(d);
    return 0;
}
