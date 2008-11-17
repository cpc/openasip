/**
 * @file test_libc.c
 * 
 * Example program that tests various functions of libc.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 */

#include <stdio.h>

/**
 * Prints string to debug, if string is NULL, "Ok" is printed.
 *
 * @param string String to print.
 */

void printstring(char* string) {
    if (string == NULL) {
        string = "Ok";
    }
    printf(string);
}

/* Test macro */
#define RUN_TEST(x) \
                    printstring("Running test " #x ": ");\
                    printstring((char*)tst_##x());\
                    printstring("\n");

#define TEST_CASES 10
void test_int64_types(void);

/**
 * Run test functions and print output.
 */
int main(int argc,char **argv,char **envp) {
    RUN_TEST(malloc);
    RUN_TEST(atof1);
    RUN_TEST(atof2);
    RUN_TEST(random);
    RUN_TEST(math);
    return 0;
}
