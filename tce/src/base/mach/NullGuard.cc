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
