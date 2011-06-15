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
 * @file BasicBlockScheduler.hh
 *
 * Declaration of BasicBlockScheduler class.
 *
 * @author Pekka J��skel�inen 2006,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @author Fabio Garzia 2010 (fabio.garzia-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BB_SCHEDULER_HH
#define TTA_BB_SCHEDULER_HH

#include "Program.hh"
#include "MoveNode.hh"
#include "MoveNodeGroup.hh"
#include "CriticalPathBBMoveNodeSelector.hh"
#include "FunctionUnit.hh"
#include "DDGPass.hh"
#include "BasicBlockPass.hh"
#include "BBSchedulerController.hh"
#include "DataDependenceGraph.hh"
#include "LLVMTCECmdLineOptions.hh"

class BasicBlockNode;
class SimpleResourceManager;
class SoftwareBypasser;
class CopyingDelaySlotFiller;
class DataDependenceGraphBuilder;

/**
 * A class that implements the functionality of a basic block scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class BasicBlockScheduler :
    public BBSchedulerController, public DDGPass {
public:
    BasicBlockScheduler(
        InterPassData& data, SoftwareBypasser* bypasser=NULL, 
        CopyingDelaySlotFiller* delaySlotFiller=NULL);
    virtual ~BasicBlockScheduler();

    virtual void handleDDG(
        DataDependenceGraph& ddg,
        SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

    virtual MoveNodeSelector* createSelector( 
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) {
        return new CriticalPathBBMoveNodeSelector(bb, machine);
    }


    using BasicBlockPass::ddgBuilder;

private:
    void scheduleRRMove(MoveNode& moveNode)
        throw (Exception);

    void scheduleOperation(MoveNodeGroup& moves)
        throw (Exception);

    int scheduleOperandWrites(int& cycle, MoveNodeGroup& moves)
        throw (Exception);

    bool scheduleResultReads(MoveNodeGroup& moves)
        throw (Exception);

    void scheduleMove(MoveNode& move, int earliestCycle)
        throw (Exception);

    void scheduleRRTempMoves(
        MoveNode& regToRegMove, MoveNode& firstMove, int lastUse)
        throw (Exception);

    void scheduleInputOperandTempMoves(
        MoveNode& operandMove, MoveNode& operandWrite)
        throw (Exception);

    void unschedule(MoveNode& moveNode);

    void unscheduleInputOperandTempMoves(MoveNode& operandMove);

    void scheduleResultReadTempMoves(
        MoveNode& resultMove, MoveNode& resultRead, int lastUse)
        throw (Exception);

    void unscheduleResultReadTempMoves(MoveNode& resultMove);

    void notifyScheduled(
        MoveNodeGroup& moves, MoveNodeSelector& selector);

    void ddgSnapshot(
        DataDependenceGraph& ddg, 
        const std::string& name,
        DataDependenceGraph::DumpFileFormat format,
        bool final,
        bool resetCounter = false) const;

    MoveNode* succeedingTempMove(MoveNode& current);
    
    /// The target machine we are scheduling the program against.
    const TTAMachine::Machine* targetMachine_;
    /// DDG of the currently scheduled BB.
    DataDependenceGraph* ddg_;
    /// Resource Manager of the currently scheduled BB.
    SimpleResourceManager* rm_;
    /// Stores the MoveNodes that were scheduled as temp moves during
    /// scheduling of the operand move.
    std::map<const MoveNode*, DataDependenceGraph::NodeSet > 
    scheduledTempMoves_;
    /// The software bypasser to use to bypass registers when possible.
    SoftwareBypasser* softwareBypasser_;

    int bypassedCount_;
    int deadResults_;

    LLVMTCECmdLineOptions* options_;
};

#endif
