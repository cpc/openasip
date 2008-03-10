/**
 * @file operations_with_state.cc
 *
 * Behavior definition file.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "OSAL.hh"

DEFINE_STATE(CLOCK_REGISTER)
    
    unsigned clock_register;
    bool started;

INIT_STATE(CLOCK_REGISTER)

    clock_register = 0;
    started = false;

END_INIT_STATE

ADVANCE_CLOCK

    if (started)
        ++clock_register;

END_ADVANCE_CLOCK

END_DEFINE_STATE

OPERATION_WITH_STATE(CLOCK, CLOCK_REGISTER) 

TRIGGER

    if (UINT(1) == 0) {
        STATE.started = true;
        STATE.clock_register = 0;
        IO(2) = 0;
        RETURN_READY;
    } else {
        STATE.started = true;
        IO(2) = STATE.clock_register;
        RETURN_READY;
    }

END_TRIGGER

END_OPERATION_WITH_STATE(CLOCK)

DEFINE_STATE(ACC_REGISTER)
    unsigned reg;
END_DEFINE_STATE

OPERATION_WITH_STATE(ACC, ACC_REGISTER)

TRIGGER

    if (UINT(1) == 0) {
        STATE.reg = 0;
        IO(2) = 0;
        RETURN_READY;
    } else {
        STATE.reg += UINT(1);
        IO(2) = STATE.reg;
        RETURN_READY;
    }

END_TRIGGER

END_OPERATION_WITH_STATE(ACC)
