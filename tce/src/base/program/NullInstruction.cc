/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file NullInstruction.cc
 *
 * Implementation of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullInstruction.hh"
#include "NullInstructionTemplate.hh"
#include "NullProcedure.hh"
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
NullInstruction::parent() const {
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
NullInstruction::addMove(Move&) {
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
NullInstruction::move(int) const {
    abortWithError("addMove()");
    throw OutOfRange(__FILE__,__LINE__,__func__, "Null intruction is empty.");
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::addImmediate(Immediate&) {
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
NullInstruction::immediate(int) {
    abortWithError("addMove()");
    // to avoid warning:
    throw new OutOfRange(__FILE__,__LINE__,__func__,"");
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address&
NullInstruction::address() const {
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
