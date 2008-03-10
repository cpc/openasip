/**
 * @file complex.cc
 *
 * Test: more complicated operation definitions
 *
 * Used for testing all simulation function declarations and 
 * return statements
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */

#include "OSAL.hh"

/// Operation without explicit RETURN statement in trigger and late-result
/// definition blocks. The return values are computed automatically.
/// Simulation functions of this operation should return false.
OPERATION(LAZY_NOT_READY)

TRIGGER
    RETURN_NOT_READY;    
END_TRIGGER;

LATE_RESULT
// returns false because no operation output is assigned a value
    RETURN_NOT_READY;
END_LATE_RESULT;

END_OPERATION(LAZY_NOT_READY);

/// Operation which cannot calculate all operands in trigger, but calculates
/// something in LATE_RESULT. 
OPERATION(LAZY_HALF_READY)

TRIGGER
   // returns false because not all operation outputs are assigned a value
   IO(1) = 1;
   RETURN_NOT_READY;
        
END_TRIGGER;

LATE_RESULT
   // returns true because a new operation output is assigned a value
   IO(2) = 2;
   SET_DONE(2);
   RETURN_READY;
END_LATE_RESULT;

END_OPERATION(LAZY_HALF_READY);

/// Tests the explicit returning from LATE_RESULT.
OPERATION(EXPLICIT_LATE_RESULT_RETURN)

TRIGGER
   // returns false because not all operation outputs are assigned a value
   IO(1) = 1;
   RETURN_NOT_READY;
        
END_TRIGGER;

LATE_RESULT
   // returns true because a new operation output is assigned a value
   IO(2) = 2;
   SET_DONE(2);
   RETURN_UPDATED;
END_LATE_RESULT;

END_OPERATION(EXPLICIT_LATE_RESULT_RETURN);
