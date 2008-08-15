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
 * @file BasicBlockScheduler.hh
 *
 * Declaration of BasicBlockScheduler class.
 *
 * @author Pekka J��skel�inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BB_SCHEDULER_HH
#define TTA_BB_SCHEDULER_HH

#include "Program.hh"
#include "MoveNode.hh"
#include "MoveNodeGroup.hh"
#include "CriticalPathBBMoveNodeSelector.hh"
#include "FunctionUnit.hh"
#include "BasicBlockPass.hh"
#include "ControlFlowGraphPass.hh"
#include "ProcedurePass.hh"
#include "ProgramPass.hh"
#include "DDGPass.hh"
#include "DataDependenceGraph.hh"

class BasicBlockNode;
class SimpleResourceManager;
class SoftwareBypasser;
class CopyingDelaySlotFiller;

/**
 * A class that implements the functionality of a basic block scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class BasicBlockScheduler :
    public BasicBlockPass, public ControlFlowGraphPass, public ProcedurePass,
    public ProgramPass, public DDGPass {
public:
    BasicBlockScheduler(
        InterPassData& data, SoftwareBypasser* bypasser=NULL, 
        CopyingDelaySlotFiller* delaySlotFiller=NULL);
    virtual ~BasicBlockScheduler();

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

    virtual void handleDDG(
        DataDependenceGraph& ddg,
        SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

protected:
    virtual DataDependenceGraph* createDDGFromBB(BasicBlock& bb);
    virtual void deleteRM(SimpleResourceManager* rm, BasicBlock& bb);

private:
    void scheduleOperation(MoveNodeGroup& moves)
        throw (Exception);

    int scheduleOperandWrites(int cycle, MoveNodeGroup& moves)
        throw (Exception);

    bool scheduleResultReads(MoveNodeGroup& moves)
        throw (Exception);

    void scheduleMove(MoveNode& move, int earliestCycle)
        throw (Exception);

    void scheduleInputOperandTempMoves(MoveNode& operandMove)
        throw (Exception);

    void unschedule(MoveNode& moveNode);

    void unscheduleInputOperandTempMoves(MoveNode& operandMove);

    void scheduleResultReadTempMoves(MoveNode& resultMove)
        throw (Exception);

    void unscheduleResultReadTempMoves(MoveNode& resultMove);

    MoveNode* succeedingTempMove(MoveNode& current);
    
    /// The target machine we are scheduling the program against.
    const TTAMachine::Machine* targetMachine_;
    /// DDG of the currently scheduled BB.
    DataDependenceGraph* ddg_;
    /// whole-procedure DDG.
    DataDependenceGraph* bigDDG_;
    /// Resource Manager of the currently scheduled BB.
    SimpleResourceManager* rm_;
    /// Stores the MoveNodes that were scheduled as temp moves during
    /// scheduling of the operand move.
    std::map<const MoveNode*, DataDependenceGraph::NodeSet > 
    scheduledTempMoves_;
    /// The software bypasser to use to bypass registers when possible.
    SoftwareBypasser* softwareBypasser_;

    CopyingDelaySlotFiller* delaySlotFiller_;

    int bypassedCount_;
    int deadResults_;
};

#endif
