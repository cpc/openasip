/**
 * @file NullDestinationField.cc
 *
 * Implementation of NullDestinationField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullDestinationField.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"

BinaryEncoding NullDestinationField::bem_;
MoveSlot NullDestinationField::moveSlot_("NULL", bem_);
NullDestinationField NullDestinationField::instance_;

/**
 * The constructor.
 */
NullDestinationField::NullDestinationField() :
    DestinationField(BinaryEncoding::LEFT, moveSlot_) {
}


/**
 * The destructor.
 */
NullDestinationField::~NullDestinationField() {
}


/**
 * Returns the only instance of NullDestinationField.
 *
 * @return The instance.
 */
NullDestinationField&
NullDestinationField::instance() {
    return instance_;
}
