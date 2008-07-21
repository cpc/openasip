/**
 * @file /OperationBehaviorLoaderTest/data/correct.cc
 *
 * oper1 behavior definition.
 * 
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */


#include "OSAL.hh"

OPERATION(OPER1);

TRIGGER
SIntWord in1;
in1 = INT(1);
IO(2) = in1 * 3;
RETURN_READY;
END_TRIGGER;

END_OPERATION(OPER1)

