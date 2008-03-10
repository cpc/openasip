/**
 * @file DisassemblySequentialGuard.cc
 *
 * Implementation of DisassemblySequentialGuard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblySequentialGuard.hh"
#include "DisassemblyBoolRegister.hh"

/**
 * The constructor.
 *
 * @param inverted Defines if the guard is inverted or not.
 */
DisassemblySequentialGuard::DisassemblySequentialGuard(bool inverted):
    DisassemblyGuard(new DisassemblyBoolRegister(), inverted) {
}


/**
 * The destructor.
 */
DisassemblySequentialGuard::~DisassemblySequentialGuard() {
}

/**
 * Returns disassembly of the guard.
 *
 * @return Disassembly of the guard as a string.
 */
std::string
DisassemblySequentialGuard::toString() const {
    std::string disassembly;
    if (inverted_) {
	disassembly = "! ";
    } else {
	disassembly = "? ";
    }
    return disassembly;
}
