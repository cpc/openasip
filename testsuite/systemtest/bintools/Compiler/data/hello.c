/**
 * @file hello.c
 *
 * Example program that uses a special user defined operation called
 * 'stdout' to print out debugging data in simulator. Expects that the
 * 'stdout' operation simulation behavior definition treats the input 
 * as a char which is written to standard output.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#ifdef __TCE_V1__
#include "tceops.h"
#else
#include "userdef.h"
#endif

void printstring(char* string) {
    while (*string) {
#ifdef __TCE_V1__
       _TCE_STDOUT((int)(*string));
#else
       WRITETO(stdout.1, *string);
#endif
       string++;
    }
}

int main(int argc,char **argv,char **envp) {

   int i = 0;
   
   printstring("Hello TTA world!\n");
 
   for (; i < 5; ++i) {
      printstring("Yeah!");
    }
    printstring("Bye!\n");
}
