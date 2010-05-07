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
 * @file SequentialScheduler.hh
 *
 * Declaration of SequentialScheduler class.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_SCHEDULER_HH
#define TTA_SEQUENTIAL_SCHEDULER_HH

#include <vector>

#include "Program.hh"
#include "MoveNode.hh"
#include "MoveNodeGroup.hh"
#include "SequentialMoveNodeSelector.hh"
#include "FunctionUnit.hh"
#include "BasicBlockPass.hh"
#include "ControlFlowGraphPass.hh"
#include "ProcedurePass.hh"
#include "ProgramPass.hh"
#include "RegisterCopyAdder.hh"

class BasicBlockNode;
class SimpleResourceManager;

/**
 * A class that implements the functionality of a basic block scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class SequentialScheduler :
    public BasicBlockPass, public ControlFlowGraphPass, public ProcedurePass,
    public ProgramPass {
public:
    SequentialScheduler(
        InterPassData& data);

    virtual ~SequentialScheduler();

    virtual void handleBasicBlock(
        BasicBlock& bb, const TTAMachine::Machine& targetMachine)
        throw (Exception);

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
    virtual std::string longDescription() const;

private:
    int scheduleOperation(MoveNodeGroup& moves, int earliestCycle)
        throw (Exception);

    int scheduleOperandWrites(
        int cycle, MoveNodeGroup& moves,
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    int scheduleResultReads(
        int triggerCycle, MoveNodeGroup& moves,
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    int scheduleMove(int earliestCycle, MoveNode& move)
        throw (Exception);

    int scheduleInputOperandTempMoves(
        int cycle, MoveNode& operandMove, 
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    void unscheduleInputOperandTempMoves(
	MoveNode& operandMove, 
	RegisterCopyAdder::AddedRegisterCopies& regCopies);

    int scheduleResultTempMoves(
        int cycle, MoveNode& resultMove, 
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);
        
    void createBasicBlocks(
        TTAProgram::Procedure& cs, 
        std::vector<BasicBlock*>& basicBlocks,
        std::vector<int>& bbAddresses);

    void copyBasicBlocksToProcedure(
        TTAProgram::Procedure& cs, 
        std::vector<BasicBlock*>& basicBlocks,
        std::vector<int>& bbAddresses);

    void unschedule(MoveNode& moveNode);

    /// The target machine we are scheduling the program against.
    const TTAMachine::Machine* targetMachine_;
    /// Resource Manager of the currently scheduled BB.
    SimpleResourceManager* rm_;
    /// Stores the MoveNodes that were scheduled as temp moves during
    /// scheduling of the operand move.

};

#endif
