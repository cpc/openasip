/**
 * @file MoveGuard.cc
 *
 * Implementation of MoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "MoveGuard.hh"
#include "Guard.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// MoveGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.

 * @param guard The guard.
 */
MoveGuard::MoveGuard(TTAMachine::Guard& guard):
    guard_(&guard) {
}

/**
 * Destructor.
 */
MoveGuard::~MoveGuard() {
}

/**
 * Tells whether this guard is a constant true or false value.
 *
 * @return false always.
 */
bool
MoveGuard::isUnconditional() const {
    return false;
}

/**
 * Tells whether the guard is inverted or not.
 *
 * @return True if the Boolean value computed out of the contents of
 *              the source register is inverted.
 */
bool
MoveGuard::isInverted() const {
    return guard_->isInverted();
}

/**
 * Returns the guard object.
 *
 * @return The guard object.
 */
Guard&
MoveGuard::guard() const {
    return *guard_;
}

/**
 * Makes a copy of the move guard.
 *
 * @return A copy of the move guard.
 */
MoveGuard*
MoveGuard::copy() const {
    return new MoveGuard(*guard_);
}

}
