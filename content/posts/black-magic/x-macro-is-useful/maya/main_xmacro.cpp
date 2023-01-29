#include <cstdio>

#undef STRUCT_BEGIN
#undef STRUCT_INT
#undef STRUCT_END

#define STRUCT_BEGIN(NAME) struct NAME {
#define STRUCT_INT(NAME, VALUE, LABEL) int NAME = VALUE;
#define STRUCT_END() };

#include "struct_data.txt"

void reset_date(Date &obj) {
    #undef STRUCT_BEGIN
    #undef STRUCT_INT
    #undef STRUCT_END

    #define STRUCT_BEGIN(NAME)
    #define STRUCT_INT(NAME, VALUE, LABEL) obj.NAME = VALUE;
    #define STRUCT_END()

    #include "struct_data.txt"
}


void print_date(Date &obj) {
    #undef STRUCT_BEGIN
    #undef STRUCT_INT
    #undef STRUCT_END

    #define STRUCT_BEGIN(NAME)
    #define STRUCT_INT(NAME, VALUE, LABEL) printf(LABEL "%d\n", obj.NAME);
    #define STRUCT_END()

    #include "struct_data.txt"
}

int main() {
    Date d = {2, 5};
    print_date(d);

    reset_date(d);
    print_date(d);
    return 0;
}
