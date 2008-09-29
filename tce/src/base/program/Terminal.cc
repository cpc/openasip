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
 * @file Terminal.cc
 *
 * Implementation of Terminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Exception.hh"
#include "Terminal.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Terminal
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 */
Terminal::Terminal() {
}

/**
 * The destructor.
 */
Terminal::~Terminal() {
}

/**
 * Tells whether the terminal is an inline immediate value.
 *
 * @return True if the terminal is an inline immediate value.
 */
bool
Terminal::isImmediate() const {
    return false;
}

/**
 * Tells whether the terminal is an inline immediate value that refers
 * to an address.
 *
 * @return True if the terminal is an inline immediate value that refers
 *         to an address.
 */
bool
Terminal::isAddress() const {
    return false;
}

/**
 * Tells whether the terminal is an inline immediate value that refers
 * to an instruction address.
 *
 * @return True if the terminal is an inline immediate value that refers
 *         to an instruction address.
 */
bool
Terminal::isInstructionAddress() const {
    return false;
}

/**
 * Tells whether the terminal is a long immediate register.
 *
 * @return True if the terminal is a long immediate register.
 */
bool
Terminal::isImmediateRegister() const {
    return false;
}

/**
 * Tells whether the terminal is a general-purpose register.
 *
 * @return True if the terminal is a general-purpose register.
 */
bool
Terminal::isGPR() const {
    return false;
}

/**
 * Tells whether the terminal is a function unit port (possibly,
 * including an operation code).
 *
 * @return True if the terminal is a function unit port.
 */
bool
Terminal::isFUPort() const {
    return false;
}

/**
 * Tells whether the terminal is a the return address port
 * in gcu.
 *
 * @return True if the terminal is a the RA port.
 */
bool
Terminal::isRA() const {
    return false;
}


/**
 * Returns the value of the inline immediate.
 *
 * @return The value of the inline immediate.
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalImmediate.
 */
SimValue
Terminal::value() const throw(WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Returns a reference to the instruction to which the immediate points.
 *
 * @return A reference to the instruction to which the immediate points.
 */
InstructionReference&
Terminal::instructionReference() const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__);
}


/**
 * Returns the address held by this terminal.
 *
 * @return A memory address.
 * @exception WrongSubclass if the terminal is not an instance of
 *     TerminalAddress.
 */
Address
Terminal::address() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
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
Terminal::registerFile() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
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
Terminal::immediateUnit() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Returns the function unit of the port.
 *
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalFUPort.
 */
const FunctionUnit&
Terminal::functionUnit() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Return the index of the register (either a long immediate or a
 * general-purpose register).
 *
 * @return The index of the register.
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalRegister.
 */
int
Terminal::index() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Tells whether terminal transports an opcode to a function unit port.
 *
 * @return True if the terminal transports an opcode to a function unit port.
 * @exception WrongSubclass always.
*/
bool
Terminal::isOpcodeSetting() const 
    throw (WrongSubclass) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Terminal must be TerminalFUPort for this method.");
}

/**
 * Tells whether terminal is a triggering FU port.
 *
 * @return True if the terminal is a triggering port.
 * @exception WrongSubclass always.
*/
bool
Terminal::isTriggering() const 
    throw (WrongSubclass) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Terminal must be TerminalFUPort for this method.");
}

/**
 * Return the operation code transported into the function unit port by this
 * terminal.
 *
 * Throws an error condition if the terminal does not read from or write to
 * a function unit port (that is, if the terminal if of a wrong type). 
 * Throws an error condition also if the terminal does not access an
 * opcode-setting function unit port.
 *
 * @exception WrongSubclass If the terminal is of the wrong type.
 * @exception InvalidData If the terminal is of the right type, but it does
 *     not carry an operation-code.
 * @return The operation carried by this terminal.
 */
Operation&
Terminal::operation() const 
    throw (WrongSubclass, InvalidData) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Terminal must be TerminalFUPort for this method.");

}

/**
 * Return the operation to which this terminal was originally bound.
 *
 * NOTE! : Method must not be used for checkin if terminal contains
 *         opcode. See. operation().
 *
 * Method is mainly used for preserving information to which operation
 * port reference is bound, during TPEF -> POM -> TPEF write cycles.
 *
 * @return The operation code to which terminal was originally bound.
 *         Null operation if terminal does not contain operation code.
 *
 * @todo This should be probably merged with operation() as "hints" are
 * not really useful, we should make sure the operation of the move is known.
 */
Operation&
Terminal::hintOperation() const 
    throw (WrongSubclass, InvalidData) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Terminal must be TerminalFUPort for this method.");
}

/**
 * Return the index that identifies the operation input or output that
 * is represented by this terminal.
 *
 * Throws an error condition if the terminal does not read from or write to
 * a function unit port (that is, if the terminal if of a wrong type). 
 * Throws an error condition also if the terminal does not access an
 * opcode-setting function unit port, nor it bears auxiliary "hint
 * operation" information.
 *
 * @return The index that identifies the operation input or output that
 * is represented by this terminal.
 * @exception WrongSubclass If the terminal is of the wrong type.
 * @exception InvalidData If the terminal is of the right type, but does
 *     not have any operation-related information.
 */
int
Terminal::operationIndex() const throw (WrongSubclass, InvalidData) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Terminal must be TerminalFUPort for this method.");
}


/**
 * Return the port accessed by this terminal.
 *
 * @return The port of the unit of the terminal.
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalFUPort or TerminalRegister.
 */
const Port&
Terminal::port() const throw (WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Change the register of the register file to the given index.
 *
 * @param index The new register index.
 * @exception OutOfRange if index is not smaller than the size of the
 *            register file or immediate unit it belongs to.
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalRegister.
 */
void
Terminal::setIndex(int) throw (WrongSubclass, OutOfRange) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Set a new referred instruction.
 *
 * @param ref The new instruction reference.
 * @exception WrongSubclass if the terminal is not an instance of
 *            TerminalInstructionAddress
 */
void
Terminal::setInstructionReference(InstructionReference&)
    throw(WrongSubclass) {
    throw WrongSubclass(__FILE__, __LINE__);
}

/**
 * Comparison operator for Terminals.
 *
 * Delegates the comparison to dynamically bound equals() method.
 *
 * @param other Comparison target.
 * @return True in case the objects are equal.
 */
bool
Terminal::operator==(const Terminal& other) const {
    return equals(other);
}

}
