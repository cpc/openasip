/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file BUBasicBlockScheduler.hh
 *
 * Declaration of BUBasicBlockScheduler class.
 *
 * @author Vladimir Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BYPASSING_BU_BB_SCHEDULER_HH
#define TTA_BYPASSING_BU_BB_SCHEDULER_HH

#include "BUMoveNodeSelector.hh"
#include "DDGPass.hh"
#include "BasicBlockPass.hh"
#include "BasicBlockScheduler.hh"
#include "DataDependenceGraph.hh"

class BasicBlockNode;
class SimpleResourceManager;
class SoftwareBypasser;
class CopyingDelaySlotFiller;
class DataDependenceGraphBuilder;
class RegisterRenamer;
class MoveNode;
class MoveNodeGroup;
class LLVMTCECmdLineOptions;

namespace TTAMachine {
    class Unit;
    class Port;
}

/**
 * A class that implements the functionality of a bottom up basic block 
 * scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class BypassingBUBasicBlockScheduler :
    public BasicBlockScheduler {
public:
    BypassingBUBasicBlockScheduler(
        InterPassData& data, SoftwareBypasser* bypasser=NULL, 
        CopyingDelaySlotFiller* delaySlotFiller=NULL,
        RegisterRenamer* registerRenamer = NULL);
    virtual ~BypassingBUBasicBlockScheduler();

    virtual void handleDDG(
        DataDependenceGraph& ddg,
        SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

    virtual MoveNodeSelector* createSelector( 
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) {
        return new BUMoveNodeSelector(bb, machine);
    }

    using BasicBlockPass::ddgBuilder;

    void scheduleOperation(MoveNodeGroup& mng, MoveNodeSelector& selector);

private:

    void finalizeOperation(ProgramOperation& po, MoveNodeSelector& selector);

    bool scheduleOperation(ProgramOperation& po, int latestCycle);

    bool scheduleResults(ProgramOperation& po, int latestCycle);

    bool scheduleMoveUB(MoveNode& mn, int earlistCycle, int latestCycle);

    bool scheduleMoveBU(MoveNode& mn, int earlistCycle, int latestCycle);

    int bypassNode(MoveNode& node, int maxHopCount);

    std::pair<MoveNode*, int> findBypassSource(
        MoveNode& node, int maxHopCount);

    bool bypassAndScheduleNode(
        MoveNode& node, MoveNode* trigger, int latestCycle);

    bool bypassAndScheduleOperands(
        ProgramOperation& po, MoveNode* trigger, int latestCycle);

    bool scheduleOperandOrTrigger(
        MoveNode& operand, MoveNode* trigger, int latestCycle);

    void unscheduleResults(ProgramOperation& po);
    
    void unscheduleOperands(ProgramOperation& po);

    void undoBypass(MoveNode& mn);

    void undoBypassAndUnschedule(MoveNode& mn);

    void unscheduleOperation(ProgramOperation& po);

    std::pair<int,int> operandCycleLimits(MoveNode& mn, MoveNode* trigger);

    int lastOperandCycle(const ProgramOperation& po);

    MoveNode* findTrigger(ProgramOperation& po);

    MoveNode* findTriggerFromUnit(
        ProgramOperation& po, TTAMachine::Unit& unit);

    std::set<const TTAMachine::Port*> findPossibleDestinationPorts(
        MoveNode& node);

    std::set<const TTAMachine::Port*> findPossibleSourcePorts(
        MoveNode& node);

    void clearCaches();

    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> bypassSources_;

    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> removedBypassSources_;

    std::set<MoveNode*, MoveNode::Comparator> removedNodes_;

    std::set<MoveNode*, MoveNode::Comparator> pendingBypassSources_;

    std::set<MoveNode*, MoveNode::Comparator> scheduledMoves_;

    bool killDeadResults_;
    int endCycle_;
};

#endif
