/**
 * @file DisassemblyNOP.cc
 *
 * Implementation of DisassemblyNOP class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyNOP.hh"

/**
 * The Contstructor.
 */
DisassemblyNOP::DisassemblyNOP():
    DisassemblyInstructionSlot() {

}


/**
 * The destructor.
 */
DisassemblyNOP::~DisassemblyNOP() {
}


/**
 * Returns disassembly of the NOP as a string.
 *
 * @return Disassembled move as a string.
 */
std::string
DisassemblyNOP::toString() const {
    return "...";
}
