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
