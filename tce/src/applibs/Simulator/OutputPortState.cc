/**
 * @file OutputPortState.cc
 *
 * Definition of OutputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "OutputPortState.hh"
#include "SimValue.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the port.
 * @param width The width of the port (needed to mask out exessive bits).
 */
OutputPortState::OutputPortState(
    FUState& parent, 
    std::size_t width) :
    PortState(parent, width) {
}

/**
 * Constructor for port which uses a shared register.
 *
 * @param parent Parent of the port.
 * @param name Name of the port.
 * @param registerToUse The shared register storage.
 */
OutputPortState::OutputPortState(
    FUState& parent, 
    SimValue& registerToUse) :
    PortState(parent, registerToUse)  {
}


/**
 * Destructor.
 */
OutputPortState::~OutputPortState() {
}

