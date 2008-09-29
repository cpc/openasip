/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file NullAddress.cc
 *
 * Implementation of NullAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
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
