/*
    Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file SignalTypes.hh
 *
 * Declaration of SignalTypes class.
 *
 * Created on: 25.5.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SIGNALTYPES_HH
#define SIGNALTYPES_HH

namespace ProGe {
/**
 * List of different types of signal. The types are also used as numerical IDs
 * for connecting similiar NetlistPorts together.
 */
enum class SignalType {
    /// Signal does not have specified usage.
    UNDEFINED = 0,
    /// Signal is left to unconnected.
    OPEN,
    /// Signal is internally defined custom signal.
    INTERNAL,
    /// Signal is externally defined custom signal.
    USERDEFINED,
    /// Clock signal.
    CLOCK,
    /// Reset signal.
    RESET,
    /// Signal holds address.
    ADDRESS,
    /// Signal contains TTA (decompressed) instruction.
    INSTRUCTIONWORD,
    /// Signal is TTA instruction block block containing (compressed)
    /// instruction.
    FETCHBLOCK,
    /// Signal to make read request.
    READ_REQUEST,
    /// Signal to make burst read request.
    BURST_READ_REQUEST,
    /// Signal to tell that requested data can be read.
    READ_REQUEST_READY,
    /// Signal to tell that part of burst-requested data can be read.
    BURST_READ_REQUEST_READY,
    /// Signal to make write request.
    WRITE_REQUEST,
    /// Signal to make either read or write request.
    READ_WRITE_REQUEST,
    /// Signal to choose mode for READ_WRITE_REQUEST or similar.
    WRITEMODE,
    /// Signal to enable or activate a device.
    ENABLE,
    /// Signal telling that source device is not ready to handle a request.
    BUSY,
    /// Signal to stopping destination device.
    STALL = BUSY,
    /// TTA Global lock request signal.
    GLOCK_REQUEST,
    /// TTA Global lock signal.
    GLOCK,
    /// Cache Invalidation signal.
    INVALIDATE_CACHE,
    READ_DATA,
    WRITE_DATA,
    WRITE_BITMASK,
    /// Signal that tell if core is halted.
    CORE_HALT_STATUS,

    /// Signal types for memory interface with separate
    /// valid/ready in address/data
    AVALID,
    AREADY,
    AADDR,
    AWREN,
    ASTRB,
    RVALID,
    RREADY,
    RDATA,
    ADATA

    //todo consider these:
    //DATA
    //DATA0..DATAn < for vectors, 0..n to point vector element
    //VECTORDATA < alternative for vector, SignalID to point vector element
    //INSTRUCTION_ADDRESS
    //DATA_ADDRESS
    //FU_OPCODE
    //FU_TRIGGER or FU_TRIGGER_LOAD = WRITE_REQUEST = LOAD
    //FU_DATA < Either for trigger, operand or result data. Determined by
    //          associated SignalGroupType
};

enum class ActiveState {
    HIGH = 0,
    LOW
};
}

#endif /* SIGNALTYPES_HH */
