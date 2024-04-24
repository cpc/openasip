int add_num(int a, int b) {
    return a + b;
}

int main() {
    volatile int a = 1;
    volatile int b = 2;
    volatile int c = add_num(a, b);
    return c;
}