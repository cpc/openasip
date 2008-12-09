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
 * @file NullGuard.cc
 *
 * Implementation of NullGuard class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullGuard.hh"
#include "Bus.hh"

namespace TTAMachine {

Bus NullGuard::bus_("NULL", 1, 0, Machine::ZERO);
NullGuard NullGuard::instance_;

/**
 * The constructor.
 */
NullGuard::NullGuard() : Guard(false, bus_) {
}


/**
 * The destructor.
 */
NullGuard::~NullGuard() {
}


/**
 * Returns the only instance of NullGuard.
 *
 * @return The instance.
 */
NullGuard&
NullGuard::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Bus*
NullGuard::parentBus() const {
    abortWithError("NullGuard::parentBus");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isEqual(const Guard&) const {
    abortWithError("NullGuard::isEqual");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isInverted() const {
    abortWithError("NullGuard::isInverted");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isMoreRestrictive(const Guard&) const {
    abortWithError("NullGuard::isMoreRestrictive");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isLessRestrictive(const Guard&) const {
    abortWithError("NullGuard::isLessRestrictive");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullGuard::isDisjoint(const Guard&) const {
    abortWithError("NullGuard::isDisjoint");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullGuard::saveState() const {
    abortWithError("NullGuard::saveState");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullGuard::loadState(const ObjectState*) 
    throw (ObjectStateLoadingException) {

    abortWithError("NullGuard::loadState");
}

}
