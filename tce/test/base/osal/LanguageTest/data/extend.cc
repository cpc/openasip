/**
 * @file extend.cc
 *
 * Operations for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include "OSAL.hh"

OPERATION(Z_EXTEND)
TRIGGER
    UIntWord in1 = UINT(1);
    IO(2) = ZERO_EXTEND(in1, BWIDTH(1));
    RETURN_READY;
END_TRIGGER;
END_OPERATION(Z_EXTEND)

OPERATION(S_EXTEND)
TRIGGER
    SIntWord in1 = INT(1);
    IO(2) = SIGN_EXTEND(in1, BWIDTH(1));
    RETURN_READY;
END_TRIGGER;
END_OPERATION(S_EXTEND)
