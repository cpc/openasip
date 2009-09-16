/**
 * Simple testcase to reproduce a bug in compiled sim with
 * custom memory operations.
 *
 */
#include "tceops.h"

int main() {
    char array[10];
    _TCE_WRITE_TEST(array);
    _TCE_READ_TEST(array);
    return 0;
}
