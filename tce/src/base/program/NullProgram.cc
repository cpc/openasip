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
