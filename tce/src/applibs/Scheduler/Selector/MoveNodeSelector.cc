/**
 * @file MoveNodeSelector.cc
 *
 * Implementation of MoveNodeSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "MoveNodeSelector.hh"
#include "Application.hh"

/**
 * Constructor.
 */
MoveNodeSelector::MoveNodeSelector() {
}

/**
 * Destructor.
 */
MoveNodeSelector::~MoveNodeSelector() {
}

/**
 * Returns a group of move nodes which should be scheduled next.
 *
 * By default returns an empty group. A non-empty group is guaranteed to
 * contain at least one unscheduled move, but not all of them necessarily
 * are.
 *
 * @return Move node group.
 */
MoveNodeGroup
MoveNodeSelector::candidates() {
    return MoveNodeGroup();
}

/**
 * This should be called by the client as soon as a MoveNode is scheduled
 * in order to update the internal state of the selector.
 *
 * @param node The scheduled MoveNode.
 */
void
MoveNodeSelector::notifyScheduled(MoveNode& node) {
    assert(&node && "Should never be called. Here just to avoid warning.");
}

/** 
 * This should be called by the client when some incoming dependence to
 * a moveNode is removed.
 *
 * @param node Node which has lost some incoming dependence
 */
void
MoveNodeSelector::mightBeReady(MoveNode& node) {
    assert(&node && "Should never be called. Here just to avoid warning.");
}
