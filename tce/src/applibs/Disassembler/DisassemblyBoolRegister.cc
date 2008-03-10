/**
 * @file DisassemblyBoolRegister.cc
 *
 * Implementation of DisassemblyBoolRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyBoolRegister.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param index Index of the register.
 */
DisassemblyBoolRegister::DisassemblyBoolRegister():
    DisassemblyElement() {
}


/**
 * The destructor.
 */
DisassemblyBoolRegister::~DisassemblyBoolRegister() {
}


/**
 * Returns disassembly of the bool register.
 *
 * @return Disassembly of the bool register as a string.
 */
std::string
DisassemblyBoolRegister::toString() const {
    return "bool";
}
