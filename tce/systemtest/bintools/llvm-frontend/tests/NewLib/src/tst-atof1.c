#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char DISABLED_TEXT[] = "test disabled";

char*
tst_atof1 (void)
{
    
    return DISABLED_TEXT;

    char buf[100];
    static char buf2[100];
    snprintf (buf, sizeof (buf), "%g", atof ("0x10p-1"));

    if (strcmp (buf, "8") != 0) {
        snprintf (buf2, sizeof (buf2), "got \"%s\", expected \"8\":", buf);
        return buf2;
    }
    return NULL;
}

/*
#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
*/
