#include "tceops.h"

int main() {
    char* test_str = "Testing123123\0";
    while (*test_str)
        _TCE_STREAM_OUT(*test_str++);
    return 0;
}
