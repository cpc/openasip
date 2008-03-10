/**
 * @file NullImmediateControlField.cc
 *
 * Implementation of NullImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullImmediateControlField.hh"
#include "BinaryEncoding.hh"

BinaryEncoding NullImmediateControlField::bem_;
NullImmediateControlField NullImmediateControlField::field_;

/**
 * The constructor.
 */
NullImmediateControlField::NullImmediateControlField() :
    ImmediateControlField(bem_) {
}


/**
 * The destructor.
 */
NullImmediateControlField::~NullImmediateControlField() {
}


/**
 * Returns the only instance of ImmediateControlField.
 */
NullImmediateControlField&
NullImmediateControlField::instance() {
    return field_;
}
