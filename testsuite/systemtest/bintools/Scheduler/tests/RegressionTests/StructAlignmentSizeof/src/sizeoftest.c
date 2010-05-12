#include <stdio.h>

typedef struct
{
    char a;
    char b;
    char c;
}
inner;

typedef struct
{
    char d;
	inner e;
}
outer;

outer foo[2][2];

int main(void) {
    if (((unsigned int)(&foo)) + sizeof(outer) == ((unsigned int)(&foo[0][1]))) {
        puts("OK\n"); 
    } else {
        puts("FAIL\n");
    }
}
