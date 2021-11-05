#define SAMPLE_SIZE 1024
volatile int input[SAMPLE_SIZE];
int output[SAMPLE_SIZE];
int out_checksum;

void __attribute__((noinline)) checksum() {
    int hash = 0;
    int x = SAMPLE_SIZE;
    // HWLoop for while-to-for converted
    #pragma clang loop unroll(disable)
    while (x > 0) {
        hash += output[x];
        x--;
    }
    out_checksum = hash;
}

void
init_data() {
    // set top borders to zero
    #pragma clang loop unroll(disable)
    for (int x = 0; x < SAMPLE_SIZE; x++) {
        output[x] = 0;
        input[x] = SAMPLE_SIZE - x;
    }
}

void __attribute__((noinline)) profile(
    unsigned char* restrict input_samples,
    unsigned char* restrict output_samples) {
    int sum = 0;
    // HWLoop with fixed array size
    #pragma clang loop unroll(disable)
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        if (input_samples[i] > 123) {
            output_samples[i] = 7;
            sum++;
        } else
            output_samples[i] = 5;
    }

    // HWLoop with iteration count in variable
    #pragma clang loop unroll(disable)
    for (int i = 0; i < sum; i++) {
        output_samples[i] = 0;
    }

    // HWLoop with iteration count (-ve) in variable
    #pragma clang loop unroll(disable)
    for (int i = sum; i > 0; i--) {
        output_samples[i] = 0;
    }
}

int
main() {
    init_data();
    // Binarization kernel
    profile((unsigned char*)input, (unsigned char*)output);
    checksum();
    return 0;
}
