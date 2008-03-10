/**
 * @file FUValidator.hh
 *
 * Declaration of FUValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_VALIDATOR_HH
#define TTA_FU_VALIDATOR_HH

namespace TTAMachine {
    class FunctionUnit;
}

class MachineValidatorResults;

/**
 * Validator for function units.
 */
class FUValidator {
public:
    static void checkOperandBindings(
        const TTAMachine::FunctionUnit& fu,
        MachineValidatorResults& results);

private:
    FUValidator();
};

#endif
