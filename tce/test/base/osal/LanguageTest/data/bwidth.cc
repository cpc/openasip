/**
 * @file bwidth.cc
 *
 * Test: operation that uses BWIDTH macro.
 *
 * Operation which returns the value of BWIDTH macro as result.
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */


#include "OSAL.hh"


OPERATION(INPUT_BIT_WIDTH);
TRIGGER

    IO(2) = BWIDTH(1);
    RETURN_READY;

END_TRIGGER;

END_OPERATION(INPUT_BIT_WIDTH)

