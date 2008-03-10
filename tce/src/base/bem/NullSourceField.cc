/**
 * @file NullSourceField.cc
 *
 * Implementation of NullSourceField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullSourceField.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"

BinaryEncoding NullSourceField::bem_;
MoveSlot NullSourceField::moveSlot_("NULL", bem_);
NullSourceField NullSourceField::instance_;

/**
 * The constructor.
 */
NullSourceField::NullSourceField() :
    SourceField(BinaryEncoding::LEFT ,moveSlot_) {
}


/**
 * The destructor.
 */
NullSourceField::~NullSourceField() {
}


/**
 * Returns the only instance of NullSourceField.
 *
 * @return The instance.
 */
NullSourceField&
NullSourceField::instance() {
    return instance_;
}
