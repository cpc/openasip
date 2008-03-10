/**
 * @file POMValidator.hh
 *
 * Declaration of POMValidator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating red
 */

#ifndef TTA_POM_VALIDATOR_HH
#define TTA_POM_VALIDATOR_HH

#include <set>

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

class POMValidatorResults;

/**
 * POMValidator validates a program object model against a target
 * processor architecture using varoius criteria.
 */
class POMValidator {
public:

    /// Error codes for different errors.
    enum ErrorCode {
        /// Connection required for a move is missing.
        CONNECTION_MISSING,
        /// Instruction template missing for a long immediate.
        LONG_IMMEDIATE_NOT_SUPPORTED,
        /// Program contains operations with unknown behaviour.
        SIMULATION_NOT_POSSIBLE,
        /// Compiled simulation is not possible
        COMPILED_SIMULATION_NOT_POSSIBLE
    };
        
    POMValidator(
        const TTAMachine::Machine& mach, const TTAProgram::Program& program);

    virtual ~POMValidator();

    POMValidatorResults* validate(const std::set<ErrorCode>& errorsToCheck);

private:
    void checkConnectivity(POMValidatorResults& results);
    void checkLongImmediates(POMValidatorResults& results);
    void checkSimulatability(POMValidatorResults& results);
    void checkCompiledSimulatability(POMValidatorResults& results);

    /// The machine to validate the program against.
    const TTAMachine::Machine& machine_;
    /// The program to validate.
    const TTAProgram::Program& program_;
    
};

#endif
