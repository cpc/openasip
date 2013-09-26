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
NullProcedure::setParent(Program&) {
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
