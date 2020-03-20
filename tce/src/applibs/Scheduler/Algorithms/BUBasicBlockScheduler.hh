/*
    Copyright (c) 2002-2011 Tampere University.

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

#ifndef TTA_BU_BB_SCHEDULER_HH
#define TTA_BU_BB_SCHEDULER_HH

#include "BUMoveNodeSelector.hh"
#include "DDGPass.hh"
#include "BasicBlockPass.hh"
#include "BasicBlockScheduler.hh"

class BasicBlockNode;
class SimpleResourceManager;
class SoftwareBypasser;
class CopyingDelaySlotFiller;
class DataDependenceGraphBuilder;
class DataDependenceGraph;
class RegisterRenamer;
class MoveNode;
class MoveNodeGroup;
class LLVMTCECmdLineOptions;

/**
 * A class that implements the functionality of a bottom up basic block
 * scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class BUBasicBlockScheduler :
    public BasicBlockScheduler {
public:
    BUBasicBlockScheduler(
        InterPassData& data, SoftwareBypasser* bypasser=NULL,
        CopyingDelaySlotFiller* delaySlotFiller=NULL,
        RegisterRenamer* registerRenamer = NULL);
    virtual ~BUBasicBlockScheduler();

    virtual int handleDDG(
        DataDependenceGraph& ddg, SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine, bool testOnly) override;

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

    virtual MoveNodeSelector* createSelector(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) {
        return new BUMoveNodeSelector(bb, machine);
    }

    using BasicBlockPass::ddgBuilder;

protected:
    struct ltstr
    {
        bool operator()(const MoveNode* m1, const MoveNode* m2) const
        {
            return m1->cycle() < m2->cycle();
        }
    };
    
    typedef std::set<MoveNode*, ltstr> OrderedSet;

    void scheduleRRMove(MoveNode& moveNode);

    void scheduleOperation(MoveNodeGroup& moves, BUMoveNodeSelector& selector);

    bool scheduleOperandWrites(MoveNodeGroup& moves, int cycle);

    int scheduleResultReads(
        MoveNodeGroup& moves, int cycle, bool bypass = false,
        bool bypassLate = false);

    void scheduleMove(MoveNode& move, int cycle);

    void scheduleResultReadTempMoves(
        MoveNode& resultMove, MoveNode& resultRead, int lastUse);

    void scheduleInputOperandTempMoves(
        MoveNode& resultMove, MoveNode& resultRead);

    void scheduleRRTempMoves(
        MoveNode& regToRegMove, MoveNode& firstMove, int lastUse);

    bool scheduleOperand(MoveNode&, int cycle);

    MoveNode* precedingTempMove(MoveNode& current);

    std::pair<OrderedSet, int> findBypassDestinations(
        MoveNode& node, int maxHopCount);

    void undoBypass(
        MoveNode& node, MoveNode* single = NULL, int originalCycle = -1);
    
    bool bypassNode(MoveNode& node, int& maxResultCycle);
    
    void finalizeSchedule(MoveNode& node, BUMoveNodeSelector& selector);
    
    std::map<MoveNode*, std::vector<MoveNode*>, MoveNode::Comparator> 
        bypassDestinations_;
    std::map<MoveNode*, std::vector<int>, MoveNode::Comparator > 
        bypassDestinationsCycle_;
    unsigned int endCycle_;
    
    bool bypass_;
    bool dre_;
    int bypassDistance_;
};

#endif
