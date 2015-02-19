#include <stdio.h>

volatile unsigned int x = 110;
unsigned int v;
void *s;
int main() {
    puts("Starting.");
    s = &&s0;
    while (1) {
	v = x;
	goto *s;
	s0:
	    if (v < 100) {
            int u = v;
            puts("Not here.");
            if (u > 10) {
                puts("Nor here.");
                s = &&s1;
            }
	    }
	    continue;
	s1:
        puts("Nor here neither.");
        continue;
    }
}

