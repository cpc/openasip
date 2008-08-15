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
 * @file NullPort.cc
 *
 * Implementation of NullPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
