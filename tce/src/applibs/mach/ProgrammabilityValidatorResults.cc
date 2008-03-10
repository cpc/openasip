/**
 * @file ProgrammabilityValidatorResults.cc
 *
 * Implementation of ProgrammabilityValidatorResults class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "ProgrammabilityValidatorResults.hh"


/**
 * The constructor.
 */
ProgrammabilityValidatorResults::ProgrammabilityValidatorResults() {
}


/**
 * The destructor.
 */
ProgrammabilityValidatorResults::~ProgrammabilityValidatorResults() {
}


/**
 * Returns the number of errors.
 *
 * @return The number of errors.
 */
int
ProgrammabilityValidatorResults::errorCount() const {
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
ProgrammabilityValidatorResults::Error
ProgrammabilityValidatorResults::error(int index) const
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
ProgrammabilityValidatorResults::addError(
    ProgrammabilityValidator::ErrorCode code,
    const std::string& errorMsg) {

    Error error(code, errorMsg);
    errors_.push_back(error);
}
