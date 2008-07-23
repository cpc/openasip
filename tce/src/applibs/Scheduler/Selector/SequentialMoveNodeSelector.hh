/**
 * @file SequentialMoveNodeSelector.hh
 *
 * Declaration of SequentialModeNodeSelector class.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_MOVE_NODE_SELECTOR_HH
#define TTA_SEQUENTIAL_MOVE_NODE_SELECTOR_HH

#include <list>

#include "MoveNodeSelector.hh"
#include "BasicBlock.hh"



/**
 * Selects move nodes from a basic block and prioritizes move nodes on
 * the critical path of the data dependence graph.
 */
class SequentialMoveNodeSelector : public MoveNodeSelector {
public:
    SequentialMoveNodeSelector(BasicBlock& bb);

    virtual ~SequentialMoveNodeSelector();

    virtual MoveNodeGroup candidates();
    virtual void notifyScheduled(MoveNode& node);

    void mightBeReady(MoveNode& node);
private:
    void createMoveNodes(BasicBlock& bb);

    // list of movenodegroups
    std::list<MoveNodeGroup*> mngs_;

    // returns the movenodegroups in order.
    // thiskeep track where we are going.
    std::list<MoveNodeGroup*>::iterator mngIter_;

    // list of all programoperations.
    std::list<ProgramOperation*> programOperations_;
};

#endif
