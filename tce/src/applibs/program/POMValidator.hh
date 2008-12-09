/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
