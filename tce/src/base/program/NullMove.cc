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
 * @file NullMove.cc
 *
 * Implementation of NullMove class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
