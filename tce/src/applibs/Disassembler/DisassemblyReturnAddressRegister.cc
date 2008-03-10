/**
 * @file DisassemblyReturnAddressRegister.cc
 *
 * Implementation of DisassemblyReturnAddressRegister class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyReturnAddressRegister.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param index Index of the register.
 */
DisassemblyReturnAddressRegister::DisassemblyReturnAddressRegister():
    DisassemblyElement() {
}


/**
 * The destructor.
 */
DisassemblyReturnAddressRegister::~DisassemblyReturnAddressRegister() {
}


/**
 * Returns disassembly of the return address register.
 *
 * @return Disassembly of the return address register as a string.
 */
std::string
DisassemblyReturnAddressRegister::toString() const {
    return "ra";
}
