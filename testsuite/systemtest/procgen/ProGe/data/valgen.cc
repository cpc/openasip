/**
 * OSAL behavior definition file.
 */

#include "OSAL.hh"


OPERATION(ONES)

TRIGGER

for (int i = 0; i < 8; i++) {
    IO(2).rawData_[SIMVALUE_MAX_BYTE_SIZE-i-1] = 0xff;
}

END_TRIGGER;


END_OPERATION(ONES)
