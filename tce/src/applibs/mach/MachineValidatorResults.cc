/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MachineValidatorResults.cc
 *
 * Implementation of MachineValidatorResults class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "MachineValidatorResults.hh"


/**
 * The constructor.
 */
MachineValidatorResults::MachineValidatorResults() {
}


/**
 * The destructor.
 */
MachineValidatorResults::~MachineValidatorResults() {
}


/**
 * Returns the number of errors.
 *
 * @return The number of errors.
 */
int
MachineValidatorResults::errorCount() const {
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
MachineValidatorResults::Error
MachineValidatorResults::error(int index) const
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
MachineValidatorResults::addError(
    MachineValidator::ErrorCode code,
    const std::string& errorMsg) {

    Error error(code, errorMsg);
    errors_.push_back(error);
}
