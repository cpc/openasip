/**
 * @file NullBridgeEncoding.cc
 *
 * Implementation of NullBridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullBridgeEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

BinaryEncoding NullBridgeEncoding::bem_;
MoveSlot NullBridgeEncoding::moveSlot_("NULL", bem_);
SourceField NullBridgeEncoding::sourceField_(
    BinaryEncoding::LEFT, moveSlot_);
NullBridgeEncoding NullBridgeEncoding::instance_;


/**
 * The constructor.
 */
NullBridgeEncoding::NullBridgeEncoding() :
    BridgeEncoding("NULL", 0, 0, sourceField_) {
}


/**
 * The destructor.
 */
NullBridgeEncoding::~NullBridgeEncoding() {
}


/**
 * Returns the instance of NullSocketEncoding.
 *
 * @return The only instance.
 */
NullBridgeEncoding&
NullBridgeEncoding::instance() {
    return instance_;
}
