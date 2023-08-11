#include "tceops.h"

char data = 0;
// Non-volatile pointer points to volatile data.
volatile char *addr;
int
main() {
    _TCEAS_STQ("#1", addr, data);
    _TCEAS_STQ("#1", addr, data + 1);
    // TODO: Assigning operations manually to FUs does not
    // take into consideration of AS alias currently
    // i.e. Following code will not produce any alising edge.
    // _TCEFU_STQ("LSU_data1", addr, data);
    // _TCEFU_STQ("LSU2_data1", addr, data+1);
    _TCEAS_STQ("#2", addr, data + 2);
    return 0;
}
