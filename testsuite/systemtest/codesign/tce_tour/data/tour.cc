/**
 * OSAL behavior definition file.
 */

#include "OSAL.hh"

OPERATION(ADD4)
TRIGGER
    IO(5) = INT(1) + INT(2) + INT(3) + INT(4);
    RETURN_READY;
END_TRIGGER
END_OPERATION(ADD4)
