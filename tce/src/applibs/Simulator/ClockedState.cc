/**
 * @file ClockedState.cc
 *
 * Definition of ClockedState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "ClockedState.hh"

/**
 * Constructor.
 */
ClockedState::ClockedState() {
}

/**
 * Destructor.
 */
ClockedState::~ClockedState() {
}

/**
 * Returns true in case the piece of state is idle, thus do not need to be
 * notified of clock advance events.
 *
 * @return Idleness status, default implementation always returns false.
 */
bool
ClockedState::isIdle() {
    return false;
}
