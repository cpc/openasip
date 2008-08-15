/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
