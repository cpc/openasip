/**
 * @file NullGPRGuardEncoding.cc
 *
 * Implementation of NullGPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullGPRGuardEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

BinaryEncoding NullGPRGuardEncoding::bem_;
MoveSlot NullGPRGuardEncoding::moveSlot_("NULL", bem_);
GuardField NullGPRGuardEncoding::guardField_(moveSlot_);
NullGPRGuardEncoding NullGPRGuardEncoding::instance_;

/**
 * The constructor.
 */
NullGPRGuardEncoding::NullGPRGuardEncoding() :
    GPRGuardEncoding("NULL", 0, false, 0, guardField_) {
}


/**
 * The destructor.
 */
NullGPRGuardEncoding::~NullGPRGuardEncoding() {
}


/**
 * Returns the instance of NullGPRGuardEncoding.
 *
 * @return The instance.
 */
NullGPRGuardEncoding&
NullGPRGuardEncoding::instance() {
    return instance_;
}
