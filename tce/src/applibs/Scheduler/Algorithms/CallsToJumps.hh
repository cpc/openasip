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
#ifndef TTA_CALLS_TO_JUMPS_HH
#define TTA_CALLS_TO_JUMPS_HH

#include "ControlFlowGraphPass.hh"

/**
 * Pass that converts all CALL operation calls to a JUMP that succeeds a 
 * regular move to RA that stores the address of the instruction following
 * the original CALL.
 *
 * This pass should be called for an unscheduled CFG before DDG construction.
 */
class CallsToJumps : public ControlFlowGraphPass {
public:
    CallsToJumps(InterPassData& data) : ControlFlowGraphPass(data) {}
    virtual ~CallsToJumps() {}

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine);

    std::string shortDescription() const {
        return "Converts CALL operations to JUMPs and explicit RA save moves.";
    }
};

#endif
