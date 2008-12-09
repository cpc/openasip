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
 * @file NullImmediateUnit.cc
 *
 * Implementation of NullImmediateUnit class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "NullImmediateUnit.hh"

namespace TTAMachine {

NullImmediateUnit NullImmediateUnit::instance_;

/**
 * The constructor.
 */
NullImmediateUnit::NullImmediateUnit() :
    ImmediateUnit("NULL", 1, 1, 1, 0, Machine::ZERO) {
}


/**
 * The destructor.
 */
NullImmediateUnit::~NullImmediateUnit() {
}


/**
 * Returns the only instance of NullImmediateUnit.
 *
 * @return The only NullImmediateUnit instance.
 */
NullImmediateUnit&
NullImmediateUnit::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullImmediateUnit::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullImmediateUnit::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::hasPort(const std::string&) const {
    abortWithError("hasPort()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::portCount() const {
    abortWithError("portCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::maxReads() const {
    abortWithError("maxReads()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::maxWrites() const {
    abortWithError("maxWrites()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
RegisterFile::Type
NullImmediateUnit::type() const {
    abortWithError("type()");
    return RegisterFile::NORMAL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isNormal() const {
    abortWithError("isNormal()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isVolatile() const {
    abortWithError("isVolatile()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullImmediateUnit::isReserved() const {
    abortWithError("isReserved()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullImmediateUnit::name() const {
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
NullImmediateUnit::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine::Extension
NullImmediateUnit::extensionMode() const {
    abortWithError("extensionMode()");
    return Machine::ZERO;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setMaxReads(int)
    throw (OutOfRange) {

    abortWithError("setMaxReads()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setMaxWrites(int)
    throw (OutOfRange) {

    abortWithError("setMaxWrites()");
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::setType(RegisterFile::Type) {
    abortWithError("setType()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::guardLatency() const {
    abortWithError("guardLatency()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setGuardLatency(int)
    throw (OutOfRange) {

    abortWithError("setGuardLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::latency() const {
    abortWithError("latency()");
    return 0;
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::setExtensionMode(Machine::Extension) {
    abortWithError("setExtensionMode()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setLatency(int)
    throw (OutOfRange) {

    abortWithError("setLatency()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::numberOfRegisters() const {
    abortWithError("numberOfRegisters()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullImmediateUnit::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setNumberOfRegisters(int)
    throw (OutOfRange) {

    abortWithError("setNumberOfRegisters()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullImmediateUnit::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullImmediateUnit::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}


/**
 * Aborts the program with error message.
 */
void
NullImmediateUnit::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullImmediateUnit::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullImmediateUnit::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
