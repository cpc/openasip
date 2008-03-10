/**
 * @file MachineCheckResults.cc
 *
 * Implementation of MachineCheckResults class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "MachineCheckResults.hh"
#include "MachineCheck.hh"

/**
 * The constructor.
 */
MachineCheckResults::MachineCheckResults() {
}


/**
 * The destructor.
 */
MachineCheckResults::~MachineCheckResults() {
}


/**
 * Returns the number of errors.
 *
 * @return The number of errors.
 */
int
MachineCheckResults::errorCount() const {
    return errors_.size();
}


/**
 * Returns an error by the given index.
 *
 * @param index The index.
 * @return The error.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of errors.
 */
MachineCheckResults::Error
MachineCheckResults::error(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= errorCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return errors_[index];
}


/**
 * Adds an error to the results.
 *
 * @param code The error code.
 * @param errorMsg The error message.
 */
void
MachineCheckResults::addError(
    const MachineCheck& check,
    const std::string& errorMsg) {

    Error error(&check, errorMsg);
    errors_.push_back(error);
}
