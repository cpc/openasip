/**
 * Tests issues with constants and reduced connectivity machines.
 *
 * Compile the .bc with -O0.
 */

volatile int b = 0x00AAAAAA;
volatile int res = 0;

int main() {
    res = b & 0x0000FFFF;
    return res;
}
