/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TerminalFUPort.cc
 *
 * Implementation of TerminalFUPort class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "TerminalFUPort.hh"
#include "FunctionUnit.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "DisassemblyFUOpcodePort.hh"
#include "DisassemblyFUPort.hh"

using std::string;
using namespace TTAMachine;

namespace TTAProgram {

/**
 * Constructor.
 *
 * @param port The port of the terminal.
 */
TerminalFUPort::TerminalFUPort(
    const TTAMachine::BaseFUPort& port) throw (IllegalParameters) :
    port_(port), operation_(NULL), opcode_(NULL) {

    if (port_.isOpcodeSetting()) {
        std::string msg = 
            "Opcode setting port terminals must have opcode set!";
        throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Creates a destination that writes to an opcode setting FU port.
 *
 * @param opcodeSettingPort The port of the terminal.
 * @param opcode The operation to trigger.
 */
TerminalFUPort::TerminalFUPort(
    const TTAMachine::FUPort& opcodeSettingPort,
    TTAMachine::HWOperation& opcode) :
    port_(opcodeSettingPort), operation_(&opcode), opcode_(NULL) {

    assert(opcodeSettingPort.isOpcodeSetting());

    opIndex_ = opcode.io(opcodeSettingPort);

    static OperationPool pool;
    // opcode is NullOperation instance if operation for that name was not
    // found
    try {
        opcode_ = &pool.operation(operation_->name().c_str());
    } catch (const Exception& e) {
        IllegalParameters ip(
            __FILE__, __LINE__, __func__, 
            (boost::format(
                "Error loading the operation '%s' definition: '%s'.")
             % operation_->name() % e.errorMessage()).str());
        ip.setCause(e);
        throw ip;
    }
}


/**
 * Constructor.
 *
 * @param operation Operation of terminal.
 * @param opIndex Operation index.
 */
TerminalFUPort::TerminalFUPort(HWOperation &operation, int opIndex)
    throw (IllegalParameters) :
    port_(*operation.port(opIndex)), operation_(&operation), 
    opIndex_(opIndex) {

    /* In case the operand cannot be resolved to a legal port, it's an
       error in the loaded input program. Thus, we cannot abort the program but
       we'll throw an exception instead. */
    if (dynamic_cast<const FUPort*>(&port_) == NULL) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__,
            (boost::format(
                "Port binding of operand %d of operation '%s' "
                "cannot be resolved.") % opIndex % operation.name()).
            str());
    }
    static OperationPool pool;
    // opcode is NullOperation instance if operation for that name was not
    // found
    try {
        opcode_ = &pool.operation(operation_->name().c_str());
    } catch (const Exception& e) {
        IllegalParameters ip(
            __FILE__, __LINE__, __func__, 
            (boost::format(
                "Error loading the operation '%s' definition: '%s'.")
             % operation_->name() % e.errorMessage()).str());
        ip.setCause(e);
        throw ip;
    }
}

/**
 * Copy Constructor. private, only called itnernally by copy();
 * @param tfup object to copy from.
 */
TerminalFUPort::TerminalFUPort(const TerminalFUPort& tfup) :
    Terminal(), port_(tfup.port_), operation_(tfup.operation_),
    opcode_(tfup.opcode_), opIndex_(tfup.opIndex_) {}
                                   
/**
 * The destructor.
 */
TerminalFUPort::~TerminalFUPort() {
}

/**
 * Tells whether the terminal is a function unit port (possibly,
 * including an operation code).
 *
 * @return True always.
 */
bool
TerminalFUPort::isFUPort() const {
    return true;
}

/**
 * Tells whether the terminal is the return address port of gcu.
 *
 * @return returns true if this is the RA.
 */
bool
TerminalFUPort::isRA() const {
    return dynamic_cast<const TTAMachine::SpecialRegisterPort*>
        (&port()) != NULL;
}

/**
 * Tells whether terminal transports an opcode to a function unit port.
 *
 * @return True if the terminal transports an opcode to a function unit port.
 * @exception WrongSubclass never.
*/
bool
TerminalFUPort::isOpcodeSetting() const 
    throw (WrongSubclass) {
    return port_.isOpcodeSetting();
}

/**
 * Tells whether terminal is a triggering port.
 *
 * @return True if the terminal transports an opcode to a function unit port.
 * @exception WrongSubclass never.
*/
bool
TerminalFUPort::isTriggering() const 
    throw (WrongSubclass) {
    return port_.isTriggering();
}


/**
 * Returns the function unit of the port.
 *
 * @return The function unit of the port.
 * @exception WrongSubclass never.
 */
const FunctionUnit&
TerminalFUPort::functionUnit() const 
    throw (WrongSubclass) {
    return *port_.parentUnit();
}

/**
 * Return the operation code transported into the function unit port.
 *
 * @return The operation code transported into the function unit port.
 * @exception InvalidData If terminal does not set opcode for FU.
 */
Operation&
TerminalFUPort::operation() const 
    throw (WrongSubclass, InvalidData) {

    if (port_.isOpcodeSetting() && opcode_ != NULL) {
        return *opcode_;
    } else {
        std::cerr 
            << "operation == NULL || !port_.isopcodesetting()" << std::endl;
    }

    throw InvalidData(__FILE__, __LINE__, __func__,
                        "Terminal does not set opcode for FU.");
}

/**
 * Return the index that identifies the operation input or output that
 * is represented by this terminal.
 *
 * @return The index that identifies the operation input or output that
 * is represented by this terminal.
 * @exception WrongSubclass Never.
 * @exception InvalidData If the terminal is of the the right class type,
 * but the FU port of the terminal is not opcode-setting and bears no
 * auxiliary "hint operation" information.
 * @todo Rename to operandIndex()?
 */
int
TerminalFUPort::operationIndex() const 
    throw (WrongSubclass, InvalidData) {

    if (operation_ != NULL) {
        return opIndex_;
    } else {
        const string msg = 
            "FU port of the terminal is not opcode-setting "
            "and bears no auxiliary 'hint operation' information.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
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
 */
Operation&
TerminalFUPort::hintOperation() const 
    throw (WrongSubclass, InvalidData) {

    if (opcode_ != NULL) {
        return *opcode_;
    } else {
        return NullOperation::instance();
    }
}

void
TerminalFUPort::setOperation(TTAMachine::HWOperation& hwOp) { 
    OperationPool opPool;
    opcode_ = &opPool.operation(hwOp.name().c_str());
    operation_ = &hwOp;
}

/**
 * Return the port.
 *
 * @return The port.
 * @exception WrongSubclass never.
 */
const Port&
TerminalFUPort::port() const 
    throw (WrongSubclass) {
    return port_;
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalFUPort::copy() const {
    return new TerminalFUPort(*this);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool
TerminalFUPort::equals(const Terminal& other) const {

    if (!other.isFUPort()) {
        return false;
    }

    const TerminalFUPort& subclass = 
        dynamic_cast<const TerminalFUPort&>(other);

    // if ports are same
    if (&port_ == &subclass.port_) {

        // we care about opcodes only if they are necessary data
        if (port_.isOpcodeSetting()) {

            // if opcodes are the same
            if (operation_ == subclass.operation_) {
                // same port same operation
                return true;
            } else {
                // if opcodes are not defined -> error
                if (operation_ == NULL ||
                    subclass.operation_ == NULL) {

                    abortWithError(
                        "Opcode _must_ be set for terminals that "
                        "refers opcode setting port!");
                }

                // same port different operations
                return false;
            }
        } else {
            // same ports not opcode setting
            return true;
        }
    }

    // different ports
    return false;
}

/**
 * Return operation index bound to the terminal port for a given operation.
 *
 * Used if operation in terminal is changed.
 *
 * @return Operation index bound to the terminal port for a given operation.
 */
int
TerminalFUPort::findNewOperationIndex() const {
    const FunctionUnit* fu = port_.parentUnit();
    const HWOperation* hwop = fu->operation(operation_->name());
    return hwop->io(dynamic_cast<const FUPort&>(port_));
}


/**
 * Returns a pointer to the HW operation
 * 
 * @return a pointer to the HW operation
 */
HWOperation*
TerminalFUPort::hwOperation() const {
     return operation_; 
}

TCEString
TerminalFUPort::toString() const {
    if (operation_ != NULL) {
        DisassemblyFUOpcodePort disasm(
            port().parentUnit()->name(), port().name(), operation_->name());
        return disasm.toString();
    } else {
        DisassemblyFUPort disasm(port().parentUnit()->name(), port().name());
        return disasm.toString();
    }
}

}
