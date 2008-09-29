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
 * @file TerminalRegister.cc
 *
 * Implementation of TerminalRegister class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "TerminalRegister.hh"
#include "ImmediateUnit.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param unit The register file.
 * @param port The port of the register file.
 * @param index Register identifier.
 */
TerminalRegister::TerminalRegister(
    const Port& port, int index) throw (InvalidData):
    unit_(*port.parentUnit()), port_(port), index_(index), isImmUnit_(false) {

    if (dynamic_cast<const ImmediateUnit*>(&unit_) != NULL) {
        isImmUnit_ = true;
    } else if (dynamic_cast<const RegisterFile*>(&unit_) == NULL) {
        throw InvalidData(
            __FILE__, __LINE__, "TerminalRegister::TerminalRegister()",
            "Unit of the terminal has invalid type");
    }
}

/**
 * The destructor.
 */
TerminalRegister::~TerminalRegister() {
}

/**
 * Returns the register file of the general-purpose register.
 *
 * Applicable only if the unit of the terminal is an instance of
 * RegisterFile.
 *
 * @return The register file of the general-purpose register.
 * @exception WrongSubclass if the unit of the terminal is not an instance
 *            of RegisterFile.
 */
const RegisterFile&
TerminalRegister::registerFile() const throw (WrongSubclass) {

    if (isImmUnit_ == false) {
        return dynamic_cast<const RegisterFile&>(unit_);
    } else {
        throw WrongSubclass(
            __FILE__, __LINE__, "TerminalRegister::registerFile()",
            "Unit of the terminal is not of type RegisterFile");
    }
}

/**
 * Returns the immediate unit of the long immediate register.
 *
 * Applicable only if the unit of the terminal is an instance of
 * ImmediateUnit.
 *
 * @return The immediate unit of the long immediate register.
 * @exception WrongSubclass if the unit of the terminal is not an instance
 *            of ImmediateUnit.
 */
const ImmediateUnit&
TerminalRegister::immediateUnit() const throw (WrongSubclass) {

    if (isImmUnit_ == true) {
        return dynamic_cast<const ImmediateUnit&>(unit_);
    } else {
        throw WrongSubclass(
            __FILE__, __LINE__, "TerminalRegister::immediateUnit()",
            "Unit of the terminal is not of type ImmediateUnit");
    }
}

/**
 * Return the port.
 *
 * @return The port of the unit of the terminal.
 * @exception WrongSubclass never.
 */
const Port&
TerminalRegister::port() const throw (WrongSubclass) {
    return port_;
}

/**
 * Change the register of the register file to the given index.
 *
 * @param index The new register index.
 * @exception OutOfRange if index is not smaller than the size of the
 *            register file or immediate unit it belongs to.
 */
void
TerminalRegister::setIndex(int index)
    throw (OutOfRange) {

    const BaseRegisterFile& reg =
        dynamic_cast<const BaseRegisterFile&>(unit_);

    if (index < reg.numberOfRegisters()) {
        index_ = index;
    } else {
        throw OutOfRange(
            __FILE__, __LINE__, "TerminalRegister::setIndex()",
            "Index out of range.");
    }
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalRegister::copy() const {
    return new TerminalRegister(port_, index_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool
TerminalRegister::equals(const Terminal& other) const {
    try {
        if (isImmediateRegister()) {
            return (other.isImmediateRegister() &&
                index() == other.index() &&
                immediateUnit().name() == other.immediateUnit().name());
        }
        if (isGPR()) {
            return (other.isGPR() &&
                index() == other.index() &&
                registerFile().name() == other.registerFile().name());
        }
	return false;
    } catch (const WrongSubclass&) {
        // the method should throw this when called for wrong type,
        // thus the objects cannot be equal
        return false;
    }
}


}
