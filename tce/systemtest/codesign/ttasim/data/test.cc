/**
 * OSAL behavior definition file.
 */

#include "OSAL.hh"

OPERATION(ID)
TRIGGER

IO(5) = IO(1);
IO(6) = IO(2);
IO(7) = IO(3);
IO(8) = IO(4);
RETURN_READY;

END_TRIGGER;
END_OPERATION(ID)
