/**
 * @file Immediate.cc
 *
 * Implementation of Immediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "Immediate.hh"
#include "Terminal.hh"
#include "NullTerminal.hh"
#include "InstructionTemplate.hh"
#include "TerminalImmediate.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Immediate
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * Creates a long immediate with the given value. The value specifies,
 * implicitly, also the width of the instruction field(s) where the
 * immediate bits are encoded). The destination register and the set
 * of instruction slots where the immediate bits are encoded are also
 * given as parameters.
 *
 * The ownership of the destination terminal object will be passed to
 * the immediate.
 *
 * @param value Value of the immediate.
 * @param dst Destination register.
 */

//Immediate::Immediate(
//    SimValue value, Terminal* dst)
//    throw (IllegalParameters) :
//    value_(value), dst_(dst) {
//}

Immediate::Immediate(
    TerminalImmediate* value, Terminal* dst)
    throw (IllegalParameters) :
    value_(value), dst_(dst) {
}

/**
 * The destructor.
 */
Immediate::~Immediate() {
    if (dst_ != NULL && dst_ != &NullTerminal::instance()) {
        delete dst_;
        dst_ = NULL;
    }

    if (value_ != NULL) {
        delete value_;
        value_ = NULL;
    }
}

/**
 * Returns the destination register of this immediate.
 *
 * @return The destination register of this immediate.
 */
const Terminal&
Immediate::destination() const {
    assert(dst_ != NULL);
    return *dst_;
}

/**
 * Returns the value of this immediate.
 *
 * @return The value of this immediate.
 */
TerminalImmediate&
Immediate::value() const {
    assert(value_ != NULL);
    return *value_;
}

/**
 * Sets the value of immediate.
 *
 * @param Value to set for immediate.
 */
void
Immediate::setValue(TerminalImmediate* value) {
    if (value_ != NULL) {
        delete value_;
    }
    value_ = value;
}

/**
 * Makes a copy of the immediate.
 *
 * The copy is identical, except that it is not registered to the
 * instruction of the original immediate (and therefore, any address it
 * refers to is not meaningful).
 *
 * @return A copy of the immediate.
 */
Immediate*
Immediate::copy() const {
    return new Immediate(
        dynamic_cast<TerminalImmediate*>(value_->copy()), dst_->copy());
}

}
