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
 * @file MachineCheck.cc
 *
 * Implementation of MachineCheck class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MachineCheck.hh"


/**
 * The Constructor.
 *
 * @param shortDesc The short description of the check.
 */
MachineCheck::MachineCheck(const std::string& shortDesc):
    shortDesc_(shortDesc) {
}

/**
 * The Destructor.
 */
MachineCheck::~MachineCheck() {
}


/**
 * Returns short description of the checker.
 */
std::string
MachineCheck::shortDescription() const {
    return shortDesc_;
}


/**
 * Returns description of the checker.
 *
 * Base class implementation returns the checker short description
 * as the long description.
 */
std::string
MachineCheck::description() const {
    return shortDescription();
}

/**
 * Returns true if the checker can automatically fix the machine to pass
 * the check.
 *
 * @return Base class implementation returns always false.
 */
bool
MachineCheck::canFix(const TTAMachine::Machine&) const {
    return false;
}

/**
 * Automatically fixes the machine to pass the test.
 *
 * The base class implementation always throws InvalidObject.
 *
 * @return Short description of the modifications to the machine.
 * @throw InvalidData If the machine can't be fixed automatically.
 */
std::string
MachineCheck::fix(TTAMachine::Machine&) const
    throw (InvalidData) {

    throw InvalidData(__FILE__, __LINE__, __func__);
}
