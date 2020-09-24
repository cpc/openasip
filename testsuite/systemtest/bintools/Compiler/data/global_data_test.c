#include <lwpr.h>

int global_array[120];

int main() {
  lwpr_print_int((unsigned int)(&global_array[0]));
  return 0;
}
