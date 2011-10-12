#include <stdio.h>

#ifdef __TCE__
#define printf iprintf
#endif

class A {
public:
    A() {
        count_ = A::obj_counter++;
        printf("A: %d\n", count_);
    }

    ~A() {
        printf("~A: %d\n", count_);
    }
    static int obj_counter;
private:
    int count_;
};
 
class B : public A {
public:
    B() : A() {
        count_ = A::obj_counter++;
        printf("B: %d\n", count_);
    }

    ~B() {
        printf("~B: %d\n", count_);
    }
private:
    int count_;
};

static A objA1;
static A objA2;
static B objB1;
static B objB2;
int A::obj_counter = 1;

int main() {
    return 0;
}
