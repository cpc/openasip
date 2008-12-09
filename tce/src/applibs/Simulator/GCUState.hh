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
 * @file GCUState.hh
 *
 * Declaration of GCUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GCU_STATE_HH
#define TTA_GCU_STATE_HH

#include <vector>

#include "FUState.hh"
#include "SimValue.hh"
#include "SimulatorConstants.hh"

class GlobalLock;

/**
 * Models the global control unit state.
 */
class GCUState : public FUState {
public:
    GCUState(int latency, int nww, GlobalLock& lock);
    virtual ~GCUState();

    InstructionAddress& programCounter();
    SimValue& returnAddressRegister();

    void setProgramCounter(const InstructionAddress& value);
    void setReturnAddress(const InstructionAddress& value);
    void setReturnAddress();

    virtual void advanceClock();

protected:

    virtual OperationContext& context();

private:
    /// Copying not allowed.
    GCUState(const GCUState&);
    /// Assignment not allowed.
    GCUState& operator=(const GCUState&);
    /// Natural word width as minimum addressable units.
    int naturalWordWidth_;
    /// The return address register.
    SimValue returnAddressRegister_;
    /// Program counter.
    InstructionAddress programCounter_;
    /// In case an control flow operation is pending, this variable
    /// contains the program counter value that should be updated
    /// when the control stage counter goes down.
    InstructionAddress newProgramCounter_;
    /// The delay of the control flow operations to take place.
    int latency_;
    /// If this this is true, there's a control flow operation pending.
    bool operationPending_;
    /// This indicates how many cycles the operation has been pending,
    /// when this reaches the latency, the PC is updated with the
    /// new value.
    int operationPendingTime_;
    /// The operation context for this FU.
    OperationContext operationContext_;

};

#include "GCUState.icc"

#endif
