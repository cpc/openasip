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
 * @file NullTerminal.cc
 *
 * Implementation of NullTerminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullTerminal.hh"
#include "Application.hh"
#include "NullRegisterFile.hh"
#include "NullImmediateUnit.hh"
#include "NullFunctionUnit.hh"
#include "Operation.hh"
#include "NullPort.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullTerminal
/////////////////////////////////////////////////////////////////////////////

NullTerminal NullTerminal::instance_;

/**
 * The constructor.
 */
NullTerminal::NullTerminal() :
    Terminal() {
}

/**
 * The destructor.
 */
NullTerminal::~NullTerminal() {
}

/**
 * Returns an instance of NullTerminal class (singleton).
 *
 * @return Singleton instance of NullTerminal class.
 */
NullTerminal&
NullTerminal::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullTerminal::isImmediate() const {
    abortWithError("isImmediate()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullTerminal::isImmediateRegister() const {
    abortWithError("isImmediate()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullTerminal::isGPR() const {
    abortWithError("iGPR()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullTerminal::isFUPort() const {
    abortWithError("isFUPort()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullTerminal::isOpcodeSetting() const throw (WrongSubclass) {
    abortWithError("isOpcodeSetting()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return A null SimValue.
 */
SimValue
NullTerminal::value() const throw(WrongSubclass) {

    abortWithError("value()");
    return NullSimValue::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null register file.
 */
const RegisterFile&
NullTerminal::registerFile() const throw (WrongSubclass) {

    abortWithError("registerFile()");
    return NullRegisterFile::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null immediate unit.
 */
const ImmediateUnit&
NullTerminal::immediateUnit() const throw (WrongSubclass) {

    abortWithError("immediateUnit()");
    return NullImmediateUnit::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null function unit.
 */
const FunctionUnit&
NullTerminal::functionUnit() const throw (WrongSubclass) {

    abortWithError("functionUnit()");
    return NullFunctionUnit::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return 0.
 */
int
NullTerminal::index() const throw (WrongSubclass) {
    abortWithError("index()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return A null operation.
 */
Operation&
NullTerminal::operation() const throw (InvalidData) {

    abortWithError("operation()");
    return NullOperation::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null port.
 */
const Port&
NullTerminal::port() const throw (WrongSubclass) {
    abortWithError("port()");
    return NullPort::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullTerminal::setIndex(int)
    throw (OutOfRange) {

    abortWithError("setIndex()");
}

/**
 * Aborts program with error log message.
 */
void
NullTerminal::setOperation(Operation&)
    throw (WrongSubclass) {

    abortWithError("setOperation()");
}

/**
 * Aborts program with error log message.
 */
void
NullTerminal::setInstructionReference(InstructionReference&)
    throw(WrongSubclass) {
    abortWithError("setInstructionReference()");
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Terminal*
NullTerminal::copy() const {
    abortWithError("copy()");
    return &NullTerminal::instance();
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool
NullTerminal::equals(const Terminal& other) const {
    return &other == this;
}

}
