/**
 * @file NullSocketEncoding.cc
 *
 * Implementation of NullSocketEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullSocketEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

BinaryEncoding NullSocketEncoding::bem_;
MoveSlot NullSocketEncoding::moveSlot_("NULL", bem_);
SourceField NullSocketEncoding::sourceField_(BinaryEncoding::LEFT, moveSlot_);
NullSocketEncoding NullSocketEncoding::instance_;


/**
 * The constructor.
 */
NullSocketEncoding::NullSocketEncoding() :
    SocketEncoding("NULL", 0, 0, sourceField_) {
}


/**
 * The destructor.
 */
NullSocketEncoding::~NullSocketEncoding() {
}


/**
 * Returns the instance of NullSocketEncoding.
 *
 * @return The only instance.
 */
NullSocketEncoding&
NullSocketEncoding::instance() {
    return instance_;
}
