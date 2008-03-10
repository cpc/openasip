/**
 * @file Listener.cc
 *
 * Implementation of Listener class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @note rating: red
 */


#include "Listener.hh"
#include "Application.hh"

/**
 * Constructor.
 */
Listener::Listener() {
}


/**
 * Destructor.
 */
Listener::~Listener() {
}


/**
 * A function that is called every time the event the listener has registered
 * to occurs.
 *
 * The default implementation passes the function call to the parameterless
 * handleEvent(). This fucntion can be redefined in the derived class, if
 * the implemented listener is interested in the event ids.
 */
void
Listener::handleEvent(int /* unused */) {
    handleEvent();
}


/**
 * A function that is called every time the event the listener has registered
 * to occurs.
 */
void
Listener::handleEvent() {
}
