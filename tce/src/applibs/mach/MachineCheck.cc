/**
 * @file MachineCheck.cc
 *
 * Implementation of MachineCheck class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
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
