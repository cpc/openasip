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
 * @file MachineValidatorResults.hh
 *
 * Declaration of MachineValidatorResults class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_VALIDATOR_RESULTS_HH
#define TTA_MACHINE_VALIDATOR_RESULTS_HH

#include <string>
#include <vector>

#include "MachineValidator.hh"
#include "Exception.hh"

/**
 * This class bundles up the results of MachineValidator.
 */
class MachineValidatorResults {
public:
    /// Typedef for an error (error code + error message).
    typedef std::pair<MachineValidator::ErrorCode, std::string> Error;

    MachineValidatorResults();
    virtual ~MachineValidatorResults();

    int errorCount() const;
    Error error(int index) const
        throw (OutOfRange);

    void addError(
        MachineValidator::ErrorCode code,
        const std::string& errorMsg);

private:
    /// Typedef for vector of errors.
    typedef std::vector<Error> ErrorTable;

    /// Contains the errors.
    ErrorTable errors_;
};

#endif
