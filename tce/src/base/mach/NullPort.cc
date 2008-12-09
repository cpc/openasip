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
 * @file NullPort.cc
 *
 * Implementation of NullPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "NullPort.hh"
#include "RegisterFile.hh"

namespace TTAMachine {

RegisterFile NullPort::regFile_(
    "dummy", 1, 1, 1, 1, 0, RegisterFile::NORMAL);
NullPort NullPort::instance_;

/**
 * The constructor.
 */
NullPort::NullPort() : Port("NULL", regFile_) {
}


/**
 * The destructor.
 */
NullPort::~NullPort() {
}


/**
 * Returns the only instance of NullPort.
 *
 * @return The only instance of NullPort.
 */
NullPort&
NullPort::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullPort::name() const {
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
NullPort::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullPort::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception IllegalConnectivity Never thrown.
 */
void
NullPort::attachSocket(Socket&)
    throw (IllegalRegistration, ComponentAlreadyExists,
           IllegalConnectivity) {

    abortWithError("attachSocket()");
}


/**
 * Aborts the program with error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullPort::detachSocket(Socket&)
    throw (InstanceNotFound) {

    abortWithError("detachSocket()");
}


/**
 * Aborts the program with error message.
 */
void
NullPort::detachAllSockets() {
    abortWithError("detachAllSockets()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Socket*
NullPort::inputSocket() const {
    abortWithError("inputSocket()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Socket*
NullPort::outputSocket() const {
    abortWithError("outputSocket()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never thrown.
 */
Socket*
NullPort::unconnectedSocket(int) const
    throw (OutOfRange) {

    abortWithError("unconnectedSocket()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullPort::socketCount() const {
    abortWithError("socketCount()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullPort::isConnectedTo(const Socket&) const {
    abortWithError("isConnectedTo()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullPort::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullPort::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
