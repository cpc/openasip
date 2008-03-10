/**
 * @file
 *
 * Behavior definition file.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include "OSAL.hh"

OPERATION(ST)

TRIGGER
    INITIATE_WRITE(UINT(1) & ~3, 4, UINT(2));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(ST)

OPERATION(LD)

TRIGGER
    INITIATE_READ(UINT(1) & ~3, 4);
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

END_OPERATION(LD)

OPERATION(LDB)

TRIGGER
    INITIATE_READ(UINT(1), 1);
    RETURN_NOT_READY;
END_TRIGGER;

LATE_RESULT
    if (MEM_RESULT_READY()) {
        UIntWord data;
        MEM_DATA(data);
        IO(2) = (unsigned char)data;
        RETURN_UPDATED;
    } else {
        RETURN_NOT_UPDATED; 
    }
END_LATE_RESULT;

END_OPERATION(LDB)

OPERATION(STB)

TRIGGER
    INITIATE_WRITE(UINT(1), 1, UINT(2));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(STB)

OPERATION(LDBU)

TRIGGER
    INITIATE_READ(UINT(1), 1);
    RETURN_NOT_READY;
END_TRIGGER;

LATE_RESULT
    if (MEM_RESULT_READY()) {
        UIntWord result;
        MEM_DATA(result);
//        ZERO_EXTEND(result, WWIDTH / 4);
        IO(2) = static_cast<unsigned char>(result);
        RETURN_UPDATED;
    } else {
        RETURN_NOT_UPDATED;
    }
END_LATE_RESULT;

END_OPERATION(LDBU)

OPERATION(NOT)

TRIGGER
    IO(2) = ~static_cast<SIntWord>(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(NOT)

OPERATION(ZXHW)

TRIGGER
    IO(2) = INT(1) & 0x0000FFFF;
    RETURN_READY;
END_TRIGGER;

END_OPERATION(ZXHW)
