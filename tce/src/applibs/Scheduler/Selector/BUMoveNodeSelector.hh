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
 * @file BUMoveNodeSelector.hh
 *
 * Declaration of BUMoveNodeSelector class.
 *
 * @author Vladim’r Guzma 2011(vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BU_MOVE_NODE_SELECTOR_HH
#define TTA_BU_MOVE_NODE_SELECTOR_HH

#ifdef _GLIBCXX_DEBUG
/* If enabled, the priority list crashes with error 
   "elements in iterator range [__first, __last) do not form a heap
   with respect to the predicate __comp" when calling readyList_.pop() in 
   CriticalPathBBMoveNodeSelector::candidates().

   It's probably caused by a situation in which a node's priority changes
   during scheduling and the priority list does not notify the change before
   pop() is called and it detects a malformed heap (a parent node larger than
   a child node), or something.

   @todo This needs to be fixed at some point somehow as it might result in
         weird errors in some machines or undeterministic schedule.
*/
#undef _GLIBCXX_DEBUG
#endif

#include "MoveNodeSelector.hh"
#include "ReadyMoveNodeGroupList.hh"
#include "DataDependenceGraph.hh"

namespace TTAProgram {
    class BasicBlock;
}

namespace TTAMachine {
    class Machine;
}

/**
 * Selects move nodes from a basic block and prioritizes move nodes on
 * the critical path of the data dependence graph.
 * This version prioritiezed for bottom up scheduling.
 */
class BUMoveNodeSelector : public MoveNodeSelector {
public:
    BUMoveNodeSelector(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine);
    BUMoveNodeSelector(
        DataDependenceGraph& bigDDG, TTAProgram::BasicBlock& bb,
        const TTAMachine::Machine& machine);
    BUMoveNodeSelector(
        DataDependenceGraph& ddg, const TTAMachine::Machine &machine);
    virtual ~BUMoveNodeSelector();
    
    virtual MoveNodeGroup candidates();
    virtual void notifyScheduled(MoveNode& node);
    virtual void mightBeReady(MoveNode& node);
    virtual DataDependenceGraph& dataDependenceGraph();

    /// Initializes ready list from nodes that are ready.
    virtual void initializeReadylist();

    static void queueOperation(
        ProgramOperation& po,
        const DataDependenceGraph::NodeSet& nodes,
        DataDependenceGraph::NodeSet& queue);

private:
    virtual bool isReadyToBeScheduled(MoveNode& node) const;
    virtual bool isReadyToBeScheduled(MoveNodeGroup& nodes) const;
    /// The data dependence graph built from the basic block.
    DataDependenceGraph* ddg_;    
    /// The prioritized ready list.
    ReadyMoveNodeGroupListBU readyList_;
    // whether owns the DDG?
    bool ddgOwned_;    
};

#endif
