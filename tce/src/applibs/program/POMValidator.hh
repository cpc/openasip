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
 * @file POMValidator.hh
 *
 * Declaration of POMValidator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating red
 */

#ifndef TTA_POM_VALIDATOR_HH
#define TTA_POM_VALIDATOR_HH

#include <set>
#include "Program.hh"

namespace TTAMachine {
    class Machine;
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
    /// The program's instructions in a quickly accessed vector.
    const TTAProgram::Program::InstructionVector instructions_;
    
};

#endif
