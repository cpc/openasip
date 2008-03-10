/**
 * @file NullAddress.cc
 *
 * Implementation of NullAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullAddress.hh"
#include "NullAddressSpace.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullAddress
/////////////////////////////////////////////////////////////////////////////

NullAddress NullAddress::instance_;

/**
 * The constructor.
 */
NullAddress::NullAddress() :
    Address(0, NullAddressSpace::instance()) {
}

/**
 * The destructor.
 */
NullAddress::~NullAddress() {
}

/**
 * Returns an instance of NullAddress class (singleton).
 *
 * @return Singleton instance of NullAddress class.
 */
NullAddress&
NullAddress::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return 0.
 */
UIntWord
NullAddress::location() const {
    abortWithError("location()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return A null address space.
 */
const AddressSpace&
NullAddress::space() const {
    abortWithError("space()");
    return NullAddressSpace::instance();
}

}
