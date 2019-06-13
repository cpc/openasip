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
 * @file SimProgramBuilder.hh
 *
 * Declaration of SimProgramBuilder class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIM_PROGRAM_BUILDER_HH
#define TTA_SIM_PROGRAM_BUILDER_HH

#include "Exception.hh"

class StateLocator;
class InstructionMemory;
class ExecutableInstruction;
class ExecutableMove;
class MachineState;
class StateData;
class ReadableState;

namespace TTAMachine {
    class Guard;
}

namespace TTAProgram {
    class Instruction;
    class Move;
    class Terminal;
    class Program;
}

/**
 * Creates InstructionMemory out of Program Object Model.
 *
 */
class SimProgramBuilder {
public:
    SimProgramBuilder();
    virtual ~SimProgramBuilder();

    InstructionMemory* build(
        const TTAProgram::Program& prog, MachineState& state);

private:
    /// Copying not allowed.
    SimProgramBuilder(const SimProgramBuilder&);
    /// Assignment not allowed.
    SimProgramBuilder& operator=(const SimProgramBuilder&);

    StateData* processBidirTerminal(
        const TTAProgram::Terminal& theTerminal, MachineState& state);

    const ReadableState& findGuardModel(
        const TTAMachine::Guard& guard, MachineState& state);

    StateData* processSourceTerminal(
        const TTAProgram::Terminal& theTerminal, MachineState& state);

    ExecutableInstruction* processInstruction(
        const TTAProgram::Instruction& instruction, MachineState& state);

    ExecutableMove* processMove(
        const TTAProgram::Move& move, MachineState& state);

    // is the source program a sequential program
    bool sequentialProgram_;
};

#endif
