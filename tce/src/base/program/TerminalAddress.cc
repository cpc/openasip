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
 * @file TerminalAddress.cc
 *
 * Implementation of TerminalAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "TerminalAddress.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * Constructor. Creates a terminal address from a given immediate value and
 * a reference to an address space.
 *
 * @param value Value of the address of the location.
 * @param space Address space of the address.
 */
TerminalAddress::TerminalAddress(
    SimValue value,
    const TTAMachine::AddressSpace& space)
    : TerminalImmediate(value), space_(space) {
}

/**
 * The destructor.
 */
TerminalAddress::~TerminalAddress() {
}

/**
 * Tells whether the terminal is an inline immediate that refers to an
 * address.
 *
 * @return true always.
 */
bool
TerminalAddress::isAddress() const {
    return true;
}

/**
 * Returns the address carried by this terminal.
 *
 * @return An address.
 * @exception WrongSubclass never.
 */
Address
TerminalAddress::address() const throw (WrongSubclass) {
    return Address(value_.unsignedValue(), space_);
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalAddress::copy() const {
    return new TerminalAddress(TerminalImmediate::value(), space_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool 
TerminalAddress::equals(const Terminal& other) const {
    
    if (other.isAddress() != true) {
        return false;
    }
    return (address().location() == other.address().location() && 
            address().space().name() == other.address().space().name());
}


} // namespace TTAProgram
