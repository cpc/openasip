/**
 * @file DisassemblyGuard.cc
 *
 * Implementation of DisassemblyGuard class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyGuard.hh"

/**
 * The constructor.
 *
 * @param guardedElement The element which the guard refers to.
 * @param inverted Defines if the guard is inverted or not.
 */
DisassemblyGuard::DisassemblyGuard(
    DisassemblyElement* guardedElement, bool inverted):
    inverted_(inverted),
    guardedElement_(guardedElement) {
}


/**
 * The destructor.
 */
DisassemblyGuard::~DisassemblyGuard() {
    delete guardedElement_;
}


/**
 * True, if the guard is inverted.
 *
 * @return True if the guard is inverted, false if not.
 */
bool
DisassemblyGuard::inverted() const {
    return inverted_;
}


/**
 * Returns the element which the guard refers to.
 *
 * @return Guarded element.
 */
const DisassemblyElement&
DisassemblyGuard::guardedElement() const {
    return *guardedElement_;
}


/**
 * Returns disassembly of the guard.
 *
 * @return Disassembly of the guard as a string.
 */
std::string
DisassemblyGuard::toString() const {
    std::string disassembly;
    if (inverted_) {
	disassembly = "!";
    } else {
	disassembly = "?";
    }

    disassembly = disassembly + guardedElement_->toString() + " ";
    return disassembly;
}
