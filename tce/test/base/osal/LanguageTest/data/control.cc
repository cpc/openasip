/**
 * @file control.cc
 *
 * Test: operations that use the control registers (PC and return address).
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */


#include "OSAL.hh"


OPERATION(CALL)
   TRIGGER
        UIntWord instructionSize = 1;
        RETURN_ADDRESS = PROGRAM_COUNTER + instructionSize;
        PROGRAM_COUNTER = UINT(1);
        RETURN_READY;
   END_TRIGGER;
END_OPERATION(CALL)

OPERATION(RET)
   TRIGGER
        PROGRAM_COUNTER = RETURN_ADDRESS.uIntWordValue();
        RETURN_READY;
   END_TRIGGER;
END_OPERATION(RET)
