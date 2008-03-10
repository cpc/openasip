/**
 * @file DisassemblyImmediate.cc
 *
 * Implementation of DisassemblyImmediate class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediate.hh"
#include "Conversion.hh"

/**
 * The constructor.
 *
 * @param value Inline immediate value.
 */
DisassemblyImmediate::DisassemblyImmediate(SimValue value):
    DisassemblyElement(),
    value_(value) {
}


/**
 * The destructor.
 */
DisassemblyImmediate::~DisassemblyImmediate() {
}


/**
 * Returns disassembly of the inline immediate value.
 *
 * @return Disassembly of the inline immediate value as a string.
 */
std::string
DisassemblyImmediate::toString() const {
    return Conversion::toString(value_.intValue());
}
