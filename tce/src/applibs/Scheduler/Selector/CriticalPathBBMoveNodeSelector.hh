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
