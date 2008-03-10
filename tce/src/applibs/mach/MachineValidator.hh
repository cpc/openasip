/**
 * @file MachineValidator.hh
 *
 * Declaration of MachineValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_VALIDATOR_HH
#define TTA_MACHINE_VALIDATOR_HH

#include <set>
#include <string>


namespace TTAMachine {
    class Machine;
    class FunctionUnit;
}

class MachineValidatorResults;

/**
 * Validator of target architecture definitions (machines). It tests a given
 * machine against various criteria (completeness, implementation-dependent
 * restrictions to structure, synthesisability).
 */
class MachineValidator {
public:
    /// Error codes for different errors.
    enum ErrorCode {
        /// GCU missing in machine.
        GCU_MISSING,
        /// Address space missing in GCU.
        GCU_AS_MISSING,
        /// Pipeline uses an IO which is not bound.
        USED_IO_NOT_BOUND,
        /// JUMP and CALL uses different port in GCU.
        DIFFERENT_PORT_FOR_JUMP_AND_CALL,
        /// PC port missing in GCU.
        PC_PORT_MISSING,
        /// RA port missing in GCU.
        RA_PORT_MISSING,
        /// RA and PC ports have unequal width.
        PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH,
        /// Instruction memory address width differs from PC/RA port width.
        IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC
    };

    MachineValidator(const TTAMachine::Machine& machine);
    virtual ~MachineValidator();

    MachineValidatorResults* validate(
        const std::set<ErrorCode>& errorsToCheck) const;

private:
    void checkGCUExists(MachineValidatorResults& results) const;
    void checkGCUHasAddressSpace(MachineValidatorResults& results) const;
    void checkOperandBindings(MachineValidatorResults& results) const;
    void checkJumpAndCallOperandBindings(
        MachineValidatorResults& results) const;
    void checkProgramCounterPort(MachineValidatorResults& results) const;
    void checkReturnAddressPort(MachineValidatorResults& results) const;
    void checkRAPortHasSameWidthAsPCPort(
        MachineValidatorResults& results) const;
    void checkIMemAddrWidth(MachineValidatorResults& results) const;

    /// The machine to validate.
    const TTAMachine::Machine& machine_;
};

#endif
