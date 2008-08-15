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
 * @file MachineCheckSuite.cc
 *
 * Implementation of MachineCheckSuite class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "MachineCheckSuite.hh"
#include "MachineCheck.hh"
#include "MachineCheckResults.hh"
#include "SequenceTools.hh"

/**
 * The Cosntructor.
 */
MachineCheckSuite::MachineCheckSuite() {
}

/**
 * The Destructor.
 */
MachineCheckSuite::~MachineCheckSuite() {
    SequenceTools::deleteAllItems(checks_);
}

/**
 * Returns number of checks in the suite.
 *
 * @return Suite check count.
 */
int
MachineCheckSuite::checkCount() const {
    return checks_.size();
}

const MachineCheck&
MachineCheckSuite::check(int index)
    throw (OutOfRange) {

    if (index < 0 || index > checkCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return *checks_[index];
}

/**
 * Runs all checks in the check suite.
 *
 * @param machine Machine to check.
 * @param results Result container where error messages are added.
 * @return True, if all checks passed, false otherwise.
 */
bool
MachineCheckSuite::run(
    const TTAMachine::Machine& machine,
    MachineCheckResults& results) {

    bool pass = true;
    for (int i = 0; i < checkCount(); i++) {
        if (!checks_[i]->check(machine, results)) {
            pass = false;
        }
    }
    return pass;
}

/**
 * Adds a check to the suite.
 *
 * @param check Check to add (suite takes ownership of the check object).
 */
void
MachineCheckSuite::addCheck(MachineCheck* check) {
    checks_.push_back(check);
}
