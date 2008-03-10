/**
 * @file GlobalLock.cc
 *
 * Definition of GlobalLock class.
 *
 * @author Jussi Nykänen 2005 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "GlobalLock.hh"

/**
 * Constructor.
 */
GlobalLock::GlobalLock() : isLocked_(false) {
}

/**
 * Destructor.
 */
GlobalLock::~GlobalLock() {
}

/**
 * Raises the lock signal.
 */
void
GlobalLock::lock() {
    isLocked_ = true;
}

/**
 * Lowers the clock signal.
 */
void
GlobalLock::unlock() {
    isLocked_ = false;
}

/**
 * Returns true if lock signal is raised, false otherwise.
 *
 * @return True if lock signal is raised, false otherwise.
 */
bool
GlobalLock::isLocked() const {
    return isLocked_;
}
