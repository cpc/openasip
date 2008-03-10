/**
 * @file NullGuard.cc
 *
 * Implementation of NullGuard class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullGuard.hh"
#include "Bus.hh"

namespace TTAMachine {

Bus NullGuard::bus_("NULL", 1, 0, Machine::ZERO);
NullGuard NullGuard::instance_;

/**
 * The constructor.
 */
NullGuard::NullGuard() : Guard(false, bus_) {
}


/**
 * The destructor.
 */
NullGuard::~NullGuard() {
}


/**
 * Returns the only instance of NullGuard.
 *
 * @return The instance.
 */
NullGuard&
NullGuard::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Bus*
NullGuard::parentBus() const {
    abortWithError("NullGuard::parentBus");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isEqual(const Guard&) const {
    abortWithError("NullGuard::isEqual");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isInverted() const {
    abortWithError("NullGuard::isInverted");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isMoreRestrictive(const Guard&) const {
    abortWithError("NullGuard::isMoreRestrictive");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isLessRestrictive(const Guard&) const {
    abortWithError("NullGuard::isLessRestrictive");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isDisjoint(const Guard&) const {
    abortWithError("NullGuard::isDisjoint");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullGuard::saveState() const {
    abortWithError("NullGuard::saveState");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullGuard::loadState(const ObjectState*) 
    throw (ObjectStateLoadingException) {

    abortWithError("NullGuard::loadState");
}

}
