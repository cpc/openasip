/*
    Copyright (c) 2002-2014 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file base.cc
 *
 * Base operation definitions.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Ari Metsähalme 2004 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2005-2014 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include <cmath> // std::isnan()
#include <fstream>
#include <iostream>

#include "OSAL.hh"
#include "TCEString.hh"
#include "OperationGlobals.hh"
#include "Application.hh"
#include "Conversion.hh"

using namespace std;

#define RUNTIME_ERROR_WITH_INT(MESSAGE, DATA) {\
       int len = strlen(MESSAGE) + 15;                \
       char *tmpBuf = static_cast<char*>(alloca(len));\
       snprintf(tmpBuf, len, "%s %d", MESSAGE, DATA); \
       OperationGlobals::runtimeError(                \
           tmpBuf, __FILE__, __LINE__, parent_);      \
}

#define RUNTIME_ERROR_WITH_LONG(MESSAGE, DATA) {\
       int len = strlen(MESSAGE) + 21;                \
       char *tmpBuf = static_cast<char*>(alloca(len));\
       snprintf(tmpBuf, len, "%s %ld", MESSAGE, (long)DATA);    \
       OperationGlobals::runtimeError(                \
           tmpBuf, __FILE__, __LINE__, parent_);      \
}

// A macro to obtain maximum value that can be represented with 'x' bits.
// NOTE: If this is needed a lot it should be in the OSAL
// language. Currently I believe it will not be needed too much, and
// hence it could be removed.
#define MAX_VALUE(x) (((x) < sizeof(SIntWord) * 8) ? \
                     (static_cast<SIntWord>(1 << (x)) - 1) : \
                     static_cast<SIntWord>(~0))

// Macro for obtaining minimum bit width of two operands.
#define MIN(x, y) ((x < y) ? x : y)


//////////////////////////////////////////////////////////////////////////////
// NEQD - arithmetic negation, floating-point
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEGD)

TRIGGER
DoubleWord in = DBL(1);
in = -in;
IO(2) = in;
END_TRIGGER;

END_OPERATION(NEGD)



//////////////////////////////////////////////////////////////////////////////
// ADDD - floating-point add
//////////////////////////////////////////////////////////////////////////////
OPERATION(ADDD)

TRIGGER
IO(3) = DBL(1) + DBL(2);
END_TRIGGER;

END_OPERATION(ADDD)

//////////////////////////////////////////////////////////////////////////////
// SUBD - floating-point subtract
//////////////////////////////////////////////////////////////////////////////
OPERATION(SUBD)

TRIGGER
IO(3) = DBL(1) - DBL(2);
END_TRIGGER;

END_OPERATION(SUBD)

//////////////////////////////////////////////////////////////////////////////
// MULD - floating-point multiply
//////////////////////////////////////////////////////////////////////////////
OPERATION(MULD)

TRIGGER
IO(3) = DBL(1) * DBL(2);
END_TRIGGER;

END_OPERATION(MULD)

//////////////////////////////////////////////////////////////////////////////
// DIVD - floating-point divide
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIVD)

TRIGGER
IO(3) = DBL(1) / DBL(2);
END_TRIGGER;

END_OPERATION(DIVD)

//////////////////////////////////////////////////////////////////////////////
// EQD - floating-point compare equal (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQD)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) == DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(EQD)

//////////////////////////////////////////////////////////////////////////////
// EQUD - floating-point compare equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQUD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) == DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(EQUD)

//////////////////////////////////////////////////////////////////////////////
// NED - floating-point compare not equal (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(NED)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) != DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(NED)

//////////////////////////////////////////////////////////////////////////////
// NEUD - floating-point compare not equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEUD)

TRIGGER
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) != DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(NEUD)

//////////////////////////////////////////////////////////////////////////////
// GTD - floating-point compare greater (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTD)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) > DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GTD)

//////////////////////////////////////////////////////////////////////////////
// GTUD - floating-point compare greater (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTUD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) > DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GTUD)

//////////////////////////////////////////////////////////////////////////////
// GED - floating-point compare greater or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(GED)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) >= DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GED)

//////////////////////////////////////////////////////////////////////////////
// GEUD - floating-point compare greater or equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GEUD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) >= DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GEUD)

//////////////////////////////////////////////////////////////////////////////
// LTD - floating-point compare lower (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LTD)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) < DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(LTD)

//////////////////////////////////////////////////////////////////////////////
// LTUD - floating-point compare lower (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LTUD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) < DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(LTUD)

//////////////////////////////////////////////////////////////////////////////
// LED - floating-point compare lower or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(LED)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) <= DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(LED)

//////////////////////////////////////////////////////////////////////////////
// LEUD - floating-point compare lower or equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LEUD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) <= DBL(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(LEUD)


//////////////////////////////////////////////////////////////////////////////
// ORDD - floating-point order check
//////////////////////////////////////////////////////////////////////////////
OPERATION(ORDD)

TRIGGER
IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2))) ? 1 : 0;
END_TRIGGER;

END_OPERATION(ORDD)

//////////////////////////////////////////////////////////////////////////////
// UORDD - floating-point unorder check
//////////////////////////////////////////////////////////////////////////////
OPERATION(UORDD)

TRIGGER
IO(3) = (isnan(DBL(1)) || isnan(DBL(2))) ? 1 : 0;
END_TRIGGER;

END_OPERATION(UORDD)

//////////////////////////////////////////////////////////////////////////////
// CDL  - convert floating-point to integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CDL)

TRIGGER
    DoubleWord in = DBL(1);
    SLongWord out = static_cast<SLongWord>(in);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CDL)

//////////////////////////////////////////////////////////////////////////////
// CDLU  - convert DP floating-point to integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CDLU)

TRIGGER
    DoubleWord in = DBL(1);
    SLongWord out = static_cast<ULongWord>(in);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CDLU)

//////////////////////////////////////////////////////////////////////////////
// CFIU  - convert floating-point to unsigned integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CFIU)

TRIGGER
    DoubleWord in = DBL(1);
    ULongWord out = static_cast<ULongWord>(in);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CFIU)

//////////////////////////////////////////////////////////////////////////////
// CIF - convert integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CLD)

TRIGGER
    SLongWord in = LONG(1);
    IO(2) = static_cast<DoubleWord>(in);
END_TRIGGER;

END_OPERATION(CLD)

//////////////////////////////////////////////////////////////////////////////
// CIFU - convert unsigned integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CLDU)

TRIGGER
    ULongWord in = ULONG(1);
    IO(2) = static_cast<DoubleWord>(in);
END_TRIGGER;

END_OPERATION(CLDU)

//////////////////////////////////////////////////////////////////////////////
// SQRTD - DP floating-point square root
//////////////////////////////////////////////////////////////////////////////

OPERATION(SQRTD)

TRIGGER
    IO(2) = sqrt(DBL(1));
END_TRIGGER;

END_OPERATION(SQRTD)

//////////////////////////////////////////////////////////////////////////////
// ABSF - floating-point absolute value
//////////////////////////////////////////////////////////////////////////////

OPERATION(ABSD)

TRIGGER
DoubleWord in = DBL(1);
if (in < 0) {
    in = -in;
}
IO(2) = in;
END_TRIGGER;

END_OPERATION(ABSD)
