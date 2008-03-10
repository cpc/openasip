/**
 * @file IDFValidator.hh
 *
 * Declaration of IDFValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IDF_VALIDATOR_HH
#define TTA_IDF_VALIDATOR_HH

#include <string>
#include <vector>

#include "Exception.hh"

namespace IDF {
    class MachineImplementation;
}

namespace TTAMachine {
    class Machine;
}

/**
 * Validates IDF files.
 */
class IDFValidator {
public:
    IDFValidator(
        const IDF::MachineImplementation& idf,
        const TTAMachine::Machine& machine);
    virtual ~IDFValidator();

    bool validate();
    int errorCount() const;
    std::string errorMessage(int index) const
        throw (OutOfRange);

    static void removeUnknownImplementations(
        IDF::MachineImplementation& idf,
        const TTAMachine::Machine& machine);

private:
    /// Vector type for string.
    typedef std::vector<std::string> StringVector;

    void checkFUImplementations();
    void checkRFImplementations();
    void checkIUImplementations();

    /// The machine.
    const TTAMachine::Machine& machine_;
    /// The implementation definition,
    const IDF::MachineImplementation& idf_;
    /// Vector of error messages.
    StringVector errorMessages_;
};

#endif
