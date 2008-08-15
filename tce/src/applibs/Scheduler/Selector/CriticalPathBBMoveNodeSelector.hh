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
 * @file CriticalPathBBMoveNodeSelector.hh
 *
 * Declaration of CriticalPathBBMoveNodeSelector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CRITICAL_PATH_BB_MOVE_NODE_SELECTOR_HH
#define TTA_CRITICAL_PATH_BB_MOVE_NODE_SELECTOR_HH

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

#include "HelperSchedulerModule.hh"
#include "MoveNodeSelector.hh"
#include "BasicBlock.hh"
#include "ReadyMoveNodeGroupList.hh"

class DataDependenceGraph;

/**
 * Selects move nodes from a basic block and prioritizes move nodes on
 * the critical path of the data dependence graph.
 */
class CriticalPathBBMoveNodeSelector : public MoveNodeSelector {
public:
    CriticalPathBBMoveNodeSelector(
        BasicBlock& bb, const TTAMachine::Machine &machine);
    CriticalPathBBMoveNodeSelector(
        DataDependenceGraph& bigDDG, BasicBlock& bb,
        const TTAMachine::Machine &machine)
        throw (ModuleRunTimeError);
    CriticalPathBBMoveNodeSelector(
        DataDependenceGraph& ddg, const TTAMachine::Machine &machine);

    virtual ~CriticalPathBBMoveNodeSelector();

    virtual MoveNodeGroup candidates();
    virtual void notifyScheduled(MoveNode& node);

    virtual DataDependenceGraph& dataDependenceGraph();

    void mightBeReady(MoveNode& node);

private:
    bool isReadyToBeScheduled(MoveNode& node) const;

    /// Initializes ready list from nodes that are ready.
    void initializeReadylist();

    /// The data dependence graph built from the basic block.
    DataDependenceGraph* ddg_;
    /// The prioritized ready list.
    ReadyMoveNodeGroupList readyList_;
    // whether owns the DDG?
    bool ddgOwned_;
};

#endif
