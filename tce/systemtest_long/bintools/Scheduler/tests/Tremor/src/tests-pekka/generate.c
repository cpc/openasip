/**
 * A helper program that creates a initialized C char array from stdin.
 *
 * Output the C code to stdout. First argument tells the output buffer
 * size.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */
#include <stdio.h>

#define BYTES_PER_ROW 16

int main(int argc, char** argv) {
    char byte;
    size_t count = 0;
    int first = 1;

    if (argc < 2) {
        printf("Give the output buffer size as argument.\n");
        return 1;
    }
    
    printf("\nchar initial_data[] =\n{");
    while (fread(&byte, 1, 1, stdin)) {
        if (first) {
            printf("0x%02hhx", byte);
            first = 0;
        } else {
            printf(", 0x%02hhx", byte);
        }
                
        ++count;
        if (count % BYTES_PER_ROW == 0) {
            printf(",\n ");
            first = 1;
        }
    }
    printf("};\n");
    printf("int initial_data_size = %d;\n", count);
    printf("int output_buffer_size = %s;\n", argv[1]);
    printf("char output_data[%s];\n", argv[1]);
    return 0;
}
