/**
 * @file DisassemblyRegister.cc
 *
 * Implementation of DisassemblyRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyRegister.hh"
#include "Conversion.hh"

/**
 * The constructor.
 *
 * @param rfName Name of the register file.
 * @param index Index of the register in the register file.
 */
DisassemblyRegister::DisassemblyRegister(std::string rfName, Word index) :
    DisassemblyElement(),
    rfName_(rfName),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyRegister::~DisassemblyRegister() {
}



/**
 * Returns disassembly of the register file.
 *
 * @return Disassembly of the register as a string.
 */
std::string
DisassemblyRegister::toString() const {
    return rfName_ + "." + Conversion::toString(index_);
}
