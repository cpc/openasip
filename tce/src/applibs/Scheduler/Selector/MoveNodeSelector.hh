/**
 * @file MoveNodeSelector.hh
 *
 * Declaration of MoveNodeSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_SELECTOR_HH
#define TTA_MOVE_NODE_SELECTOR_HH

#include "HelperSchedulerModule.hh"
#include "MoveNodeGroup.hh"

/**
 * Move node selector is an abstraction to ready operation list in a 
 * list scheduler.
 *
 * MoveNodeSelectors are responsible in maintaining the ready list and updating
 * it according to scheduling progress.
 */
class MoveNodeSelector : public HelperSchedulerModule {
public:
    MoveNodeSelector();
    virtual ~MoveNodeSelector();
    
    virtual MoveNodeGroup candidates() = 0;
    virtual void notifyScheduled(MoveNode& node) = 0;
    virtual void mightBeReady(MoveNode& node) = 0;
};

#endif
