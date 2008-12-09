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
 * @file NullProgram.cc
 *
 * Implementation of NullProgram class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullProgram.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"
#include "NullAddress.hh"
#include "NullAddressSpace.hh"
#include "NullGlobalScope.hh"
#include "NullInstructionReferenceManager.hh"
#include "NullMachine.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullProgram
/////////////////////////////////////////////////////////////////////////////

NullProgram NullProgram::instance_;

/**
 * The constructor.
 */
NullProgram::NullProgram():
    Program(
        NullAddressSpace::instance(), NullAddress::instance()) {
}

/**
 * The destructor.
 */
NullProgram::~NullProgram() {
}

/**
 * Returns an instance of NullProgram class (singleton).
 *
 * @return Singleton instance of NullProgram class.
 */
NullProgram&
NullProgram::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing.
 */
GlobalScope&
NullProgram::globalScope() {
    abortWithError("globalScope()");
    return NullGlobalScope::instance();
}

const GlobalScope&
NullProgram::globalScopeConst() const {
    abortWithError("globalScopeConst()");
    return NullGlobalScope::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing
 */

TTAMachine::Machine&
NullProgram::targetProcessor() const {
    abortWithError("targetProcessor()");
    return TTAMachine::NullMachine::instance();
}
/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address
NullProgram::startAddress() const {
    abortWithError("startAddress()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address
NullProgram::entryAddress() const {
    abortWithError("entryAddress()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullProgram::setEntryAddress(Address) {
    abortWithError("setEntryAddress()");
}

/**
 * Aborts program with error log message.
 *
 * @exception IllegalRegistration never.
 */
void
NullProgram::addProcedure(Procedure&) throw (IllegalRegistration) {
    abortWithError("addProcedure()");
}

/**
 * Aborts program with error log message.
 *
 * @exception IllegalRegistration never.
 */
void
NullProgram::addInstruction(Instruction&)
    throw (IllegalRegistration) {
    abortWithError("addInstruction()");
}

/**
 * Aborts program with error log message.
 */
void
NullProgram::relocate(const Procedure&, UIntWord) {
    abortWithError("relocate()");
}

/**
 * Aborts program with error log message.
 *
 * @exception InstanceNotFound never.
 * @return Nothing.
 */
Procedure&
NullProgram::firstProcedure() const throw (InstanceNotFound) {
    abortWithError("firstProcedure()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 *
 * @exception InstanceNotFound never.
 * @return Nothing.
 */
Procedure&
NullProgram::lastProcedure() const throw (InstanceNotFound) {
    abortWithError("lastProcedure()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing.
 */
Procedure&
NullProgram::nextProcedure(const Procedure&) const
    throw (IllegalRegistration) {

    abortWithError("nextProcedure()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int
NullProgram::procedureCount() const {
    abortWithError("addInstruction()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return a null procedure.
 * @exception OutOfRange never.
 */
Procedure&
NullProgram::procedure(int) const throw (OutOfRange) {
    abortWithError("addInstruction()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 *
 * @exception KeyNotFound never.
 * @return Nothing.
 */
Procedure&
NullProgram::procedure(const std::string&) const throw (KeyNotFound) {
    abortWithError("procedure()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 *
 * @exception InstanceNotFound never.
 * @return Nothing.
 */
Instruction&
NullProgram::firstInstruction() const throw (InstanceNotFound) {
    abortWithError("firstInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 * @exception KeyNotFound never.
 */
const Instruction&
NullProgram::instructionAt(UIntWord) const throw (KeyNotFound) {
    abortWithError("instructionAt()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 */
const Instruction&
NullProgram::nextInstruction(const Instruction&) const
    throw (IllegalRegistration) {

    abortWithError("nextInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @exception InstanceNotFound never.
 * @return Nothing.
 */
Instruction&
NullProgram::lastInstruction() const throw (InstanceNotFound) {
    abortWithError("lastInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing.
 */
InstructionReferenceManager&
NullProgram::instructionReferenceManager() {
    abortWithError("instructionReferenceManager()");
    return NullInstructionReferenceManager::instance();
}

}
