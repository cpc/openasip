#include <stdint.h>

#ifdef __TCE__
#include <tceops.h>
#define DEBUG_MARKER(x) _TCE_STDOUT(x)
#else
#include <stdio.h>
#define DEBUG_MARKER(x) putchar(x)
#endif

typedef struct {
    int8_t a8[5];
    int16_t a16[5];
    int16_t b16[5];
} Huffman_table;

static Huffman_table HTDC[4];

/**
 * This must be function.
 */
void init_table(Huffman_table *HT) {
    int i;
    for (i=0;i<5;i++) {
        HT->a8[i] = i;
        HT->a16[i] = 5 - i;
    }
}

#define PRINT_TABLE(x) { int i;\
for (i = 0; i < sizeof(x)/sizeof(x[0]);i++) DEBUG_MARKER((char)x[i] + '0');\
DEBUG_MARKER(' '); }

#define PRINT_STRUCT(x) {\
    PRINT_TABLE((x).a8);\
    PRINT_TABLE((x).a16);\
}

void print_struct(Huffman_table *HT) {
    PRINT_STRUCT((*HT));
}

#define CHECK_TABLE(x) \
    {\
        DEBUG_MARKER(x+'0');\
        DEBUG_MARKER(' ');\
        int i;\
        for (i = 0; i < 5;i++) {\
            if (HTDC[x].a8[i] != i ||\
                HTDC[x].a16[i] != 5-i) {\
                return x+10;\
            }\
        }\
        if (HTDC[x].a16[3] != 2) {\
            return x+20;\
        }\
    }

int main() {
    init_table(&HTDC[0]);
    init_table(&HTDC[1]);
    init_table(&HTDC[2]);
    init_table(&HTDC[3]);
  
    // this prints out correctly
    DEBUG_MARKER(65);
    print_struct(&HTDC[1]);
    // this not
    DEBUG_MARKER(66);
    PRINT_STRUCT(HTDC[1]);

    DEBUG_MARKER(67);
    CHECK_TABLE(0);
    CHECK_TABLE(1);
    CHECK_TABLE(2);
    CHECK_TABLE(3);

    DEBUG_MARKER('\n');
}
