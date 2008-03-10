/**
 * @file NullGuardField.cc
 *
 * Implementation of NullGuardField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullGuardField.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"

BinaryEncoding NullGuardField::bem_;
MoveSlot NullGuardField::moveSlot_("NULL", bem_);
NullGuardField NullGuardField::instance_;

/**
 * The constructor.
 */
NullGuardField::NullGuardField() : GuardField(moveSlot_) {
}


/**
 * The destructor.
 */
NullGuardField::~NullGuardField() {
}


/**
 * Returns the instance of NullGuardField.
 *
 * @return The instance.
 */
NullGuardField&
NullGuardField::instance() {
    return instance_;
}

