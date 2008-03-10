/**
 * @file DisassemblyIntRegister.cc
 *
 * Implementation of DisassemblyIntRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyIntRegister.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param index Index of the register.
 */
DisassemblyIntRegister::DisassemblyIntRegister(Word index):
    DisassemblyElement(),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyIntRegister::~DisassemblyIntRegister() {
}


/**
 * Returns disassembly of the int register.
 *
 * @return Disassembly of the int register as a string.
 */
std::string
DisassemblyIntRegister::toString() const {

    switch (index_) {
    case 0: return "ires0"; // Return value.
    case 1: return "sp";    // Stack pointer.
    // Registers for passing function parameters:
    case 2: return "iarg1";
    case 3: return "iarg2";
    case 4: return "iarg3";
    case 5: return "iarg4";
    }

    return "r" + Conversion::toString(index_);
}
