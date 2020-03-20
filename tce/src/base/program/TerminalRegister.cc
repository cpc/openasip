/*
    Copyright (c) 2002-2011 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TerminalRegister.cc
 *
 * Implementation of TerminalRegister class.
 *
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#include "TerminalRegister.hh"
#include "ImmediateUnit.hh"
#include "DisassemblyRegister.hh"
#include "UnboundedRegisterFile.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param unit The register file.
 * @param port The port of the register file.
 * @param index Register identifier.
 */
TerminalRegister::TerminalRegister(const Port& port, int index)
    : unit_(*port.parentUnit()), port_(port), index_(index), isImmUnit_(false) {
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
TerminalRegister::registerFile() const {
    if (isImmUnit_ == false) {
        return static_cast<const RegisterFile&>(unit_);
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
TerminalRegister::immediateUnit() const {
    if (isImmUnit_ == true) {
        return static_cast<const ImmediateUnit&>(unit_);
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
TerminalRegister::port() const {
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
TerminalRegister::setIndex(int index) {
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

TCEString
TerminalRegister::toString() const {
    DisassemblyRegister disasm(registerFile().name(), index());
    return disasm.toString();
}

/**
 * Tells whether the terminal is a reg in UniversalMachine
 *
 * @return True if the terminal is a register in universalmachine,
 * ie. not in any real machine. This practically means the
 * register is unallocated and meant to be bypassed.
 */
bool
TerminalRegister::isUniversalMachineRegister() const {
    return (dynamic_cast<const UnboundedRegisterFile*>(&unit_) != NULL);
}

}
