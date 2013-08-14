//#include "tceops.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <lwpr.h>

volatile uint arr_a[100];
volatile uint arr_b[100]; 
volatile uint arr_c[100];
volatile uint arr_d[100];
volatile uint arr_e[100];
volatile uint arr_f[100];
volatile uint a;
volatile uint b;
volatile uint c;
volatile uint d;
volatile uint e;
volatile uint f;

static const size_t length = 10;

uint n_max(volatile uint *a, size_t size)
{
	uint max = 0;
	for (size_t i=0; i<size; i++)
	{
		if(a[i] > max)
			max = a[i];
	}
	return max;
}

void init()
{
	for(int i = 0; i < 100; i++)
	{
		arr_a[i] = i;
		arr_b[i] = i;
		arr_c[i] = i;
		arr_d[i] = i;
		arr_e[i] = i;
		arr_f[i] = i;
	}	
}

int main() {
	// Init arrays
	init();	
	
	a = n_max(arr_a, 100);
	b = n_max(arr_b, 100);
	c = n_max(arr_c, 100);
	d = n_max(arr_d, 100);
	e = n_max(arr_e, 100);
	f = n_max(arr_f, 100);

    lwpr_print_int(a);
    lwpr_newline();

    lwpr_print_int(b);
    lwpr_newline();

    lwpr_print_int(c);
    lwpr_newline();

    lwpr_print_int(d);
    lwpr_newline();

    lwpr_print_int(e);
    lwpr_newline();

    lwpr_print_int(f);
    lwpr_newline();

    return 0;
}
