/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @author Ari Mets‰halme 2004 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include <iostream>
#include <fstream>
#include <math.h> // isnan()

#include "OSAL.hh"
#include "TCEString.hh"
#include "OperationGlobals.hh"
#include "Application.hh"
#include "Conversion.hh"

#define RUNTIME_ERROR_WITH_INT(MESSAGE, DATA) {\
       int len = strlen(MESSAGE) + 15;                \
       char *tmpBuf = static_cast<char*>(alloca(len));\
       snprintf(tmpBuf, len, "%s %d", MESSAGE, DATA); \
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

// Base, vital operations:

//////////////////////////////////////////////////////////////////////////////
// ADD - integer add
//////////////////////////////////////////////////////////////////////////////
OPERATION(ADD)

TRIGGER
    IO(3) = UINT(1) + UINT(2);
END_TRIGGER;

END_OPERATION(ADD)

//////////////////////////////////////////////////////////////////////////////
// SUB - integer subtract
//////////////////////////////////////////////////////////////////////////////
OPERATION(SUB)

TRIGGER
    IO(3) = UINT(1) - UINT(2);
END_TRIGGER;

END_OPERATION(SUB)

//////////////////////////////////////////////////////////////////////////////
// LDW - load 4 minimum addressable units from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDW)

TRIGGER
   if (UINT(1) % 4 != 0) 
       RUNTIME_ERROR_WITH_INT(
           "Memory access alignment error, address: ", UINT(1));
    UIntWord data;
    MEMORY.read(UINT(1), 4, data);
    IO(2) = data;
END_TRIGGER;

END_OPERATION(LDW)

//////////////////////////////////////////////////////////////////////////////
// LDQ - load 1 mimimum addressable unit from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDQ)

TRIGGER
    UIntWord data;
    MEMORY.read(UINT(1), 1, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE);
END_TRIGGER;

END_OPERATION(LDQ)

//////////////////////////////////////////////////////////////////////////////
// LDH - load 2 mimimum addressable units from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDH)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    UIntWord data;
    MEMORY.read(UINT(1), 2, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LDH)

//////////////////////////////////////////////////////////////////////////////
// LDD - load double word (64 bits) from memory
//
// @todo: needs a proper implementation for different MAUs. Currently only
//        works with byte addressed memory in the target.
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDD)

TRIGGER
    if (UINT(1) % 4 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    DoubleWord d;
    MEMORY.read(UINT(1), d);
    IO(2) = d;
END_TRIGGER;

END_OPERATION(LDD)

//////////////////////////////////////////////////////////////////////////////
// STW - store 4 minimum addressable units to memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(STW)

TRIGGER
    if (UINT(1) % 4 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    MEMORY.write(UINT(1), 4, UINT(2));
END_TRIGGER;

END_OPERATION(STW)

//////////////////////////////////////////////////////////////////////////////
// STQ - store 1 mimimum addressable unit to memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(STQ)

TRIGGER
    MEMORY.write(UINT(1), 1, UINT(2));
END_TRIGGER;

END_OPERATION(STQ)

//////////////////////////////////////////////////////////////////////////////
// STH - store 2 mimimum addressable units to memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(STH)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    MEMORY.write(UINT(1), 2, UINT(2));
END_TRIGGER;

END_OPERATION(STH)

//////////////////////////////////////////////////////////////////////////////
// STD  - store double word (64 bits) to memory
//
// @todo: needs a proper implementation for different MAUs. Currently only
//        works with byte addressed memory in the target.
//////////////////////////////////////////////////////////////////////////////
OPERATION(STD)

TRIGGER
    if (UINT(1) % 4 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    assert(MAU_SIZE == sizeof(Byte)*8 && 
           "STD works only with byte sized MAU at the moment.");

    MEMORY.write(UINT(1), DBL(2));
END_TRIGGER;

END_OPERATION(STD)

//////////////////////////////////////////////////////////////////////////////
// EQ - compare equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQ)

TRIGGER
    if (UINT(1) == UINT(2)) {
        IO(3) = 1;
    } else {
        IO(3) = 0;
    }
END_TRIGGER;

END_OPERATION(EQ)

//////////////////////////////////////////////////////////////////////////////
// GT - compare greater
//////////////////////////////////////////////////////////////////////////////
OPERATION(GT)

TRIGGER
    SIntWord in1 = static_cast<SIntWord>(UINT(1));
    SIntWord in2 = static_cast<SIntWord>(UINT(2));
    IO(3) = (in1 > in2) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GT)

//////////////////////////////////////////////////////////////////////////////
// GTU - compare greater, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTU)

TRIGGER
    IO(3) = (UINT(1) > UINT(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GTU)


//////////////////////////////////////////////////////////////////////////////
// SHL - bit shift left
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHL)

TRIGGER
    IO(3) = UINT(1) << UINT(2);
END_TRIGGER;

END_OPERATION(SHL)

//////////////////////////////////////////////////////////////////////////////
// SHR  - bit shift right, signed (arithmetic)
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHR)

TRIGGER
    SIntWord int1 = static_cast<SIntWord>(UINT(1));
    SIntWord int2 = static_cast<SIntWord>(UINT(2));

    if (int2 > MIN(
            static_cast<SIntWord>(BWIDTH(1)), 
            static_cast<SIntWord>(OSAL_WORD_WIDTH))) {
        IO(3) = 0;
        return true;
    }
    
    SIntWord int3 = int1 >> int2;
    IO(3) = static_cast<SIntWord>(int3);
END_TRIGGER;

END_OPERATION(SHR)

//////////////////////////////////////////////////////////////////////////////
// SHRU - bit shift right, unsigned (logical)
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHRU)

TRIGGER
    UIntWord in1 = UINT(1);
    UIntWord in2 = UINT(2);
    
    if (in2 > MIN(
            static_cast<UIntWord>(BWIDTH(1)), 
            static_cast<UIntWord>(OSAL_WORD_WIDTH))) {
        IO(3) = 0;
        return true;
    }
    
    IO(3) = in1 >> in2;
END_TRIGGER;

END_OPERATION(SHRU)

//////////////////////////////////////////////////////////////////////////////
// AND - bitwise and
//////////////////////////////////////////////////////////////////////////////
OPERATION(AND)

TRIGGER
    IO(3) = UINT(1) & UINT(2);
END_TRIGGER;

END_OPERATION(AND)

//////////////////////////////////////////////////////////////////////////////
// IOR - inclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(IOR)

TRIGGER
    IO(3) = UINT(1) | UINT(2);
END_TRIGGER;

END_OPERATION(IOR)

//////////////////////////////////////////////////////////////////////////////
// XOR - exclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(XOR)

TRIGGER
    IO(3) = UINT(1) ^ UINT(2);
END_TRIGGER;

END_OPERATION(XOR)

//////////////////////////////////////////////////////////////////////////////
// JUMP - absolute jump
//////////////////////////////////////////////////////////////////////////////
OPERATION(JUMP)

TRIGGER
    PROGRAM_COUNTER = UINT(1);
END_TRIGGER;

END_OPERATION(JUMP)

//////////////////////////////////////////////////////////////////////////////
// CALL - absolute call
//////////////////////////////////////////////////////////////////////////////
OPERATION(CALL)

TRIGGER
    // save the address of the instruction to return to
    SAVE_RETURN_ADDRESS;
    PROGRAM_COUNTER = UINT(1);
END_TRIGGER;

END_OPERATION(CALL)

//////////////////////////////////////////////////////////////////////////////
// MIN - integer minimum
//////////////////////////////////////////////////////////////////////////////
OPERATION(MIN)

TRIGGER
    SIntWord in1 = static_cast<SIntWord>(INT(1));
    SIntWord in2 = static_cast<SIntWord>(INT(2));
    SIntWord in3 = (in1 < in2) ? in1 : in2;
    IO(3) = static_cast<SIntWord>(in3);
END_TRIGGER;

END_OPERATION(MIN)

//////////////////////////////////////////////////////////////////////////////
// MAX - integer maximum
//////////////////////////////////////////////////////////////////////////////
OPERATION(MAX)

TRIGGER
    SIntWord in1 = static_cast<SIntWord>(INT(1));
    SIntWord in2 = static_cast<SIntWord>(INT(2));
    SIntWord in3 = (in1 > in2) ? in1 : in2;
    IO(3) = static_cast<SIntWord>(in3);
END_TRIGGER;

END_OPERATION(MAX)

//////////////////////////////////////////////////////////////////////////////
// MINU - integer minimum, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MINU)

TRIGGER
    UIntWord in1 = UINT(1);
    UIntWord in2 = UINT(2);

    if (in1 < in2) {
        IO(3) = in1;
    } else {
        IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MINU)

//////////////////////////////////////////////////////////////////////////////
// MAXU - integer maximum, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MAXU)

TRIGGER
    UIntWord in1 = UINT(1);
    UIntWord in2 = UINT(2);

    if (in1 > in2) {
        IO(3) = in1;
    } else {
        IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MAXU)

//////////////////////////////////////////////////////////////////////////////
// SXQW - sign extend from 8 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXQW)

TRIGGER
    IO(2) = SIGN_EXTEND(UINT(1), 8);
END_TRIGGER;

END_OPERATION(SXQW)

//////////////////////////////////////////////////////////////////////////////
// SXHW - sign extend from 16 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXHW)

TRIGGER
    IO(2) = SIGN_EXTEND(UINT(1), 16);
END_TRIGGER;

END_OPERATION(SXHW)

//////////////////////////////////////////////////////////////////////////////
// NEG  - arithmetic negation
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEG)

TRIGGER
    SIntWord in1 = static_cast<SIntWord>(UINT(1));
    in1 = -in1;
    IO(2) = static_cast<SIntWord>(in1);
END_TRIGGER;

END_OPERATION(NEG)

//////////////////////////////////////////////////////////////////////////////
// NOT  - bitwise negation
//////////////////////////////////////////////////////////////////////////////
OPERATION(NOT)

TRIGGER
    SIntWord in1 = static_cast<SIntWord>(UINT(1));
    in1 = ~in1;
    IO(2) = static_cast<SIntWord>(in1);
END_TRIGGER;

END_OPERATION(NOT)

//////////////////////////////////////////////////////////////////////////////
// NEQF - arithmetic negation, floating-point
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEGF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE) {
        FloatWord in = FLT(1);
        in = -in;
        IO(2) = in;
    } else if (BWIDTH(1) == DBL_WORD_SIZE) {
        DoubleWord in = DBL(1);
        in = -in;
        IO(2) = in;
    } else {
        abortWithError("bit width of operand erronous");
    }
END_TRIGGER;

END_OPERATION(NEGF)

//////////////////////////////////////////////////////////////////////////////
// MUL - integer multiply
//////////////////////////////////////////////////////////////////////////////
OPERATION(MUL)

TRIGGER
    IO(3) = UINT(1)*UINT(2);
END_TRIGGER;

END_OPERATION(MUL)

//////////////////////////////////////////////////////////////////////////////
// DIV - integer divide
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIV)

TRIGGER
    if (UINT(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    IO(3) = static_cast<SIntWord>(
	(static_cast<SIntWord>(UINT(1)) / static_cast<SIntWord>(UINT(2))));
END_TRIGGER;

END_OPERATION(DIV)

//////////////////////////////////////////////////////////////////////////////
// DIVU - integer divide, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIVU)

TRIGGER
    if (UINT(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    IO(3) = UINT(1) / UINT(2);
END_TRIGGER;

END_OPERATION(DIVU)

//////////////////////////////////////////////////////////////////////////////
// MOD - integer modulo
//////////////////////////////////////////////////////////////////////////////
OPERATION(MOD)

TRIGGER
    if (UINT(2) == 0)
         RUNTIME_ERROR("Divide by zero.")
            
    SIntWord in1 = static_cast<SIntWord>(UINT(1));
    SIntWord in2 = static_cast<SIntWord>(UINT(2));
    SIntWord out1 = in1 % in2;
    IO(3) = static_cast<SIntWord>(out1);
END_TRIGGER;

END_OPERATION(MOD)

//////////////////////////////////////////////////////////////////////////////
// MODU - integer modulo, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MODU)

TRIGGER
    if (UINT(2) == 0)
         RUNTIME_ERROR("Divide by zero.")    

    UIntWord tempIn1 = UINT(1);
    UIntWord tempIn2 = UINT(2);
    unsigned int in1 = static_cast<unsigned int>(tempIn1);
    unsigned int in2 = static_cast<unsigned int>(tempIn2);

    unsigned int out1 = in1 % in2;

    IO(3) = static_cast<UIntWord>(out1);
END_TRIGGER;

END_OPERATION(MODU)


//////////////////////////////////////////////////////////////////////////////
// ADDF - floating-point add
//////////////////////////////////////////////////////////////////////////////
OPERATION(ADDF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = FLT(1) + FLT(2);
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = DBL(1) + DBL(2);
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(ADDF)

//////////////////////////////////////////////////////////////////////////////
// SUBF - floating-point subtract
//////////////////////////////////////////////////////////////////////////////
OPERATION(SUBF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = FLT(1) - FLT(2);
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = DBL(1) - DBL(2);
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(SUBF)

//////////////////////////////////////////////////////////////////////////////
// MULF - floating-point multiply
//////////////////////////////////////////////////////////////////////////////
OPERATION(MULF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = FLT(1) * FLT(2);
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = DBL(1) * DBL(2);
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(MULF)

//////////////////////////////////////////////////////////////////////////////
// DIVF - floating-point divide
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIVF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = FLT(1) / FLT(2);
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = DBL(1) / DBL(2);
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(DIVF)

//////////////////////////////////////////////////////////////////////////////
// EQF - floating-point compare equal (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) == FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) == DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(EQF)

  
//////////////////////////////////////////////////////////////////////////////
// EQUF - floating-point compare equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) == FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) == DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(EQUF)

  
//////////////////////////////////////////////////////////////////////////////
// NEF - floating-point compare not equal (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) != FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) != DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(NEF)

  
//////////////////////////////////////////////////////////////////////////////
// NEUF - floating-point compare not equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) != FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) != DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(NEUF)

//////////////////////////////////////////////////////////////////////////////
// GTF - floating-point compare greater (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) > FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) > DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GTF)

//////////////////////////////////////////////////////////////////////////////
// GTUF - floating-point compare greater (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) > FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) > DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GTUF)

//////////////////////////////////////////////////////////////////////////////
// GEF - floating-point compare greater or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(GEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) >= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) >= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GEF)

//////////////////////////////////////////////////////////////////////////////
// GEUF - floating-point compare greater or equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(GEUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) >= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) >= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GEUF)

//////////////////////////////////////////////////////////////////////////////
// LTF - floating-point compare lower (ordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LTF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) < FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) < DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LTF)

  
//////////////////////////////////////////////////////////////////////////////
// LTUF - floating-point compare lower (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LTUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) < FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) < DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LTUF)

//////////////////////////////////////////////////////////////////////////////
// LEF - floating-point compare lower or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(LEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2)) && FLT(1) <= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2)) && DBL(1) <= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LEF)

//////////////////////////////////////////////////////////////////////////////
// LEUF - floating-point compare lower or equal (unordered)
//////////////////////////////////////////////////////////////////////////////
OPERATION(LEUF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2)) || FLT(1) <= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2)) || DBL(1) <= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LEUF)


//////////////////////////////////////////////////////////////////////////////
// ORDF - floating-point order check
//////////////////////////////////////////////////////////////////////////////
OPERATION(ORDF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (!isnan(FLT(1)) && !isnan(FLT(2))) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (!isnan(DBL(1)) && !isnan(DBL(2))) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(ORDF)

	 
//////////////////////////////////////////////////////////////////////////////
// UORDF - floating-point unorder check
//////////////////////////////////////////////////////////////////////////////
OPERATION(UORDF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (isnan(FLT(1)) || isnan(FLT(2))) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (isnan(DBL(1)) || isnan(DBL(2))) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(UORDF)

//////////////////////////////////////////////////////////////////////////////
// CFI  - convert floating-point to integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CFI)

TRIGGER
    FloatWord in = FLT(1);
    SIntWord out = static_cast<SIntWord>(in);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CFI)

//////////////////////////////////////////////////////////////////////////////
// CFIU  - convert floating-point to unsigned integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CFIU)

TRIGGER
    FloatWord in = FLT(1);
    UIntWord out = static_cast<UIntWord>(in);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CFIU)

//////////////////////////////////////////////////////////////////////////////
// CIF - convert integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CIF)

TRIGGER
    SIntWord in = INT(1);
    IO(2) = static_cast<FloatWord>(in);
END_TRIGGER;

END_OPERATION(CIF)

//////////////////////////////////////////////////////////////////////////////
// CIFU - convert unsigned integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CIFU)

TRIGGER
    UIntWord in = UINT(1);
    IO(2) = static_cast<FloatWord>(in);
END_TRIGGER;

END_OPERATION(CIFU)

//////////////////////////////////////////////////////////////////////////////
// CFD  - convert single precision floating-point to double
//////////////////////////////////////////////////////////////////////////////
OPERATION(CFD)

TRIGGER
    FloatWord in = FLT(1);
    IO(2) = static_cast<Double>(in);
END_TRIGGER;

END_OPERATION(CFD)

//////////////////////////////////////////////////////////////////////////////
// CDF - convert double precision floating-point to single precision
//////////////////////////////////////////////////////////////////////////////
OPERATION(CDF)

TRIGGER
    DoubleWord in = DBL(1);
    IO(2) = static_cast<FloatWord>(in);
END_TRIGGER;

END_OPERATION(CDF)

//////////////////////////////////////////////////////////////////////////////
// CHF - convert half precision floating-point to single precision
//////////////////////////////////////////////////////////////////////////////
OPERATION(CHF)

TRIGGER
    HalfFloatWord in = HFLT(1);
    IO(2) = FloatWord(in);
END_TRIGGER;

END_OPERATION(CHF)

//////////////////////////////////////////////////////////////////////////////
// CFH  - convert single precision floating-point to half
//////////////////////////////////////////////////////////////////////////////
OPERATION(CFH)

TRIGGER
    FloatWord in = FLT(1);
    IO(2) = HalfFloatWord(in);
END_TRIGGER;

END_OPERATION(CFH)

//////////////////////////////////////////////////////////////////////////////
// INVSQRTH - half-float inverse square root
//////////////////////////////////////////////////////////////////////////////
OPERATION(INVSQRTH)

TRIGGER
    HalfFloatWord in = HFLT(1);
    FloatWord infl(in);
    FloatWord result( 1/sqrt(infl) );
    IO(2) = HalfFloatWord(result);
END_TRIGGER;

END_OPERATION(INVSQRTH)


//////////////////////////////////////////////////////////////////////////////
// ROTL - rotate left
//////////////////////////////////////////////////////////////////////////////

OPERATION(ROTL)

TRIGGER
    UIntWord in1 = UINT(1);
    UIntWord in2 = UINT(2);
    in2 = in2 % MIN(
        static_cast<UIntWord>(BWIDTH(1)), 
        static_cast<UIntWord>(OSAL_WORD_WIDTH));
    IO(3) = (in1 << in2) | (in1 >> 
                             (MIN(
                                 static_cast<UIntWord>(BWIDTH(1)), 
                                 static_cast<UIntWord>(OSAL_WORD_WIDTH)) - 
                              in2));
END_TRIGGER;

END_OPERATION(ROTL)

//////////////////////////////////////////////////////////////////////////////
// ROTR - rotate right
//////////////////////////////////////////////////////////////////////////////

OPERATION(ROTR)

TRIGGER
    UIntWord in1 = UINT(1);
    UIntWord in2 = UINT(2);
    in2 = in2 % MIN(
        static_cast<UIntWord>(BWIDTH(1)), 
        static_cast<UIntWord>(OSAL_WORD_WIDTH));
    IO(3) = (in1 >> in2) | (in1 << (MIN(
                                         static_cast<UIntWord>(BWIDTH(1)), 
                                         static_cast<UIntWord>(OSAL_WORD_WIDTH))
                                     - in2));
END_TRIGGER;

END_OPERATION(ROTR)

//////////////////////////////////////////////////////////////////////////////
// ABS - absolute value
//////////////////////////////////////////////////////////////////////////////

OPERATION(ABS)

TRIGGER
    SIntWord temp = static_cast<SIntWord>(UINT(1));
    if (temp < 0) {
        temp = -temp;
    }
    IO(2) = temp;
END_TRIGGER;

END_OPERATION(ABS)

//////////////////////////////////////////////////////////////////////////////
// ABSF - floating-point absolute value
//////////////////////////////////////////////////////////////////////////////

OPERATION(ABSF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE) {
        FloatWord in1 = FLT(1);
        if (in1 < 0) {
            in1 = -in1;
        }
        IO(2) = in1;
    } else if (BWIDTH(1) == DBL_WORD_SIZE) {
        DoubleWord in = DBL(1);
        if (in < 0) {
            in = -in;
        }
        IO(2) = in;
    } else { 
        abortWithError("bit width of operand erronous");
    }
END_TRIGGER;

END_OPERATION(ABSF)

//////////////////////////////////////////////////////////////////////////////
// SQRTF - floating-point square root
//////////////////////////////////////////////////////////////////////////////

OPERATION(SQRTF)

TRIGGER
    IO(2) = sqrtf(FLT(1));
END_TRIGGER;

END_OPERATION(SQRTF)

//////////////////////////////////////////////////////////////////////////////
// MAXF - higher of two floating-point values
//////////////////////////////////////////////////////////////////////////////

OPERATION(MAXF)

TRIGGER
    FloatWord in1 = FLT(1);
    FloatWord in2 = FLT(2);
    if( in1 > in2 ) {
      IO(3) = in1;
    }
    else {
      IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MAXF)

//////////////////////////////////////////////////////////////////////////////
// MINF - lower of two floating-point values
//////////////////////////////////////////////////////////////////////////////

OPERATION(MINF)

TRIGGER
    FloatWord in1 = FLT(1);
    FloatWord in2 = FLT(2);
    if( in1 < in2 ) {
      IO(3) = in1;
    }
    else {
      IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MINF)

//////////////////////////////////////////////////////////////////////////////
// LDQU - load 1 mimimum addressable unit from memory (unsigned)
//////////////////////////////////////////////////////////////////////////////

OPERATION(LDQU)

TRIGGER
    UIntWord data;
    MEMORY.read(UINT(1), 1, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE);
END_TRIGGER;

END_OPERATION(LDQU)

//////////////////////////////////////////////////////////////////////////////
// LDHU - load 2 mimimum addressable units from memory (unsigned)
//////////////////////////////////////////////////////////////////////////////

OPERATION(LDHU)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    UIntWord data;
    MEMORY.read(UINT(1), 2, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LDHU)

//////////////////////////////////////////////////////////////////////////////
// STDOUT - Output a byte of data to "standard output", whatever it might
//          be on the platform at hand. Prints the char to simulator console.
//////////////////////////////////////////////////////////////////////////////
OPERATION(STDOUT)

TRIGGER
    OUTPUT_STREAM << static_cast<char>(INT(1));
END_TRIGGER;

END_OPERATION(STDOUT)

//////////////////////////////////////////////////////////////////////////////
// LMBD - Leftmost bit detection.
//////////////////////////////////////////////////////////////////////////////
OPERATION(LMBD)

TRIGGER
    int foundBits = 0;
    UIntWord word = UINT(1);
    const UIntWord bitToSearch = INT(2);
    if (bitToSearch > 1)
        RUNTIME_ERROR("LMDB's 2nd operand must be 0 or 1!");
    const int BITS_IN_WORD = sizeof(UIntWord)*8;
    for (; foundBits <= BITS_IN_WORD; ++foundBits) {
        // check the top bit: is the searched bit?
        if (word >> (BITS_IN_WORD - 1) == bitToSearch)
            break; // found the bit
        word <<= 1;
    }
    IO(3) = foundBits;
END_TRIGGER;

END_OPERATION(LMBD)

//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM - State definition for the STREAM_IN.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = context.functionUnitName() + ".in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN - Reads a sample from the default input stream.
//
// @todo: Support for other sample sizes than 8.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN, INPUT_STREAM)

TRIGGER
    if (BWIDTH(2) != 8) {
        Application::logStream() 
            << "STREAM_IN works with bytes only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
    char input;
    STATE.inputFile.read(&input, 1);
    IO(2) = static_cast<int>(input);
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS, INPUT_STREAM)

TRIGGER

    /*
        0 = buffer empty
        1 = not empty nor full 
        2 = buffer full (the simulated input buffer is never full)
    */

    // test if there's more data to read
    char input;
    STATE.inputFile.read(&input, 1);
    if (STATE.inputFile.eof()) {
        IO(2) = 0;
    } else {
        IO(2) = 1;
    }
    STATE.inputFile.putback(input);
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS)

//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM - State definition for the STREAM_OUT.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = context.functionUnitName() + ".out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 8.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT, OUTPUT_STREAM)

TRIGGER

    if (BWIDTH(1) != 8) 
        Application::logStream() 
            << "STREAM_OUT works with bytes only at the moment." 
            << std::endl;

    char data = static_cast<char>(UINT(1));
    STATE.outputFile.write(&data, 1);
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS, OUTPUT_STREAM)

TRIGGER
    IO(2) = 0;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS)

//////////////////////////////////////////////////////////////////////////////
// RTIMER - Counts real time (in microseconds) down from the given value.
//
// When triggered with 0, returns the current timer value, when triggered 
// with a value greater than 0, sets the timer value to the written value.
//
// The simulation behavior definition assumes 100MHz clock frequency, 
// which can be modified using the environment variable TCE_RTIMER_CLOCK=MHZ.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(TIMER_VALUE)
    // current value of the timer
    double value;
    // number of usecs per cycle
    double stepping;
    // last cycle count reading
    CycleCount lastCycleCount;
INIT_STATE(TIMER_VALUE)
    
    int clockFrequencyMHz = 100;

    const char* freqFromEnv = getenv("TCE_RTIMER_CLOCK");
    if (freqFromEnv != NULL) {
        clockFrequencyMHz = Conversion::toInt(freqFromEnv);
    } 

    value = 0.0;

    // How many usec per cycle? Inverse of clock frequency in MHz.
    stepping = 1.0 / clockFrequencyMHz;

    lastCycleCount = 0;

END_INIT_STATE;

END_DEFINE_STATE


OPERATION_WITH_STATE(RTIMER, TIMER_VALUE)

TRIGGER
    // update the timer using the passed cycles
    CycleCount currentCycles = CYCLE_COUNT;
    STATE.value -= (currentCycles - STATE.lastCycleCount)*STATE.stepping;
    // saturate to 0
    STATE.value = std::max(STATE.value, 0.0);

    STATE.lastCycleCount = currentCycles;


    UIntWord value = UINT(1);
    if (value == 0) {
        IO(2) = static_cast<UIntWord>(STATE.value);
    } else {
        IO(2) = static_cast<UIntWord>(STATE.value);
        STATE.value = static_cast<double>(value);   
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(RTIMER)

 
//////////////////////////////////////////////////////////////////////////////
// RTC - Counts real time (in microseconds) up from zero.
//
// When triggered with 0, resets the the current real time value, when triggered 
// with a value greater than 0, returns current real time value.
//
// The simulation behavior definition assumes 100MHz clock frequency, 
// which can be modified using the environment variable TCE_RTIMER_CLOCK=MHZ.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(RTC_VALUE)
    // current value of the clock
    double value;
    // number of usecs per cycle
    double stepping;
    // last cycle count reading
    CycleCount resetCycleCount;
INIT_STATE(RTC_VALUE)
    
    int clockFrequencyMHz = 100;

    const char* freqFromEnv = getenv("TCE_RTIMER_CLOCK");
    if (freqFromEnv != NULL) {
        clockFrequencyMHz = Conversion::toInt(freqFromEnv);
    } 

    value = 0.0;

    // How many usec per cycle? Inverse of clock frequency in MHz.
    stepping = 1.0 / clockFrequencyMHz;

    resetCycleCount = 0;

END_INIT_STATE;

END_DEFINE_STATE


OPERATION_WITH_STATE(RTC, RTC_VALUE)

TRIGGER
    // update the timer
    CycleCount currentCycles = CYCLE_COUNT;
    STATE.value = (currentCycles - STATE.resetCycleCount)*STATE.stepping;

    UIntWord value = UINT(1);
    if (value == 0) {
        STATE.resetCycleCount = currentCycles;
        STATE.value = 0;
        IO(2) = 0;
    } else {
        IO(2) = static_cast<UIntWord>(STATE.value);
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(RTC)

//////////////////////////////////////////////////////////////////////////////
// LEDS - Switches leds on and off.
//
// Bit 1 switches led on and 0 switches it off. Bit position corresponds to
// the led position
//////////////////////////////////////////////////////////////////////////////
OPERATION(LEDS)

TRIGGER
     unsigned int dummy = UINT(1);
     dummy = dummy;
END_TRIGGER;

END_OPERATION(LEDS)


//////////////////////////////////////////////////////////////////////////////
// BCAST2 - broadcast to two vector lanes
//////////////////////////////////////////////////////////////////////////////
OPERATION(BCAST2)

TRIGGER
IO(2) = IO(1);
IO(3) = IO(1);
END_TRIGGER;

END_OPERATION(BCAST2)

//////////////////////////////////////////////////////////////////////////////
// BCAST4 - broadcast to four vector lanes
//////////////////////////////////////////////////////////////////////////////
OPERATION(BCAST4)

TRIGGER
IO(2) = IO(1);
IO(3) = IO(1);
IO(4) = IO(1);
IO(5) = IO(1);
END_TRIGGER;

END_OPERATION(BCAST4)

//////////////////////////////////////////////////////////////////////////////
// BCAST8 - broadcast to four vector lanes
//////////////////////////////////////////////////////////////////////////////
OPERATION(BCAST8)

TRIGGER
IO(2) = IO(1);
IO(3) = IO(1);
IO(4) = IO(1);
IO(5) = IO(1);
IO(6) = IO(1);
IO(7) = IO(1);
IO(8) = IO(1);
IO(9) = IO(1);
END_TRIGGER;

END_OPERATION(BCAST8)

//////////////////////////////////////////////////////////////////////////////
// ECC - Execution cycle count
//////////////////////////////////////////////////////////////////////////////
OPERATION(ECC)
TRIGGER
int dummy = INT(1);
dummy = dummy;
IO(2) = static_cast<unsigned int>(CYCLE_COUNT);
END_TRIGGER;
END_OPERATION(ECC)

//////////////////////////////////////////////////////////////////////////////
// LCC - (Global) Lock cycle count
//////////////////////////////////////////////////////////////////////////////
OPERATION(LCC)
TRIGGER
int dummy = INT(1);
dummy = dummy;
// Simulator does not model global lock, return 0
IO(2) = 0;
END_TRIGGER;
END_OPERATION(LCC)


