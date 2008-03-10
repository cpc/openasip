/**
 * @file ProgrammabilityValidatorResults.hh
 *
 * Declaration of ProgrammabilityValidatorResults class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
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
