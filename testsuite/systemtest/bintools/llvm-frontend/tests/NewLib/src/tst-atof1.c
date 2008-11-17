#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
tst_atof1 (void)
{
    
    return "Test disabled";

    static char buf2[100];
    float retVal = atof ("0x10p-1");

    if (retVal != 8) {
        snprintf (buf2, sizeof (buf2), "got \"%f\", expected \"8\":", retVal);
        return buf2;
    }
    return NULL;
}

