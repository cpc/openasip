#define SAMPLE_SIZE 1024
#define UNUSED 0
#define _weight_mem __attribute__((address_space(1)))
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

    // Test imlpicit ld_st by touching the data.
    // i.e. We load and store to same location.
    _TCEAS_IST_INIT("#0", &output[0], sizeof(int)); // Configure ST$
    _TCEAS_ILD_INIT("#0", &output[1], sizeof(int)); // Configure LD
    int data = output[0];
    #pragma clang loop unroll(disable)
    for (int x = 0; x < SAMPLE_SIZE; x++) {
        _TCEAS_ILD32_IST32("#0", data, data);
    }
}
#if !defined(ILSU)
void __attribute__((noinline))
profile(int* restrict input_samples, int* restrict output_samples) {
    _weight_mem static const int coeffs[8] = {-2380, -5017, 5615,  14563,
                                              14563, 5615,  -5017, -2380};

    #pragma clang loop unroll(disable)
    for (int i = 7; i < SAMPLE_SIZE; i++) {
        int sum = 0;
        #pragma clang loop unroll(disable)
        for (int c = 0; c < 8; c++) {
            sum += input_samples[i - c] * coeffs[c];
        }
        output_samples[i] = sum;
    }
}
#else
void __attribute__((noinline))
profile(int* restrict input_samples, int* restrict output_samples) {
    _weight_mem static const int coeffs[8] = {-2380, -5017, 5615,  14563,
                                              14563, 5615,  -5017, -2380};

    // Configure auto-increment LD/ST engine (IMPLICIT LSU)
    _TCEAS_IST_INIT("#0", &output_samples[7], sizeof(int)); // Configure ST

    #pragma clang loop unroll(disable)
    for (int i = 7; i < SAMPLE_SIZE; i++) {
        int sum = 0;
        int weight, data;

        // Configure implicit loads
        _TCEAS_ILD_INIT("#0", &input_samples[i], -1 * sizeof(int));
        _TCEAS_ILD_INIT("#1", &coeffs[0], sizeof(int));

        #pragma clang loop unroll(disable)
        for (int c = 0; c < 8; c++) {
            _TCEAS_ILD32("#1", UNUSED, weight);
            _TCEAS_ILD32("#0", UNUSED, data);
            sum += weight * data;
        }
        _TCEAS_IST32("#0", sum);
    }
}
#endif

int
main() {
    init_data();
    // FIR kernel
    profile((int*)input, output);
    checksum();
    return 0;
}
