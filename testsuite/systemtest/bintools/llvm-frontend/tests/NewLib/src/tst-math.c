#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

volatile float result1, result2, result3, result4, result5;
volatile float calc1, calc2, calc3, calc4, calc5;

static int max_mantissa = 0xffffff;
static int max_i = 32;
static char buf[1024];

// allows values to differ a bit 
// (one bit of mantissa of smaller parameter)
int near_enough(const char* func, float a, float b) {

#ifndef __TCE__
    return 1;
#endif
    
    // byteorder and float size dependent stuff..
    
    float allowed_difference = 0;
    float dif = 0;
    long abs_a_temp = (*(long*)&a)&0x7fffffff;
    long abs_b_temp = (*(long*)&b)&0x7fffffff;
    float abs_a = *(float*)&abs_a_temp;
    float abs_b = *(float*)&abs_b_temp;                   

    if (abs_a > abs_b) {
        long bLong = (*(long*)&abs_b)&0xff000000;
        dif = abs_a - abs_b;
        allowed_difference = *(float*)&bLong;
    } else {
        long aLong = (*(long*)&abs_a)&0xff000000;
        dif = abs_b - abs_a;
        allowed_difference = *(float*)&aLong;
    }

        
/*
     printf ("With %s....\n" 
             "a: %f(%8x) b: %f(%8x)\n" 
             "dif: %f(%8x), allowed: %f(%8x)\n",   
             func, 
             a, *(long*)&a, b, *(long*)&b, 
             dif, *(long*)&dif,  
             allowed_difference,  
             *(long*)&allowed_difference); 
*/

    if (dif > allowed_difference) {
        
        snprintf (buf, sizeof(buf), 
                  "With %s....\n"
                  "a: %f(%8x) b: %f(%8x)\n"
                  "dif: %f(%8x), allowed: %f(%8x)\n",  
                  func,
                  a, *(long*)&a, b, *(long*)&b,
                  dif, *(long*)&dif, 
                  allowed_difference, 
                  *(long*)&allowed_difference);

        return 0;
    }

    return 1;
}

char*
tst_math (void)
{
    int i = 0;
    
    for (i = 0; i < max_i; i++) {
        result3 = (float)i;
        result1 = result3 / max_mantissa;
        result2 = result3 / (max_mantissa>>1);
        result4 = result3 * (max_mantissa/max_i);
        result5 = result3 * (float)((unsigned)0xffffffff);
        
        calc1 = sqrt(result1*result1);
        calc2 = sqrt(result2*result2);
        calc3 = sqrt(result3*result3);
        calc4 = sqrt(result4*result4);
        calc5 = sqrt(result5*result5);
        
        if (!near_enough("sqrt", result1,calc1) || 
            !near_enough("sqrt", result2,calc2) || 
            !near_enough("sqrt", result3,calc3) || 
            !near_enough("sqrt", result4,calc4) || 
            !near_enough("sqrt", result5,calc5)) {
            
            return buf;
        }            

        calc1 = sin(asin(sin(result1)));
        calc2 = sin(asin(sin(result2)));
        calc3 = sin(asin(sin(result3)));
        calc4 = sin(asin(sin(result4)));
        
        if (!near_enough("sin&asin", sin(result1),calc1) || 
            !near_enough("sin&asin", sin(result2),calc2) ||
            !near_enough("sin&asin", sin(result3),calc3) ||
            !near_enough("sin&asin", sin(result4),calc4)) {            
            return buf;
        }            

    }

    return NULL;
}
