/**
 * @file DisassemblyFPRegister.cc
 *
 * Implementation of DisassemblyFPRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyFPRegister.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param index Index of the register.
 */
DisassemblyFPRegister::DisassemblyFPRegister(Word index):
    DisassemblyElement(),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyFPRegister::~DisassemblyFPRegister() {
}


/**
 * Returns disassembly of the float register.
 *
 * @return Disassembly of the float register as a string.
 */
std::string
DisassemblyFPRegister::toString() const {

    // Registers reserved for passing and returning function parameters.
    switch (index_) {
    case 0: return "fres0";
    case 1: return "farg1";
    case 2: return "farg2";
    case 3: return "farg3";
    case 4: return "farg4";
    }

    return "f" + Conversion::toString(index_);
}
