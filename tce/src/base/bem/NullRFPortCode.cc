/**
 * @file NullRFPortCode.cc
 *
 * Implementation of NullRFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullRFPortCode.hh"
#include "BinaryEncoding.hh"
#include "SocketCodeTable.hh"

BinaryEncoding NullRFPortCode::bem_;
SocketCodeTable NullRFPortCode::socketCodes_("NULL", bem_);
NullRFPortCode NullRFPortCode::instance_;

/**
 * The constructor.
 */
NullRFPortCode::NullRFPortCode() :
    RFPortCode("NULL", 0, 0, 0, socketCodes_) {
}


/**
 * The destructor.
 */
NullRFPortCode::~NullRFPortCode() {
}


/**
 * Returns the only instance of NullRFPortCode.
 *
 * @return The only instance.
 */
NullRFPortCode&
NullRFPortCode::instance() {
    return instance_;
}
