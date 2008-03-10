/**
 * @file NullImmediateEncoding.cc
 *
 * Implementation of NullImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullImmediateEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

BinaryEncoding NullImmediateEncoding::bem_;
MoveSlot NullImmediateEncoding::moveSlot_("NULL", bem_);
SourceField NullImmediateEncoding::sourceField_(
    BinaryEncoding::LEFT, moveSlot_);
NullImmediateEncoding NullImmediateEncoding::instance_;

/**
 * The constructor.
 */
NullImmediateEncoding::NullImmediateEncoding() :
    ImmediateEncoding(0, 0, 0, sourceField_) {
}


/**
 * The destructor.
 */
NullImmediateEncoding::~NullImmediateEncoding() {
}


/**
 * Returns the only instance of NullImmediateEncoding.
 *
 * @return The instance.
 */
NullImmediateEncoding&
NullImmediateEncoding::instance() {
    return instance_;
}
