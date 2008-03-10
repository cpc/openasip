/**
 * @file POMValidatorResults.hh
 *
 * Declaration of POMValidatorResults class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_POM_VALIDATOR_RESULTS_HH
#define TTA_POM_VALIDATOR_RESULTS_HH

#include <string>
#include <vector>

#include "POMValidator.hh"
#include "Exception.hh"

/**
 * Class for storing POMValidator results.
 */
class POMValidatorResults {
public:
    typedef std::pair<POMValidator::ErrorCode, std::string> Error;

    POMValidatorResults();
    virtual ~POMValidatorResults();

    int errorCount() const;
    Error error(int index) const
        throw (OutOfRange);

    void addError(POMValidator::ErrorCode code, const std::string& errorMsg);

private:
    /// Vector storing the errors.
    std::vector<Error> errors_;
};

#endif
