/**
 * @file TriggeringInputPortState.cc
 *
 * Definition of TriggeringInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "TriggeringInputPortState.hh"
#include "SimValue.hh"
#include "FUState.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the port.
 * @param width Width of the port.
 */
TriggeringInputPortState::TriggeringInputPortState(
    FUState& parent, 
    int width) : 
    InputPortState(parent, width) {
}

/**
 * Constructor for port which uses a shared register.
 *
 * @param parent Parent of the port.
 * @param name Name of the port.
 * @param registerToUse The shared register storage.
 */
TriggeringInputPortState::TriggeringInputPortState(
    FUState& parent, 
    SimValue& registerToUse) :
    InputPortState(parent, registerToUse) {
}

/**
 * Destructor.
 */
TriggeringInputPortState::~TriggeringInputPortState() {
}

/**
 * Sets the value of the port and triggers an operation.
 *
 * @param value Value to be set.
 */
void
TriggeringInputPortState::setValue(const SimValue& value) {
    parent_->setTriggered();
    RegisterState::setValue(value);
}

