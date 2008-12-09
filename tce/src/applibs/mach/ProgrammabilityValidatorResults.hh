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
 * @file ProgrammabilityValidatorResults.hh
 *
 * Declaration of ProgrammabilityValidatorResults class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAMMABILITY_VALIDATOR_RESULTS_HH
#define TTA_PROGRAMMABILITY_VALIDATOR_RESULTS_HH

#include <string>
#include <vector>

#include "ProgrammabilityValidator.hh"
#include "Exception.hh"


/**
 * This class bundles up the results of ProgrammabilityValidator.
 */
class ProgrammabilityValidatorResults {
public:
    /// Typedef for an error (error code + error message).
    typedef std::pair<ProgrammabilityValidator::ErrorCode,
                      std::string> Error;

    ProgrammabilityValidatorResults();
    virtual ~ProgrammabilityValidatorResults();

    int errorCount() const;
    Error error(int index) const
        throw (OutOfRange);

    void addError(
        ProgrammabilityValidator::ErrorCode code,
        const std::string& errorMsg);

private:
    /// Typedef for vector of errors.
    typedef std::vector<Error> ErrorTable;

    /// Contains the errors.
    ErrorTable errors_;
};

#endif
