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
 * @file NullRegisterFile.cc
 *
 * Implementation of NullRegisterFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullRegisterFile.hh"

namespace TTAMachine {

NullRegisterFile NullRegisterFile::instance_;

/**
 * The constructor.
 */
NullRegisterFile::NullRegisterFile() :
    RegisterFile("NULL", 1, 1, 1, 1, 0, RegisterFile::NORMAL) {
}


/**
 * The destructor.
 */
NullRegisterFile::~NullRegisterFile() {
}


/**
 * Returns the only instance of NullRegisterFile.
 */
NullRegisterFile&
NullRegisterFile::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullRegisterFile::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullRegisterFile::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::hasPort(const std::string&) const {
    abortWithError("hasPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::portCount() const {
    abortWithError("portCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullRegisterFile::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine");
}


/**
 * Aborts the program with error message.
 */
void
NullRegisterFile::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::numberOfRegisters() const {
    abortWithError("numberOfRegisters()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setNumberOfRegisters(int)
    throw (OutOfRange) {

    abortWithError("setNumberOfRegisters()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::maxReads() const {
    abortWithError("maxReads()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::maxWrites() const {
    abortWithError("maxWrites()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
RegisterFile::Type
NullRegisterFile::type() const {
    abortWithError("type()");
    return RegisterFile::NORMAL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isNormal() const {
    abortWithError("isNormal()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isVolatile() const {
    abortWithError("isVolatile()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullRegisterFile::isReserved() const {
    abortWithError("isReserved()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullRegisterFile::name() const {
    abortWithError("name()");
    return "";
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
NullRegisterFile::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxReads(int)
    throw (OutOfRange) {

    abortWithError("setMaxReads()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxWrites(int)
    throw (OutOfRange) {

    abortWithError("setMaxWrites()");
}


/**
 * Aborts the program with error message.
 */
void
NullRegisterFile::setType(RegisterFile::Type) {
    abortWithError("setType()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullRegisterFile::guardLatency() const {
    abortWithError("guardLatency()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setGuardLatency(int)
    throw (OutOfRange) {

    abortWithError("setGuardLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullRegisterFile::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullRegisterFile::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}
}
