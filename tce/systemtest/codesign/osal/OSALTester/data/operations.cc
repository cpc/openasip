/**
 * @file operations.cc
 *
 * Operation definitions for test purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include "OSAL.hh"

//////////////////////////////////////////////////////////////////////////////
// add2
//////////////////////////////////////////////////////////////////////////////
OPERATION(ADD2)

TRIGGER
    IO(3) = UINT(1) + UINT(2);
    RETURN_READY;
END_TRIGGER;

END_OPERATION(ADD2)

//////////////////////////////////////////////////////////////////////////////
// mul2
/////////////////////////////////////////////////////////////////////////////
    
OPERATION(MUL2)

TRIGGER
    IO(3) = UINT(1)*UINT(2);
    RETURN_READY;
END_TRIGGER;

END_OPERATION(MUL2)

//////////////////////////////////////////////////////////////////////////////
// accadd
/////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(AccuState)

    SIntWord accu;

    INIT_STATE(AccuState) 
        accu = 0;
    END_INIT_STATE;

    FINALIZE_STATE(AccuState)
    // just to test that the destructor macro compiles
    END_FINALIZE_STATE;

END_DEFINE_STATE
    
OPERATION_WITH_STATE(ACCADD, AccuState)

    TRIGGER
        STATE.accu += UINT(1);
        IO(2) = STATE.accu;
        RETURN_READY;
    END_TRIGGER;

END_OPERATION_WITH_STATE(ACCADD)
