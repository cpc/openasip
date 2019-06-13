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
 * @file NullAddressSpace.cc
 *
 * Implementation of NullAddressSpace class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "NullAddressSpace.hh"
#include "Machine.hh"

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
NullAddressSpace::setName(const std::string&) {
    abortWithError("setName()");
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setWidth(int) {
    abortWithError("setWidth()");
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never thrown.
 */
void
NullAddressSpace::setAddressBounds(unsigned int, unsigned int) {
    abortWithError("setAddressBounds()");
}

/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullAddressSpace::setMachine(Machine&) {
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
NullAddressSpace::ensureRegistration(const Component&) const {
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
NullAddressSpace::loadState(const ObjectState*) {
    abortWithError("loadState()");
}
}
