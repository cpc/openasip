/*
    Copyright (c) 2002-2021 Eindhoven University of Technology

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
 * @file implicit_lsu.cc
 *
 * Implicit LSU operation definitions.
 *
 * @author Kanishkan Vadivel (k.vadivel-no.spam-tue.nl)
 * @author Barry de Bruin (e.d.bruin-no.spam-tue.nl)
 */

#include "OSAL.hh"
#include "OperationGlobals.hh"

#define RUNTIME_ERROR_WITH_LONG(MESSAGE, DATA) {\
       int len = strlen(MESSAGE) + 21;                \
       char *tmpBuf = static_cast<char*>(alloca(len));\
       snprintf(tmpBuf, len, "%s %ld", MESSAGE, (long)DATA);    \
       OperationGlobals::runtimeError(                \
           tmpBuf, __FILE__, __LINE__, parent_);      \
}

DEFINE_STATE(IMPLICIT_LSU_STATE)
    uint16_t loadStride;
    uint16_t storeStride;
    uint16_t loadAddr;
    uint16_t storeAddr;
INIT_STATE(IMPLICIT_LSU_STATE)
    loadAddr = 0;
    storeAddr = 0;
    loadStride = 0;
    storeStride = 0;
END_INIT_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// set LSU configuration register
//////////////////////////////////////////////////////////////////////////////
OPERATION_WITH_STATE(ILD_INIT, IMPLICIT_LSU_STATE)
TRIGGER
    STATE.loadAddr = UINT(1);
    STATE.loadStride = UINT(2);
END_TRIGGER;
END_OPERATION_WITH_STATE(ILD_INIT)

OPERATION_WITH_STATE(IST_INIT, IMPLICIT_LSU_STATE)
TRIGGER
    STATE.storeAddr = UINT(1);
    STATE.storeStride = UINT(2);
END_TRIGGER;
END_OPERATION_WITH_STATE(IST_INIT)

//////////////////////////////////////////////////////////////////////////////
// LD8 - load a 8-bit from memory with sign extension
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILD8, IMPLICIT_LSU_STATE)

TRIGGER
    // Usage: ildu8 out1
    ULongWord data;
    MEMORY.readLE(STATE.loadAddr, 1, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE);

    // Post-increment address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILD8)

//////////////////////////////////////////////////////////////////////////////
// LDU8 - load a 8-bit from memory with zero extension
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILDU8, IMPLICIT_LSU_STATE)

TRIGGER
    // Usage: ildu8 out1
    ULongWord data;
    MEMORY.readLE(STATE.loadAddr, 1, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE);

    // Post-increment address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILDU8)

//////////////////////////////////////////////////////////////////////////////
// ST8 - store a 8-bit word to memory
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(IST8, IMPLICIT_LSU_STATE)

TRIGGER
    // Usage: ist8 in1
    UIntWord data = UINT(1);
    MEMORY.writeLE(STATE.storeAddr, 1, data);

    // Post-increment address
    STATE.storeAddr += STATE.storeStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(IST8)


//////////////////////////////////////////////////////////////////////////////
// LD32 - load a 32-bit little endian word from memory with sign extension
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILD32, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.loadAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.loadAddr);
    // Usage: ild32 out1
    ULongWord data;
    MEMORY.readLE(STATE.loadAddr, 4, data);
    IO(2) = SIGN_EXTEND(data, MAU_SIZE*4);

    // Post-increment address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILD32)


//////////////////////////////////////////////////////////////////////////////
// LDU32 - load a 32-bit little endian word from memory with zero extension
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILDU32, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.loadAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.loadAddr);
    // Usage: ildu32 out1
    ULongWord data;
    MEMORY.readLE(STATE.loadAddr, 4, data);
    IO(2) = ZERO_EXTEND(data, MAU_SIZE*4);

    // Post-increment address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILDU32)


//////////////////////////////////////////////////////////////////////////////
// ST32 - store a 32-bit word to memory in little endian byte order
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(IST32, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.storeAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.storeAddr);
    // Usage: ist32 in1
    UIntWord data = UINT(1);
    MEMORY.writeLE(STATE.storeAddr, 4, data);

    // Post-increment address
    STATE.storeAddr += STATE.storeStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(IST32)


//////////////////////////////////////////////////////////////////////////////
// ILDU32_IST8 - load a 32-bit little endian word from memory with zero extension
// and store 8-bit data to memory
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILDU32_IST8, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.loadAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.loadAddr);
    // Usage: ildu_ist8 out1, in1
    UIntWord data = UINT(1);
    MEMORY.writeLE(STATE.storeAddr, 1, data);

    // Post-increment store address
    STATE.storeAddr += STATE.storeStride;

    ULongWord data2;
    MEMORY.readLE(STATE.loadAddr, 4, data2);
    IO(2) = ZERO_EXTEND(data2, MAU_SIZE*4);

    // Post-increment load address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILDU32_IST8)


///////////////////////////////////////////////////////////////////////////////////////
// ILD32_IST32 - load and store a 32-bit little endian word from memory with sign extension
///////////////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILD32_IST32, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.storeAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.storeAddr);
    if (STATE.loadAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.loadAddr);
    // Usage: ild_ist32 out1, in1
    UIntWord data = UINT(1);
    MEMORY.writeLE(STATE.storeAddr, 4, data);

    // Post-increment store address
    STATE.storeAddr += STATE.storeStride;

    ULongWord data2;
    MEMORY.readLE(STATE.loadAddr, 4, data2);
    IO(2) = SIGN_EXTEND(data2, MAU_SIZE*4);

    // Post-increment load address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILD32_IST32)


//////////////////////////////////////////////////////////////////////////////
// ILDU32_IST32 - load 32-bit little endian word from memory with zero extension
// and store 32-bit data to memory
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(ILDU32_IST32, IMPLICIT_LSU_STATE)

TRIGGER
    if (STATE.storeAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.storeAddr);
    if (STATE.loadAddr % 4 != 0)
        RUNTIME_ERROR_WITH_LONG(
                "Memory access alignment error, address: ", STATE.loadAddr);
    // Usage: ildu_ist32 out1, in1
    UIntWord data = UINT(1);
    MEMORY.writeLE(STATE.storeAddr, 4, data);

    // Post-increment address
    STATE.storeAddr += STATE.storeStride;

    ULongWord data2;
    MEMORY.readLE(STATE.loadAddr, 4, data2);
    IO(2) = ZERO_EXTEND(data2, MAU_SIZE*4);

    // Post-increment address
    STATE.loadAddr += STATE.loadStride;
END_TRIGGER;

END_OPERATION_WITH_STATE(ILDU32_IST32)
