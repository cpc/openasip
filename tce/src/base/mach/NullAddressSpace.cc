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
 * @file NullAddressSpace.cc
 *
 * Implementation of NullAddressSpace class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullAddressSpace.hh"

namespace TTAMachine {

Machine NullAddressSpace::machine_;
NullAddressSpace NullAddressSpace::instance_;

/**
 * The constructor.
 */
NullAddressSpace::NullAddressSpace() :
    AddressSpace("NULL", 1, 0, 1, machine_) {
}


/**
 * The destructor.
 */
NullAddressSpace::~NullAddressSpace() {
}


/**
 * Returns the only instance of NullAddressSpace.
 *
 * @return The only instance of NullAddressSpace.
 */
NullAddressSpace&
NullAddressSpace::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullAddressSpace::width() const {
    abortWithError("width()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
unsigned int
NullAddressSpace::start() const {
    abortWithError("start()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
unsigned int
NullAddressSpace::end() const {
    abortWithError("end()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullAddressSpace::name() const {
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
NullAddressSpace::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setWidth(int)
    throw (OutOfRange) {

    abortWithError("setWidth()");
}


/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setAddressBounds(unsigned int, unsigned int)
    throw (OutOfRange) {

    abortWithError("setAddressBounds()");
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullAddressSpace::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}

/**
 * Aborts the program with error message.
 */
void
NullAddressSpace::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullAddressSpace::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullAddressSpace::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullAddressSpace::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullAddressSpace::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullAddressSpace::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
