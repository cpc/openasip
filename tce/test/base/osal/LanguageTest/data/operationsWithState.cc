/**
 * @file operationsWithState.cc
 *
 * Test definition of operation definitions with state.
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */

#include "OSAL.hh"

DEFINE_STATE(ACCUSTATE)

    UIntWord accu;
    INIT_STATE(ACCUSTATE) 
       accu = 0;
    END_INIT_STATE;

    FINALIZE_STATE(ACCUSTATE)
       // just to test that the destructor macro compiles
    END_FINALIZE_STATE;

END_DEFINE_STATE

OPERATION_WITH_STATE(ACCADD, ACCUSTATE)

    TRIGGER
         SIntWord input = INT(1);
         STATE.accu += input;
         IO(2) = STATE.accu;
         RETURN_READY;
    END_TRIGGER;

END_OPERATION_WITH_STATE(ACCADD)

OPERATION_WITH_STATE(ACCSUB, ACCUSTATE)

    TRIGGER
         SIntWord input = INT(1);
         STATE.accu -= input;
         IO(2) = STATE.accu;
         RETURN_READY;
    END_TRIGGER;

END_OPERATION_WITH_STATE(ACCSUB)
