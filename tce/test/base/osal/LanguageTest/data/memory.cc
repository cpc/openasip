/**
 * @file memory.cc
 *
 * Operations for test purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include "OSAL.hh"

OPERATION(LOAD)

TRIGGER
    INITIATE_READ(INT(1), 4);
    RETURN_NOT_READY;
END_TRIGGER;

LATE_RESULT
    if (MEM_RESULT_READY()) {
        UIntWord data;
        MEM_DATA(data);
        IO(2) = data;
        RETURN_UPDATED;
    } else {
        RETURN_NOT_UPDATED;
    }
END_LATE_RESULT;

END_OPERATION(LOAD);

OPERATION(STORE)

TRIGGER
    INITIATE_WRITE(INT(1), 4, UINT(2));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(STORE);
