/**
 * OSAL behavior definition file.
 */

#include <cmath>
#include <iostream>
#include "OSAL.hh"


OPERATION(SQRTF)

TRIGGER

DoubleWord result = std::sqrt(DBL(1));
IO(2) = result;
RETURN_READY;
END_TRIGGER;

END_OPERATION(SQRTF)
