/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file ScheduleEstimator.hh
 *
 * Declaration of ScheduleEstimator class.
 *
 * This class tries to estimate the worst possible cycle count
 * for a basic block.
 *
 * @author Heikki Kultala 2019 (hkultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "ControlFlowGraphPass.hh"

namespace TTAProgram {
    class BasicBlock;
}

namespace TTAMachine {
    class Machine;
}

class ScheduleEstimator : public ControlFlowGraphPass {

public:
    ScheduleEstimator(InterPassData& data) : ControlFlowGraphPass(data) {}

    virtual void
    handleControlFlowGraph(
        ControlFlowGraph& cfg, const TTAMachine::Machine& mach) override;

    static int maximumSizeOfBB(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach);

    virtual std::string shortDescription() const { return "Estimates max schedule length"; }
};
