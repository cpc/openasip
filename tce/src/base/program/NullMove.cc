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
 * @file NullMove.cc
 *
 * Implementation of NullMove class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullMove.hh"
#include "NullMoveGuard.hh"
#include "NullTerminal.hh"
#include "NullInstruction.hh"
#include "NullBus.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullMove
/////////////////////////////////////////////////////////////////////////////

NullMove NullMove::instance_;

/**
 * The constructor.
 */
NullMove::NullMove() :
    Move(
        &NullTerminal::instance(), &NullTerminal::instance(),
        NullBus::instance() ) {
}

/**
 * The destructor.
 */
NullMove::~NullMove() {
}

/**
 * Returns an instance of NullMove class (singleton).
 *
 * @return Singleton instance of NullMove class.
 */
NullMove&
NullMove::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null instruction.
 */
Instruction&
NullMove::parent() const throw (IllegalRegistration) {
    abortWithError("parent()");
    return NullInstruction::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullMove::setParent(Instruction&) {
    abortWithError("setParent()");
}

/**
 * Aborts program with error log message.
 *
 * @return false.
 */
bool
NullMove::isInInstruction() const {
    abortWithError("isInInstruction()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return false.
 */
bool
NullMove::isUnconditional() const {
    abortWithError("isUnconditional()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Terminal&
NullMove::source() const {
    abortWithError("source()");
    return NullTerminal::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return false.
 */
void
NullMove::setSource(Terminal*) {
    abortWithError("setSource()");
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Terminal&
NullMove::destination() const {
    abortWithError("destination()");
    return NullTerminal::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullMove::setDestination(Terminal*) {
    abortWithError("setDestination()");
}

/**
 * Aborts program with error log message.
 *
 * @return A null guard.
 */
MoveGuard&
NullMove::guard() const throw (InvalidData) {
    abortWithError("guard()");
    return NullMoveGuard::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullMove::setGuard(MoveGuard*) {
    abortWithError("setGuard()");
}

/**
 * Aborts program with error log message.
 *
 * @return A null bus.
 */
Bus&
NullMove::bus() const {
    abortWithError("bus()");
    return NullBus::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return false.
 */
void
NullMove::setBus(TTAMachine::Bus&) {
    abortWithError("setBus()");
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing.
 */
Socket&
NullMove::destinationSocket() const {
    abortWithError("destinationSocket()");
    throw 1;
}

/**
 * Aborts program with error log message.
 *
 * @return Nothing.
 */
Socket&
NullMove::sourceSocket() const throw (WrongSubclass) {
    abortWithError("sourceSocket()");
    throw 1;
}

}
