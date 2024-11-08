/*
    Test a few instruction patterns that the compiler should recognize.
    Also test the intrinsic support.
*/

int test_func(int a, int b) {
    return a + b + 1;
}

int test_func2(int a, int b) {
    return (( a >> 6 ) ^ 1020) + b;
}

int test_func3(int a) {
    return (a << 1) + 123;
}

int test_func4(int a) {
    int retval = 0;
    _OA_RV_TEST_OP_4(a, retval);
    return retval;
}