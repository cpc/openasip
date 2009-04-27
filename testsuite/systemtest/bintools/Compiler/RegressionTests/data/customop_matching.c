volatile int in1=1,in2=2;
volatile int out1, out2, out3;

int main() {
    // check andn
    int a = in1 + 2;
    int b = in2 / 4;
    int c = in2 / 6;
    out1 = (a&(~b))*c;

    // check addsub
    int e = a+b;
    int f = a-b;
    out2 = e;
    out3 = f;
}
