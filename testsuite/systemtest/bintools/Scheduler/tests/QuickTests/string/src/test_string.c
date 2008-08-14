/**
 * @file stringtest.c
 * 
 * Test for some standard C string & character functions.
 * 
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
 */

#include <string.h>
#ifdef __TCE_V1__
#include "tceops.h"
#else
#include "userdef.h"
#endif

char in[] = "hello12345";
char out[] = "0123456789";
char foo[] = "Foo";

/* test strings for strpbrk */
char str1[] = "abcd";
char str2[] = "1234567890ABCDEcjee";
char str3[] = "simsalabim";

/**
 * Prints debug strings with STDOUT customop.
 */
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


/**
 * Tests some stdlib string functions.
 */
int main() {

   char* pos, pos2, pos3, pos4, catenated = NULL;
   int n,i = 0;
   
   /* strcmp */
   printstring("strcmp: ");
   if (strcmp(in, "hello12345") == 0) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   }
   if (strcmp(in, out) != 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   /* memcpy */
   printstring("memcpy: ");
   memcpy(out, in, 10);
   if (strcmp(in, out) == 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   /* memmove */
   printstring("memmove: ");
   memmove(out+1, out, 5);
   if (strcmp(out, "hhello2345") == 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   /* strlen */
   printstring("strlen: ");
   if (strlen("12345") == 5) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   }
   if (strlen(in) == 10) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   /* strchr */
   printstring("strchr: ");
   if (strchr(in, 'X') == 0) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   } 

   pos = strchr(in, 'h');
   pos2 = strchr(in, 'o');
   if (pos == in) {
      printstring("OK");
   } else {
      printstring("ERROR");
   }
   if (pos2 == (in + 4)) {
      printstring(" OK\n");
   } else {
      printstring(" ERROR\n");
      
   }
   
   /* isalpha */
   printstring("isalpha: ");
   if (isalpha(in[0])) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   } 
   if (!isalpha(in[7])){
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   /* isupper */
   printstring("isupper: ");
   if (isupper(foo[0])) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   } 
   if (!isupper(foo[1])) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   } 

   /* islower */
   printstring("islower: ");
   if (!islower(foo[0])) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   } 
   if (islower(foo[1])) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   } 

   /* tolower */
   printstring("tolower: ");
   if (islower(tolower(foo[0]))) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   } 

   /* toupper */
   printstring("toupper: ");
   if (isupper(toupper(foo[2]))) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   } 

   /* memset */
   printstring("memset: ");
   memset(out, 'x', 10);
   if (strcmp(out, "xxxxxxxxxx") == 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   
   /* atoi */
   printstring("atoi: ");
   n = atoi("12345");
   if (n == 12345) {
      printstring("OK ");
   } else {
      printstring("ERROR ");
   }
   i = atoi("-999999");
   if (i == -999999) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }


   /* strpbrk */
   printstring("strpbrk: ");
   pos3 = strpbrk(str2, str1);
   if (pos3 == (str2 + 15)) {
      printstring("OK\n");
   } else {
      printstring("ERROR\n");
   }

   
   /* strcat */
   printstring("strcat: ");
   catenated = strcat(in, foo);
   if (strcmp(catenated, "hello12345Foo") == 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR int strcat()\n");
   }

   /* strstr */
   printstring("strstr: ");
   pos4 = strstr(str3, "sala");
   if (pos4 == NULL || strcmp(pos4, "salabim") == 0) {
      printstring("OK\n");
   } else {
      printstring("ERROR in strstr()");
      printstring("\n");
   }
   
}
