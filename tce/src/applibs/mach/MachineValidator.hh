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
