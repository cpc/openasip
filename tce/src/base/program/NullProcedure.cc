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
 * @file NullProcedure.cc
 *
 * Implementation of NullProcedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullProcedure.hh"
#include "NullProgram.hh"
#include "NullInstruction.hh"
#include "NullAddress.hh"
#include "NullAddressSpace.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullProcedure
/////////////////////////////////////////////////////////////////////////////

NullProcedure NullProcedure::instance_;

/**
 * The constructor.
 */
NullProcedure::NullProcedure() :
    Procedure("NULL", NullAddressSpace::instance()) {
}

/**
 * The destructor.
 */
NullProcedure::~NullProcedure() {
}

/**
 * Returns an instance of NullProcedure class (singleton).
 *
 * @return Singleton instance of NullProcedure class.
 */
NullProcedure&
NullProcedure::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null program.
 * @exception IllegalRegistration never.
 */
Program&
NullProcedure::parent() const throw (IllegalRegistration) {
    abortWithError("parent()");
    return NullProgram::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullProcedure::setParent(const Program&) {
    abortWithError("setParent()");
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullProcedure::isInProgram() const {
    abortWithError("isInProgram()");
    return false;
}

std::string
NullProcedure::name() const {
    abortWithError("name()");
    return "";
}

int
NullProcedure::alignment() const {
    abortWithError("alignment()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 * @exception IllegalRegistration never.
 */
Address
NullProcedure::address(const Instruction&) const
    throw (IllegalRegistration) {

    abortWithError("address()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address
NullProcedure::startAddress() const {
    abortWithError("startAddress()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullProcedure::setStartAddress(Address) {
    abortWithError("setStartAddress()");
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address
NullProcedure::endAddress() const {
    abortWithError("endAddress()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int
NullProcedure::instructionCount() const {
    abortWithError("instructionCount()");
    return -1;
}

Instruction&
NullProcedure::firstInstruction() const throw (InstanceNotFound) {
    abortWithError("firstInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 * @exception KeyNotFound never.
 */
Instruction&
NullProcedure::instructionAt(UIntWord) const
    throw (KeyNotFound) {

    abortWithError("instructionAt()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 * @exception IllegalRegistration never.
 */
bool
NullProcedure::hasNextInstruction(const Instruction&) const
    throw (IllegalRegistration) {
    abortWithError("hasNextInstruction()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 * @exception IllegalRegistration never.
 */
Instruction&
NullProcedure::nextInstruction(const Instruction&) const
    throw (IllegalRegistration) {

    abortWithError("nextInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 * @exception IllegalRegistration never.
 */
Instruction&
NullProcedure::previousInstruction(const Instruction&) const
    throw (IllegalRegistration) {

    abortWithError("previousInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 * @exception IllegalRegistration never.
 */
Instruction&
NullProcedure::lastInstruction() const throw (IllegalRegistration) {
    abortWithError("lastInstruction()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 *
 * @exception IllegalRegistration never.
 */
void
NullProcedure::addInstruction(Instruction&) throw (IllegalRegistration) {
    abortWithError("addInstruction()");
}

/**
 * Aborts program with error log message.
 *
 * @exception IllegalRegistration never.
 */
void
NullProcedure::insertInstructionAfter(
    const Instruction&, Instruction*) throw (IllegalRegistration) {

    abortWithError("insertInstructionAfter()");
}

}
