/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file GCUState.hh
 *
 * Declaration of GCUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
