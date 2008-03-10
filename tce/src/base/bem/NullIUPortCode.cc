/*
 * @file NullIUPortCode.cc
 *
 * Implementation of NullIUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullIUPortCode.hh"
#include "BinaryEncoding.hh"
#include "SocketCodeTable.hh"

BinaryEncoding NullIUPortCode::bem_;
SocketCodeTable NullIUPortCode::socketCodes_("NULL", bem_);
NullIUPortCode NullIUPortCode::instance_;

/**
 * The constructor.
 */
NullIUPortCode::NullIUPortCode() :
    IUPortCode("NULL", 0, 0, 0, socketCodes_) {
}


/**
 * The destructor.
 */
NullIUPortCode::~NullIUPortCode() {
}


/**
 * Returns the only instance of NullIUPortCode.
 *
 * @return The only instance.
 */
NullIUPortCode&
NullIUPortCode::instance() {
    return instance_;
}
