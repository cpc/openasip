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
 * @author Ari Mets‰halme 2004 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2005-2014 (pekka.jaaskelainen-no.spam-tut.fi)
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
// ADD64 - long integer add
//////////////////////////////////////////////////////////////////////////////
OPERATION(ADD64)

TRIGGER
    IO(3) = ULONG(1) + ULONG(2);
END_TRIGGER;

END_OPERATION(ADD64)

//////////////////////////////////////////////////////////////////////////////
// SUB - integer subtract
//////////////////////////////////////////////////////////////////////////////
OPERATION(SUB)

TRIGGER
    IO(3) = UINT(1) - UINT(2);
END_TRIGGER;

END_OPERATION(SUB)

//////////////////////////////////////////////////////////////////////////////
// SUB64 - long integer subtract
//////////////////////////////////////////////////////////////////////////////
OPERATION(SUB64)

TRIGGER
    IO(3) = ULONG(1) - ULONG(2);
END_TRIGGER;

END_OPERATION(SUB64)

//////////////////////////////////////////////////////////////////////////////
// LDW - load a 32-bit word in big endian order from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDW)

TRIGGER
   if (UINT(1) % 4 != 0) 
       RUNTIME_ERROR_WITH_INT(
           "Memory access alignment error, address: ", UINT(1));
    ULongWord data;
    MEMORY.readBE(UINT(1), 4, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*4);
END_TRIGGER;

END_OPERATION(LDW)

//////////////////////////////////////////////////////////////////////////////
// LD32 - load a 32-bit word in little endian order from memory, sign-extend
//////////////////////////////////////////////////////////////////////////////
OPERATION(LD32)

TRIGGER
   if (ULONG(1) % 4 != 0)
       RUNTIME_ERROR_WITH_LONG(
           "Memory access alignment error, address: ", ULONG(1));
    ULongWord data;
    MEMORY.readLE(ULONG(1), 4, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*4);
END_TRIGGER;

END_OPERATION(LD32)

//////////////////////////////////////////////////////////////////////////////
// LDU32 - load a 32-bit word in little endian order from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDU32)

TRIGGER
   if (ULONG(1) % 4 != 0)
       RUNTIME_ERROR_WITH_LONG(
           "Memory access alignment error, address: ", ULONG(1));
    ULongWord data;
    MEMORY.readLE(ULONG(1), 4, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE*4);
END_TRIGGER;

END_OPERATION(LDU32)

//////////////////////////////////////////////////////////////////////////////
// LD64 - load a 64-bit word in little endian order from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LD64)

TRIGGER
   if (ULONG(1) % 8 != 0)
       RUNTIME_ERROR_WITH_LONG(
           "Memory access alignment error, address: ", ULONG(1));
    ULongWord data;
    MEMORY.readLE(ULONG(1), 8, data);
//std::cerr << "Reading 64-bit value: " << data << std::endl;
    IO(2) = data;//, MAU_SIZE*8;
END_TRIGGER;

END_OPERATION(LD64)

//////////////////////////////////////////////////////////////////////////////
// LDQ - load 1 mimimum addressable unit from memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDQ)

TRIGGER
    ULongWord data;
    MEMORY.readBE(UINT(1), 1, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE);
END_TRIGGER;

END_OPERATION(LDQ)

//////////////////////////////////////////////////////////////////////////////
// LDH - load 16-bit word in big endian order from memory with sign extension
//////////////////////////////////////////////////////////////////////////////
OPERATION(LDH)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    ULongWord data;
    MEMORY.readBE(UINT(1), 2, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LDH)

//////////////////////////////////////////////////////////////////////////////
// LD16 - load 16-bit word in little endian order from memory with sign extension
//////////////////////////////////////////////////////////////////////////////
OPERATION(LD16)

TRIGGER
    if (UINT(1) % 2 != 0)
    RUNTIME_ERROR_WITH_INT(
        "Memory access alignment error, address: ", UINT(1));
    ULongWord data;
    MEMORY.readLE(ULONG(1), 2, data);
    // See the comment in ST32.
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LD16)

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
    MEMORY.readBE(UINT(1), d);
    IO(2) = d;
END_TRIGGER;

END_OPERATION(LDD)

//////////////////////////////////////////////////////////////////////////////
// STW - store a 32-bit word to memory in big endian byte order
//////////////////////////////////////////////////////////////////////////////
OPERATION(STW)

TRIGGER
    if (UINT(1) % 4 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    MEMORY.writeBE(UINT(1), 4, UINT(2));
END_TRIGGER;

END_OPERATION(STW)

//////////////////////////////////////////////////////////////////////////////
// ST32 - store a 32-bit word to memory in little endian byte order
//////////////////////////////////////////////////////////////////////////////
OPERATION(ST32)

TRIGGER
    if (UINT(1) % 4 != 0)
    RUNTIME_ERROR_WITH_INT(
        "Memory access alignment error, address: ", UINT(1));
    UIntWord data = UINT(2);
    MEMORY.writeLE(UINT(1), 4, data);
END_TRIGGER;

END_OPERATION(ST32)

//////////////////////////////////////////////////////////////////////////////
// ST64 - store a 64-bit longword to memory in little endian byte order
//////////////////////////////////////////////////////////////////////////////
OPERATION(ST64)

TRIGGER
    if (UINT(1) % 8 != 0)
    RUNTIME_ERROR_WITH_LONG(
        "Memory access alignment error, address: ", ULONG(1));
    ULongWord data = ULONG(2);
    MEMORY.writeLE(ULONG(1), 8, data);
END_TRIGGER;

END_OPERATION(ST64)

//////////////////////////////////////////////////////////////////////////////
// STQ - store 1 mimimum addressable unit to memory
//////////////////////////////////////////////////////////////////////////////
OPERATION(STQ)

TRIGGER
    MEMORY.writeBE(UINT(1), 1, UINT(2));
END_TRIGGER;

END_OPERATION(STQ)

//////////////////////////////////////////////////////////////////////////////
// STH - store a 16-bit word to memory in big endian byte order
//////////////////////////////////////////////////////////////////////////////
OPERATION(STH)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    MEMORY.writeBE(UINT(1), 2, UINT(2));
END_TRIGGER;

END_OPERATION(STH)

//////////////////////////////////////////////////////////////////////////////
// ST16 - store a 16-bit word to memory in little endian byte order
//////////////////////////////////////////////////////////////////////////////
OPERATION(ST16)

TRIGGER
    if (UINT(1) % 2 != 0)
    RUNTIME_ERROR_WITH_INT(
        "Memory access alignment error, address: ", UINT(1));
    // See the comment in ST32.
    UIntWord data = UINT(2);
    MEMORY.writeLE(ULONG(1), 2, data);
END_TRIGGER;

END_OPERATION(ST16)

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

    MEMORY.writeBE(UINT(1), DBL(2));
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
// EQ64 - compare equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQ64)

TRIGGER
    if (ULONG(1) == ULONG(2)) {
        IO(3) = 1l;
    } else {
        IO(3) = 0l;
    }
END_TRIGGER;

END_OPERATION(EQ64)

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
// GT64 - compare greater
//////////////////////////////////////////////////////////////////////////////
OPERATION(GT64)

TRIGGER
    SLongWord in1 = LONG(1);
    SLongWord in2 = LONG(2);
    IO(3) = (in1 > in2) ? 1l : 0l;
END_TRIGGER;

END_OPERATION(GT64)

//////////////////////////////////////////////////////////////////////////////
// GTU - compare greater, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTU)

TRIGGER
    IO(3) = (UINT(1) > UINT(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GTU)

//////////////////////////////////////////////////////////////////////////////
// GTU64 - compare greater, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTU64)

TRIGGER
    IO(3) = (ULONG(1) > ULONG(2)) ? 1 : 0;
END_TRIGGER;

END_OPERATION(GTU64)


//////////////////////////////////////////////////////////////////////////////
// SHL - bit shift left
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHL)

TRIGGER
    IO(3) = UINT(1) << UINT(2);
END_TRIGGER;

END_OPERATION(SHL)

//////////////////////////////////////////////////////////////////////////////
// SHL64 - bit shift left
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHL64)

TRIGGER
    IO(3) = ULONG(1) << ULONG(2);
END_TRIGGER;

END_OPERATION(SHL64)

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
// SHR64  - bit shift right, signed (arithmetic)
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHR64)

TRIGGER
    SLongWord int1 = LONG(1);
    SLongWord int2 = LONG(2);

    if (int2 > MIN(
            static_cast<SLongWord>(BWIDTH(1)),
            static_cast<SLongWord>(OSAL_WORD_WIDTH))) {
        IO(3) = 0;
        return true;
    }

    IO(3) = int1 >> int2;
END_TRIGGER;

END_OPERATION(SHR64)

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
// SHRU64 - bit shift right, unsigned (logical)
//////////////////////////////////////////////////////////////////////////////
OPERATION(SHRU64)

TRIGGER
    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);

    if (in2 > 64) {
        IO(3) = 0l;
        return true;
    }

    IO(3) = in1 >> in2;
END_TRIGGER;

END_OPERATION(SHRU64)

//////////////////////////////////////////////////////////////////////////////
// AND - bitwise and
//////////////////////////////////////////////////////////////////////////////
OPERATION(AND)

TRIGGER
    IO(3) = UINT(1) & UINT(2);
END_TRIGGER;

END_OPERATION(AND)

//////////////////////////////////////////////////////////////////////////////
// AND64 - bitwise and
//////////////////////////////////////////////////////////////////////////////
OPERATION(AND64)

TRIGGER
    IO(3) = ULONG(1) & ULONG(2);
END_TRIGGER;

END_OPERATION(AND64)

//////////////////////////////////////////////////////////////////////////////
// IOR - inclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(IOR)

TRIGGER
    IO(3) = UINT(1) | UINT(2);
END_TRIGGER;

END_OPERATION(IOR)

//////////////////////////////////////////////////////////////////////////////
// IOR64 - inclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(IOR64)

TRIGGER
    IO(3) = ULONG(1) | ULONG(2);
END_TRIGGER;

END_OPERATION(IOR64)

//////////////////////////////////////////////////////////////////////////////
// XOR - exclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(XOR)

TRIGGER
    IO(3) = UINT(1) ^ UINT(2);
END_TRIGGER;

END_OPERATION(XOR)

//////////////////////////////////////////////////////////////////////////////
// XOR64 - exclusive bitwise or
//////////////////////////////////////////////////////////////////////////////
OPERATION(XOR64)

TRIGGER
    IO(3) = ULONG(1) ^ ULONG(2);
END_TRIGGER;

END_OPERATION(XOR64)

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
// MIN64 - long integer minimum
//////////////////////////////////////////////////////////////////////////////
OPERATION(MIN64)

TRIGGER
    SLongWord in1 = LONG(1);
    SLongWord in2 = LONG(2);
    SLongWord in3 = (in1 < in2) ? in1 : in2;
    IO(3) = in3;
END_TRIGGER;

END_OPERATION(MIN64)

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
// MAX64 - long integer maximum
//////////////////////////////////////////////////////////////////////////////
OPERATION(MAX64)

TRIGGER
    SLongWord in1 = LONG(1);
    SLongWord in2 = LONG(2);
    IO(3) = (in1 < in2) ? in2 : in1;
END_TRIGGER;

END_OPERATION(MAX64)

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
// MINU64 - integer minimum, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MINU64)

TRIGGER
    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);

    if (in1 < in2) {
        IO(3) = in1;
    } else {
        IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MINU64)

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
// MAXU64 - integer maximum, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MAXU64)

TRIGGER
    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);

    if (in1 > in2) {
        IO(3) = in1;
    } else {
        IO(3) = in2;
    }
END_TRIGGER;

END_OPERATION(MAXU64)

//////////////////////////////////////////////////////////////////////////////
// SXBW - sign extend from 1 bit to 32 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXBW)

TRIGGER
    IO(2) = SIGN_EXTEND(UINT(1), 1);
END_TRIGGER;

END_OPERATION(SXBW)

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
// SXB64 - sign extend from 1 bits to 64 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXB64)

TRIGGER
    IO(2) = SIGN_EXTEND(ULONG(1), 1);
END_TRIGGER;

END_OPERATION(SXB64)

//////////////////////////////////////////////////////////////////////////////
// SXQ64 - sign extend from 8 bits to 64 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXQ64)

TRIGGER
    IO(2) = SIGN_EXTEND(ULONG(1), 8);
END_TRIGGER;

END_OPERATION(SXQ64)

//////////////////////////////////////////////////////////////////////////////
// SXH64 - sign extend from 16 bits to 64 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXH64)

TRIGGER
    IO(2) = SIGN_EXTEND(ULONG(1), 16);
END_TRIGGER;

END_OPERATION(SXH64)

//////////////////////////////////////////////////////////////////////////////
// SXW64 - sign extend from 32 bits to 64 bits
//////////////////////////////////////////////////////////////////////////////
OPERATION(SXW64)

TRIGGER
    IO(2) = SIGN_EXTEND(ULONG(1), 32);
END_TRIGGER;

END_OPERATION(SXW64)

//////////////////////////////////////////////////////////////////////////////
// TRUNCWH - truncate 32 bit int to 16 bit int
//////////////////////////////////////////////////////////////////////////////
OPERATION(TRUNCWH)

TRIGGER
    IO(2) = ZERO_EXTEND(UINT(1), 16);
END_TRIGGER;

END_OPERATION(TRUNCWH)

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
// NEG64  - arithmetic negation
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEG64)

TRIGGER
    SLongWord in1 = LONG(1);
    in1 = -in1;
    IO(2) = in1;
END_TRIGGER;

END_OPERATION(NEG64)

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
// NOT64  - bitwise negation
//////////////////////////////////////////////////////////////////////////////
OPERATION(NOT64)

TRIGGER
    SLongWord in1 = static_cast<SLongWord>(ULONG(1));
    in1 = ~in1;
    IO(2) = static_cast<SLongWord>(in1);
END_TRIGGER;

END_OPERATION(NOT64)

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
// MUL64 - long integer multiply
//////////////////////////////////////////////////////////////////////////////
OPERATION(MUL64)

TRIGGER
    IO(3) = ULONG(1)*ULONG(2);
END_TRIGGER;

END_OPERATION(MUL64)

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
// DIV64 - long integer divide
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIV64)

TRIGGER
    if (ULONG(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    IO(3) = LONG(1) / LONG(2);
END_TRIGGER;

END_OPERATION(DIV64)

//////////////////////////////////////////////////////////////////////////////
// DIVU - long integer divide, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(DIVU64)

TRIGGER
    if (ULONG(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    IO(3) = ULONG(1) / ULONG(2);
END_TRIGGER;

END_OPERATION(DIVU64)

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
// MOD64 - long integer modulo
//////////////////////////////////////////////////////////////////////////////
OPERATION(MOD64)

TRIGGER
    if (LONG(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    SLongWord in1 = static_cast<SLongWord>(LONG(1));
    SLongWord in2 = static_cast<SLongWord>(LONG(2));
    SLongWord out1 = in1 % in2;
    IO(3) = static_cast<SLongWord>(out1);
END_TRIGGER;

END_OPERATION(MOD64)

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
// MODU64 - long integer modulo, unsigned
//////////////////////////////////////////////////////////////////////////////
OPERATION(MODU64)

TRIGGER
    if (ULONG(2) == 0)
         RUNTIME_ERROR("Divide by zero.")

    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);
    ULongWord out1 = in1 % in2;

    IO(3) = out1;
END_TRIGGER;

END_OPERATION(MODU64)


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
// CHI  - convert floating-point to integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CHI)

TRIGGER
    HalfFloatWord in = HFLT(1);
    FloatWord inf = FloatWord(in);
    SIntWord out = static_cast<SIntWord>(inf);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CHI)

//////////////////////////////////////////////////////////////////////////////
// CHIU  - convert floating-point to unsigned integer
//////////////////////////////////////////////////////////////////////////////
OPERATION(CHIU)

TRIGGER
    HalfFloatWord in = HFLT(1);
    FloatWord inf = FloatWord(in);
    UIntWord out = static_cast<UIntWord>(inf);
    IO(2) = out;
END_TRIGGER;

END_OPERATION(CHIU)

//////////////////////////////////////////////////////////////////////////////
// CIH - convert integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CIH)

TRIGGER
    SIntWord in = INT(1);
    FloatWord inf = static_cast<FloatWord>(in);
    IO(2) = HalfFloatWord(inf);
END_TRIGGER;

END_OPERATION(CIH)

//////////////////////////////////////////////////////////////////////////////
// CIHU - convert unsigned integer to floating-point (i2f)
//////////////////////////////////////////////////////////////////////////////
// NOTE: can't be tested whether the output really is float...
OPERATION(CIHU)

TRIGGER
    UIntWord in = UINT(1);
    FloatWord inf = static_cast<FloatWord>(in);
    IO(2) = HalfFloatWord(inf);
END_TRIGGER;

END_OPERATION(CIHU)

//////////////////////////////////////////////////////////////////////////////
// INVSQRTH - half-float inverse square root
//////////////////////////////////////////////////////////////////////////////
OPERATION(INVSQRTH)

TRIGGER
    HalfFloatWord in = HFLT(1);
    FloatWord infl(in);
    FloatWord result(1 / sqrt(infl));
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
    in2 = in2 % 32;
    IO(3) = (in1 << in2) | (in1 >> (32 - in2 ));
END_TRIGGER;

END_OPERATION(ROTL)


//////////////////////////////////////////////////////////////////////////////
// ROTL64 - rotate left
//////////////////////////////////////////////////////////////////////////////

OPERATION(ROTL64)

TRIGGER
    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);
    in2 = in2 % 64;
    IO(3) = (in1 << in2) | (in1 >> (64 - in2 ));
END_TRIGGER;

END_OPERATION(ROTL64)

//////////////////////////////////////////////////////////////////////////////
// ROTR - rotate right
//////////////////////////////////////////////////////////////////////////////

OPERATION(ROTR)

TRIGGER
    UIntWord in1 = ULONG(1);
    UIntWord in2 = ULONG(2);
    in2 = in2 % 32;
    IO(3) = (in1 >> in2) | (in1 << (32 - in2));
END_TRIGGER;

END_OPERATION(ROTR)

//////////////////////////////////////////////////////////////////////////////
// ROTR64 - rotate right
//////////////////////////////////////////////////////////////////////////////

OPERATION(ROTR64)

TRIGGER
    ULongWord in1 = ULONG(1);
    ULongWord in2 = ULONG(2);
    in2 = in2 % 64;
    IO(3) = (in1 >> in2) | (in1 << (64 - in2));
END_TRIGGER;

END_OPERATION(ROTR64)

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
// ABS - absolute value
//////////////////////////////////////////////////////////////////////////////

OPERATION(ABS64)

TRIGGER
    SLongWord temp = LONG(1);
    if (temp < 0) {
        temp = -temp;
    }
    IO(2) = temp;
END_TRIGGER;

END_OPERATION(ABS64)

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
// LDQU - load a byte from memory and zero extend to the output port width
//////////////////////////////////////////////////////////////////////////////

OPERATION(LDQU)

TRIGGER
    ULongWord data;
    MEMORY.readBE(UINT(1), 1, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE);
END_TRIGGER;

END_OPERATION(LDQU)

//////////////////////////////////////////////////////////////////////////////
// LDHU - load a 16-bit big endian word from memory with zero extension
//////////////////////////////////////////////////////////////////////////////

OPERATION(LDHU)

TRIGGER
    if (UINT(1) % 2 != 0)
	RUNTIME_ERROR_WITH_INT(
	    "Memory access alignment error, address: ", UINT(1));
    ULongWord data;
    MEMORY.readBE(UINT(1), 2, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LDHU)

//////////////////////////////////////////////////////////////////////////////
// LDU16 - load a 16-bit little endian word from memory with zero extension
//////////////////////////////////////////////////////////////////////////////

OPERATION(LDU16)

TRIGGER
    if (UINT(1) % 2 != 0)
    RUNTIME_ERROR_WITH_INT(
        "Memory access alignment error, address: ", UINT(1));
    ULongWord data;
    MEMORY.readLE(ULONG(1), 2, data);
    // See the comment in ST32.
    IO(2) = ZERO_EXTEND(data, MAU_SIZE*2);
END_TRIGGER;

END_OPERATION(LDU16)

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
IO(2) = static_cast<unsigned int>(CYCLE_COUNT);
END_TRIGGER;
END_OPERATION(ECC)

//////////////////////////////////////////////////////////////////////////////
// LCC - (Global) Lock cycle count
//////////////////////////////////////////////////////////////////////////////
OPERATION(LCC)
TRIGGER
// Simulator does not model global lock, return 0
IO(2) = 0;
END_TRIGGER;
END_OPERATION(LCC)

//////////////////////////////////////////////////////////////////////////////
// SELECT - select from two values
//////////////////////////////////////////////////////////////////////////////
OPERATION(SELECT)
TRIGGER
if (UINT(3) & 1) {
    IO(4) = IO(1);
} else {
    IO(4) = IO(2);
}
//IO(4) = ((IO(3)) & 1) ? IO(1) : IO(2);
END_TRIGGER;
END_OPERATION(SELECT)

//////////////////////////////////////////////////////////////////////////////
// SELECT64 - select from two values
//////////////////////////////////////////////////////////////////////////////
OPERATION(SELECT64)
TRIGGER
if (ULONG(3) & 1) {
    IO(4) = IO(1);
} else {
    IO(4) = IO(2);
}
//IO(4) = ((IO(3)) & 1) ? IO(1) : IO(2);
END_TRIGGER;
END_OPERATION(SELECT64)

////////////////////////////////////////////////////////////////////////////////
// CHS - convert half to short
////////////////////////////////////////////////////////////////////////////////
OPERATION(CHS)
TRIGGER
HalfFloatWord in = HFLT(1);
int tmp = SIntWord(in);
if (tmp < -32768) {
    tmp = -32768;
} else if (tmp > 32767) {
    tmp = 32767;
}
IO(2) = tmp;
END_TRIGGER;
END_OPERATION(CHS)

////////////////////////////////////////////////////////////////////////////////
// CHSU - convert half to unsigned short
////////////////////////////////////////////////////////////////////////////////
OPERATION(CHSU)
TRIGGER
HalfFloatWord in = HFLT(1);
int tmp = SIntWord(in);
if (tmp < 0) {
    tmp = 0;
}
IO(2) = tmp;
END_TRIGGER;
END_OPERATION(CHSU)

////////////////////////////////////////////////////////////////////////////////
// CSH - convert short to half
////////////////////////////////////////////////////////////////////////////////
OPERATION(CSH)
TRIGGER

SIntWord in = INT(1);
if (in & 0x8000) {
    in |= 0xFFFF0000;
} else {
    in &= 0x0000FFFF;
}
//HalfFloatWord tmp(FloatWord(in));
IO(2) = HalfFloatWord(FloatWord(in));
END_TRIGGER;
END_OPERATION(CSH)

////////////////////////////////////////////////////////////////////////////////
// CSHU - convert short to half
////////////////////////////////////////////////////////////////////////////////
OPERATION(CSHU)
TRIGGER

UIntWord in = (UINT(1));
UIntWord tmp1 = in & 0xFFFF;
IO(2) = HalfFloatWord(FloatWord(tmp1));
END_TRIGGER;
END_OPERATION(CSHU)

#include <signal.h>

void dummy(int) {
}

////////////////////////////////////////////////////////////////////////////////
// SLEEP - lock the core until an external signal is asserted
//
// The simulation behavior models this by waiting for the POSIX signal SIGUSR1.
//
////////////////////////////////////////////////////////////////////////////////
OPERATION(SLEEP)
TRIGGER

struct sigaction sa;
sa.sa_handler = dummy;
sa.sa_flags = SA_RESETHAND;
sigaction(SIGUSR1, &sa, NULL);

sigset_t sigs[1];
sigaddset(sigs, SIGUSR1);
int sig;

OUTPUT_STREAM << "[going to sleep...]" << std::endl;

sigwait(sigs, &sig);

OUTPUT_STREAM << "[...woke up]" << std::endl;

END_TRIGGER;
END_OPERATION(SLEEP)

