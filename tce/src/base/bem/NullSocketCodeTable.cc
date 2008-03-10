/**
 * @file NullSocketCodeTable.cc
 *
 * Implementation of NullSocketCodeTable class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NullSocketCodeTable.hh"
#include "BinaryEncoding.hh"

BinaryEncoding NullSocketCodeTable::bem_;
NullSocketCodeTable NullSocketCodeTable::instance_;

/**
 * The constructor.
 */
NullSocketCodeTable::NullSocketCodeTable() : SocketCodeTable("NULL", bem_) {
}


/**
 * The destructor.
 */
NullSocketCodeTable::~NullSocketCodeTable() {
}


/**
 * Returns the only instance of NullSocketCodeTable.
 *
 * @return The only instance.
 */
NullSocketCodeTable&
NullSocketCodeTable::instance() {
    return instance_;
}
