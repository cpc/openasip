/**
 * @file NullMoveGuard.cc
 *
 * Implementation of NullMoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullMoveGuard.hh"
#include "Application.hh"
#include "NullTerminal.hh"
#include "NullGuard.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullMoveGuard
/////////////////////////////////////////////////////////////////////////////

NullMoveGuard NullMoveGuard::instance_;

/**
 * The constructor.
 */
NullMoveGuard::NullMoveGuard() :
    MoveGuard(NullGuard::instance()) {
}

/**
 * The destructor.
 */
NullMoveGuard::~NullMoveGuard() {
}

/**
 * Returns an instance of NullMoveGuard class (singleton).
 *
 * @return Singleton instance of NullMoveGuard class.
 */
NullMoveGuard&
NullMoveGuard::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullMoveGuard::isUnconditional() const {
    abortWithError("isUnconditional()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullMoveGuard::isInverted() const {
    abortWithError("isInverted()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Guard&
NullMoveGuard::guard() const {
    abortWithError("source()");
    return NullGuard::instance();
}

}
