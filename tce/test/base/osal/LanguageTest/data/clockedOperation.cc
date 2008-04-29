/**
 * @file clockedOperation.cc
 *
 * Test definition of operation definition which uses clock signal.
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */

#include "OSAL.hh"

/// This operation is a clock counter. The trigger returns the current
/// clock count stored in state. ADVANCE_CLOCK increases the counter.
DEFINE_STATE(CLOCK_COUNTER)

    UIntWord cycleCount;

    INIT_STATE(CLOCK_COUNTER)
        cycleCount = 0;
    END_INIT_STATE;

    ADVANCE_CLOCK
        cycleCount++;
    END_ADVANCE_CLOCK;

END_DEFINE_STATE;

OPERATION_WITH_STATE(CLOCK, CLOCK_COUNTER)

    TRIGGER
         IO(1) = STATE.cycleCount;
         RETURN_READY;
    END_TRIGGER;

END_OPERATION_WITH_STATE(CLOCK);
