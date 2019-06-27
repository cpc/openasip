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
 * @file MachineCheckSuite.cc
 *
 * Implementation of MachineCheckSuite class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
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
MachineCheckSuite::check(int index) {
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
