/**
 * Simple test for stdlib qsort.
 * 
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
 */

#include <string.h>
#include <stdlib.h>


char s[] = "qwertyuiopasdfghjklzxcvbnm";
char corr[] = "abcdefghijklmnopqrstuvwxyz";

#ifdef __TCE__
#ifdef __TCE_V1__
# include "tceops.h"
#else
#include "userdef.h"
#endif

void printstring(char* string) {
    int printcount = 0;
    while (*string) {
       #ifdef __TCE_V1__
       int c = *string;
       _TCE_STDOUT(c);
       #else
       WRITETO(stdout.1, *string);
       #endif
       string++;
    }

}

#else
#include <stdio.h>
void printstring(char* string) {
   printf(string);
}
#endif

int compare(const void* a, const void* b){
   char first = *((char*)a);
   char second = *((char*)b);
   
   if (first > second) return 1;
   if (first < second) return -1;
   return 0;
}

int main() {
   if (strcmp(s, corr) == 0) {
      printstring("Error in strcmp()\n");
      return 1;
   }
   qsort(s, strlen(s), sizeof(char), compare);
   if (strcmp(s, corr) != 0) {
      printstring("Error in qsort(): ");
      printstring(s);
      printstring("\n");
      return 1;
   }

   printstring("OK: ");
   printstring(s);
   printstring("\n");
   return 0;
}
