/**
 * @file NullNOPEncoding.cc
 *
 * Implementation of NullNOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullNOPEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

BinaryEncoding NullNOPEncoding::bem_;
MoveSlot NullNOPEncoding::moveSlot_("NULL", bem_);
SourceField NullNOPEncoding::sourceField_(
    BinaryEncoding::LEFT, moveSlot_);
NullNOPEncoding NullNOPEncoding::instance_;

/**
 * The constructor.
 */
NullNOPEncoding::NullNOPEncoding() :
    NOPEncoding(0, 0, sourceField_) {
}


/**
 * The destructor.
 */
NullNOPEncoding::~NullNOPEncoding() {
}


/**
 * Returns the only instance of NullNOPEncoding.
 *
 * @return The instance.
 */
NullNOPEncoding&
NullNOPEncoding::instance() {
    return instance_;
}
