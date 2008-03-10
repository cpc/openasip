/**
 * @file DisassemblyImmediateRegister.cc
 *
 * Implementation of DisassemblyImmediateRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediateRegister.hh"
#include "Conversion.hh"

/**
 * The constructor.
 *
 * @param immName Name of the immediate unit.
 * @param index Index of the register in the immediate unit.
 */
DisassemblyImmediateRegister::DisassemblyImmediateRegister(
    std::string immName, Word index) :
    DisassemblyElement(),
    immName_(immName),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyImmediateRegister::~DisassemblyImmediateRegister() {
}



/**
 * Returns disassembly of the immediate register.
 *
 * @return Disassembly of the immediate register as a string.
 */
std::string
DisassemblyImmediateRegister::toString() const {
    return immName_ + "." + Conversion::toString(index_);
}
