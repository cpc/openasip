/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file PreBypassBasicBlockScheduler.hh
 *
 * Declaration of PreBypassBasicBlockScheduler class.
 *
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#ifndef TTA_PREBYPASS_BB_SCHEDULER_HH
#define TTA_PREBYPASS_BB_SCHEDULER_HH

#include "BasicBlockScheduler.hh"
#include "CopyingDelaySlotFiller.hh"

/**
 * A class that implements the functionality of a basic block scheduler for
 * sequential input that has already been software bypassed.
 *
 * Uses LLVM MachineInstruction info for assiting in building the DDGs.
 */
class PreBypassBasicBlockScheduler : public ProgramPass, ProcedurePass, 
    ControlFlowGraphPass, BasicBlockScheduler {
public:
    PreBypassBasicBlockScheduler(
        InterPassData& data, DataDependenceGraphBuilder& ddgBuilder) : 
        ProgramPass(data),
        ProcedurePass(data),
        ControlFlowGraphPass(data),
        BasicBlockScheduler(data, NULL, new CopyingDelaySlotFiller()),
        ddgBuilder_(&ddgBuilder) {}    

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual void handleProgram(
        TTAProgram::Program& program,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual std::string shortDescription() const;

    virtual DataDependenceGraphBuilder& ddgBuilder() {
        return *ddgBuilder_;
    }
private:
    DataDependenceGraphBuilder* ddgBuilder_;
};

#endif

