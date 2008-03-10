/**
 * @file StateLocator.cc
 *
 * Definition of StateLocator class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "StateLocator.hh"
#include "StateData.hh"
#include "MachinePart.hh"

using std::string;
using namespace TTAMachine;

/**
 * Constructor.
 */
StateLocator::StateLocator() {
}

/**
 * Destructor.
 */
StateLocator::~StateLocator() {
}

/**
 * Adds state to to StateLocator.
 *
 * @param momComponent Machine object model component.
 * @param state Machine state model state.
 */
void
StateLocator::addState(const MachinePart& momComponent, StateData& state) {
    StateContainer::iterator iter = states_.find(&momComponent);
    if (iter != states_.end()) {
        (*iter).second = &state;
    } else {
        states_[&momComponent] = &state;
    }
}

/**
 * Returns state corresponding to the given component.
 *
 * @param momComponent Component.
 * @return State corresponding to given component.
 * @exception IllegalParameters If state is not found.
 */
StateData&
StateLocator::state(const MachinePart& momComponent) const
    throw (IllegalParameters) {

    StateContainer::const_iterator iter = states_.find(&momComponent);
    if (iter == states_.end()) {
        string msg = "State corresponding to component not found";
        throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
    }

    return *((*iter).second);
}
