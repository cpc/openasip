/**
 * @file NullUnconditionalGuardEncoding.cc
 *
 * Implementation of NullUnconditionalGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullUnconditionalGuardEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

BinaryEncoding NullUnconditionalGuardEncoding::bem_;
MoveSlot NullUnconditionalGuardEncoding::moveSlot_("NULL", bem_);
GuardField NullUnconditionalGuardEncoding::guardField_(moveSlot_);
NullUnconditionalGuardEncoding NullUnconditionalGuardEncoding::instance_;

/**
 * The constructor.
 */
NullUnconditionalGuardEncoding::NullUnconditionalGuardEncoding() :
    UnconditionalGuardEncoding(false, 0, guardField_) {
}


/**
 * The destructor.
 */
NullUnconditionalGuardEncoding::~NullUnconditionalGuardEncoding() {
}


/**
 * Returns the only instance of NullUnconditionalGuardEncoding class.
 *
 * @return The instance.
 */
NullUnconditionalGuardEncoding&
NullUnconditionalGuardEncoding::instance() {
    return instance_;
}
