/**
 * @file TerminalImmediate.cc
 *
 * Implementation of TerminalImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "TerminalImmediate.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param value The value of the inline immediate.
 */
TerminalImmediate::TerminalImmediate(SimValue value):
    value_(value) {
}

/**
 * The destructor.
 */
TerminalImmediate::~TerminalImmediate() {
}

/**
 * Tells whether the terminal is an inline immediate value.
 *
 * @return true always.
 */
bool
TerminalImmediate::isImmediate() const {
    return true;
}

/**
 * Returns the value of the inline immediate.
 *
 * @return The value of the inline immediate.
 * @exception WrongSubclass never.
 */
SimValue
TerminalImmediate::value() const throw (WrongSubclass) {
    return value_;
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalImmediate::copy() const {
    return new TerminalImmediate(value_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool 
TerminalImmediate::equals(const Terminal& other) const {

    if (other.isImmediate() != true) {
        return false;
    }
    return value() == other.value();
}

}
