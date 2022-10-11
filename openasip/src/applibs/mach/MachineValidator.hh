/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MachineValidator.hh
 *
 * Declaration of MachineValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
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
        IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC,
        /// FU has no operations with a trigger
        FU_NO_VALID_OPERATIONS,
        /// FU is missing ports
        FU_PORT_MISSING
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
    void checkFUConnections(MachineValidatorResults& results) const;

    /// The machine to validate.
    const TTAMachine::Machine& machine_;
};

#endif
