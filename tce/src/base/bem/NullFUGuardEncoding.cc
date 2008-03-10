/**
 * @file NullFUGuardEncoding.cc
 *
 * Implementation of NullFUGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullFUGuardEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

BinaryEncoding NullFUGuardEncoding::bem_;
MoveSlot NullFUGuardEncoding::moveSlot_("NULL", bem_);
GuardField NullFUGuardEncoding::guardField_(moveSlot_);
NullFUGuardEncoding NullFUGuardEncoding::instance_;

/**
 * The constructor.
 */
NullFUGuardEncoding::NullFUGuardEncoding() :
    FUGuardEncoding("NULL", "NULL", false, 0, guardField_) {
}


/**
 * The destructor.
 */
NullFUGuardEncoding::~NullFUGuardEncoding() {
}


/**
 * Returns the instance of NullFUGuardEncoding.
 *
 * @return The instance.
 */
NullFUGuardEncoding&
NullFUGuardEncoding::instance() {
    return instance_;
}
