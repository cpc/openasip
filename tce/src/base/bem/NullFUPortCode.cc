/**
 * @file NullFUPortCode.cc
 *
 * Implementation of NullFUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullFUPortCode.hh"
#include "BinaryEncoding.hh"
#include "SocketCodeTable.hh"

BinaryEncoding NullFUPortCode::bem_;
SocketCodeTable NullFUPortCode::socketCodes_("NULL", bem_);
NullFUPortCode NullFUPortCode::instance_;

/**
 * The constructor.
 */
NullFUPortCode::NullFUPortCode() :
    FUPortCode("NULL", "NULL", 0, false, socketCodes_) {
}


/**
 * The destructor.
 */
NullFUPortCode::~NullFUPortCode() {
}


/**
 * Returns the only instance of NullFUPortCode.
 *
 * @return The only instance.
 */
NullFUPortCode&
NullFUPortCode::instance() {
    return instance_;
}
