/**
 * @file MachineValidatorResults.hh
 *
 * Declaration of MachineValidatorResults class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
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
