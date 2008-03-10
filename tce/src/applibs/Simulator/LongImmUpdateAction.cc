/**
 * @file LongImmUpdateAction.cc
 *
 * Definition of LongImmUpdateAction class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "LongImmUpdateAction.hh"
#include "SimValue.hh"
#include "LongImmediateRegisterState.hh"
#include "Application.hh"

/**
 * Constructor.
 *
 * @param value Value to be updated.
 * @param state Destination state.
 */
LongImmUpdateAction::LongImmUpdateAction(
    SimValue value, 
    LongImmediateRegisterState& state) : value_(value), state_(state) {
}

/**
 * Destructor.
 */
LongImmUpdateAction::~LongImmUpdateAction() {
}

/**
 * Updates the value of the destination register state.
 */
void
LongImmUpdateAction::execute() {
    state_.setValue(value_);
}
