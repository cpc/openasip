/**
 * @file DisassemblyLabel.cc
 *
 * Implementation of DisassemblyLabel class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyLabel.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param label Label name.
 */
DisassemblyLabel::DisassemblyLabel(std::string label):
    DisassemblyElement(),
    label_(label) {
}


/**
 * The destructor.
 */
DisassemblyLabel::~DisassemblyLabel() {
}


/**
 * Returns disassembly of the code label.
 *
 * @return Disassembly of the code label as a string.
 */
std::string
DisassemblyLabel::toString() const {
    return label_;
}
