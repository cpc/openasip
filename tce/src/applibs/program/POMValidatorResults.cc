/**
 * @file POMValidatorResults.cc
 *
 * Implementation of POMValidatorResults class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "POMValidatorResults.hh"

/**
 * The constructor.
 */
POMValidatorResults::POMValidatorResults() {
}


/**
 * The destructor.
 */
POMValidatorResults::~POMValidatorResults() {
}


/**
 * Returns the number of errors.
 *
 * @return The number of errors.
 */
int
POMValidatorResults::errorCount() const {
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
POMValidatorResults::Error
POMValidatorResults::error(int index) const
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
POMValidatorResults::addError(
    POMValidator::ErrorCode code,
    const std::string& errorMsg) {

    Error error(code, errorMsg);
    errors_.push_back(error);
}
