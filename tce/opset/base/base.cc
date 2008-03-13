/**
 * @file base.cc
 *
 * Base operation definitions.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 * @author Ari Mets‰halme 2004 (ari.metsahalme@tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen@tut.fi)
 */

#include <iostream>

#include "OSAL.hh"
#include "Application.hh"

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
        RUNTIME_ERROR("Memory access alignment error.")

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
        RUNTIME_ERROR("Memory access alignment error.")
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
        RUNTIME_ERROR("Memory access alignment error.");

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
        RUNTIME_ERROR("Memory access alignment error.");

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
        RUNTIME_ERROR("Memory access alignment error.");
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
        RUNTIME_ERROR("Memory access alignment error.");

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
// EQF - floating-point compare equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(EQF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) == FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) == DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(EQF)

//////////////////////////////////////////////////////////////////////////////
// NEF - floating-point compare not equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(NEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) != FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) != DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(NEF)

//////////////////////////////////////////////////////////////////////////////
// GTF - floating-point compare greater
//////////////////////////////////////////////////////////////////////////////
OPERATION(GTF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) > FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) > DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GTF)

//////////////////////////////////////////////////////////////////////////////
// GEF - floating-point compare greater or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(GEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) >= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) >= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(GEF)

//////////////////////////////////////////////////////////////////////////////
// LTF - floating-point compare lower
//////////////////////////////////////////////////////////////////////////////
OPERATION(LTF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) < FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) < DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LTF)

//////////////////////////////////////////////////////////////////////////////
// LEF - floating-point compare lower or equal
//////////////////////////////////////////////////////////////////////////////
OPERATION(LEF)

TRIGGER
    if (BWIDTH(1) == FLT_WORD_SIZE && BWIDTH(2) == FLT_WORD_SIZE) {
        IO(3) = (FLT(1) <= FLT(2)) ? 1 : 0;
    } else if (BWIDTH(1) == DBL_WORD_SIZE && BWIDTH(2) == DBL_WORD_SIZE) {
        IO(3) = (DBL(1) <= DBL(2)) ? 1 : 0;
    } else {
        abortWithError("bit widths of operands erronous");
    }
END_TRIGGER;

END_OPERATION(LEF)

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
        RUNTIME_ERROR("Memory access alignment error.")

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
