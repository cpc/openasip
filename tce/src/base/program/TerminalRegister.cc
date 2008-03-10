/**
 * @file TerminalRegister.cc
 *
 * Implementation of TerminalRegister class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
