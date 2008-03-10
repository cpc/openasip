/**
 * @file simple.cc
 *
 * Test: simple operation definitions.
 *
 * Operations contain no state and define only the main simulation function.
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi)
 */


#include "OSAL.hh"

OPERATION(ADD4);

TRIGGER

    IO(5) = INT(1) + INT(2) + INT(3) + INT(4);
    RETURN_READY;

END_TRIGGER;

END_OPERATION(ADD4);

OPERATION(FLOATMUL3)

TRIGGER
    IO(4) = FLT(1)*FLT(2)*FLT(3);
    RETURN_READY;
END_TRIGGER;

END_OPERATION(FLOATMUL3);

OPERATION(DOUBLESUB3)

TRIGGER

    // test indirect indexing of operands
    int index = 1;
    DoubleWord one, two, three;
    one = DBL(1);
    two = DBL(index + 1);
    three = DBL(3);
    IO(4) = one - two - three;
    RETURN_READY;

END_TRIGGER;

END_OPERATION(DOUBLESUB3);

// test operation that lacks return statement

OPERATION(MISSING_RETURN_STATEMENT)

TRIGGER
    IO(1) = 3 + 3;
    RETURN_READY;
END_TRIGGER;

END_OPERATION(MISSING_RETURN_STATEMENT);
