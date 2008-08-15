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
 * @file NullInstruction.cc
 *
 * Implementation of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullInstruction.hh"
#include "NullInstructionTemplate.hh"
#include "NullProcedure.hh"
#include "NullImmediate.hh"
#include "NullMove.hh"
#include "NullAddress.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullInstruction
/////////////////////////////////////////////////////////////////////////////

NullInstruction NullInstruction::instance_;

/**
 * The constructor.
 */
NullInstruction::NullInstruction() :
    Instruction(TTAMachine::NullInstructionTemplate::instance()) {
}

/**
 * The destructor.
 */
NullInstruction::~NullInstruction() {
}


/**
 * Returns an instance of NullInstruction class (singleton).
 *
 * @return Singleton instance of NullInstruction class.
 */
NullInstruction&
NullInstruction::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null procedure.
 * @exception IllegalRegistration never.
 */
Procedure&
NullInstruction::parent() const throw (IllegalRegistration) {
    abortWithError("parent()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::setParent(const Procedure&) {
    abortWithError("setParent()");
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullInstruction::isInProcedure() const {
    abortWithError("isInProcedure()");
    return false;
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::addMove(Move&)
    throw (ObjectAlreadyExists) {

    abortWithError("addMove()");
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int NullInstruction::moveCount() {
    abortWithError("addMove()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return A null move.
 */
const Move&
NullInstruction::move(int) const throw (OutOfRange) {
    abortWithError("addMove()");
    return NullMove::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::addImmediate(Immediate&)
    throw (ObjectAlreadyExists) {
    abortWithError("addMove()");
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int NullInstruction::immediateCount() {
    abortWithError("addMove()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return A null immediate.
 */
Immediate&
NullInstruction::immediate(int) throw (OutOfRange) {
    abortWithError("addMove()");
    return NullImmediate::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address&
NullInstruction::address() const throw (IllegalRegistration) {
    abortWithError("address()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int
NullInstruction::size() const {
    abortWithError("size()");
    return -1;
}

}
