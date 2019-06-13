/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TerminalAddress.cc
 *
 * Implementation of TerminalAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "TerminalAddress.hh"
#include "AddressSpace.hh"

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
TerminalAddress::address() const {
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
