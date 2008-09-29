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
 * @file ProgrammabilityValidatorResults.cc
 *
 * Implementation of ProgrammabilityValidatorResults class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
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
