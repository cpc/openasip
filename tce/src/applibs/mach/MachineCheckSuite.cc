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
