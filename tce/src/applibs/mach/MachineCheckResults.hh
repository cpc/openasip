/**
 * @file MachineCheckResults.hh
 *
 * Declaration of MachineCheckResults class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CHECK_RESULTS_HH
#define TTA_MACHINE_CHECK_RESULTS_HH

#include <string>
#include <vector>

#include "Exception.hh"

class MachineCheck;

/**
 * Container for MachineCheck error messages.
 */
class MachineCheckResults {
public:
    /// Typedef for an error (error code + error message).
    typedef std::pair<const MachineCheck*, std::string> Error;

    MachineCheckResults();
    virtual ~MachineCheckResults();

    int errorCount() const;
    Error error(int index) const
        throw (OutOfRange);

    void addError(
        const MachineCheck& check,
        const std::string& errorMsg);

private:
    /// Typedef for vector of errors.
    typedef std::vector<Error> ErrorTable;

    /// Contains the errors.
    ErrorTable errors_;
};

#endif
