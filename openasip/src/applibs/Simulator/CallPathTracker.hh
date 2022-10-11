/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file CallPathTracker.hh
 *
 * @author Pekka Jääskeläinen 2010
 */

#ifndef TTA_CALL_PATH_TRACKER_HH
#define TTA_CALL_PATH_TRACKER_HH

#include <deque>

#include "Listener.hh"
#include "SimulatorConstants.hh"
#include "ProcedureTransferTracker.hh"

class SimulatorFrontend;

namespace TTAProgram {
    class Instruction;
}

/**
 * Tracks procedure transfers in the simulated program and stores
 * them in memory for quick viewing during simulation/debugging.
 *
 * There should be one instance of this tracker for each tracked core.
 */
class CallPathTracker : public ProcedureTransferTracker {
public:
    struct ProcedureTransfer {
        ClockCycleCount cycle;
        InstructionAddress address;
        InstructionAddress sourceAddress;
        bool isEntry;
    };

    typedef std::deque<ProcedureTransfer> ProcedureTransferQueue;

    CallPathTracker(
        SimulatorFrontend& subject,
        unsigned,
        int historyLength) : 
        ProcedureTransferTracker(subject), 
        historyLength_(historyLength) {}
    virtual ~CallPathTracker() {}

    virtual void addProcedureTransfer(
        ClockCycleCount cycle,
        InstructionAddress address,
        InstructionAddress sourceAddress,
        bool isEntry);

    const ProcedureTransferQueue& transfers() const { return transfers_; }

    const ProcedureTransferQueue backTrace() const;

private:
    /// max number of last calls/returns to store
    std::size_t historyLength_;
    /// the transfers are stored in a queue of which size is kept
    /// under historyLength_
    ProcedureTransferQueue transfers_;
};

#endif
