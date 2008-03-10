/**
 * @file InputPortState.cc
 *
 * Definition of InputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "InputPortState.hh"
#include "SimValue.hh"
#include "FUState.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the port.
 * @param width The bit width of the port.
 */
InputPortState::InputPortState(
    FUState& parent, int width) : 
    PortState(parent, width) {
}

/**
 * Constructor for port which uses a shared register.
 *
 * @param parent Parent of the port.
 * @param registerToUse The shared register storage.
 */
InputPortState::InputPortState(
    FUState& parent, 
    SimValue& registerToUse) :
    PortState(parent, registerToUse) {
}

/**
 * Destructor.
 */
InputPortState::~InputPortState() {
}

