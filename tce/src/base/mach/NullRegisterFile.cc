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
 * @file NullRegisterFile.cc
 *
 * Implementation of NullRegisterFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
NullRegisterFile::ensureRegistration(const Component&) const {
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
NullRegisterFile::setMachine(Machine&) {
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
NullRegisterFile::setNumberOfRegisters(int) {
    abortWithError("setNumberOfRegisters()");
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setWidth(int) {
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
NullRegisterFile::setName(const std::string&) {
    abortWithError("setName()");
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxReads(int) {
    abortWithError("setMaxReads()");
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullRegisterFile::setMaxWrites(int) {
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
NullRegisterFile::setGuardLatency(int) {
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
NullRegisterFile::loadState(const ObjectState*) {
    abortWithError("loadState()");
}
}
