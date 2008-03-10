/**
 * @file SimProgramBuilder.hh
 *
 * Declaration of SimProgramBuilder class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIM_PROGRAM_BUILDER_HH
#define TTA_SIM_PROGRAM_BUILDER_HH

#include "Exception.hh"
#include "Program.hh"

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
        const TTAProgram::Program& prog, 
        MachineState& state)
        throw (IllegalProgram);

private:
    /// Copying not allowed.
    SimProgramBuilder(const SimProgramBuilder&);
    /// Assignment not allowed.
    SimProgramBuilder& operator=(const SimProgramBuilder&);

    StateData* processBidirTerminal(
        const TTAProgram::Terminal& theTerminal, 
        MachineState& state) throw (IllegalProgram);

    const ReadableState& findGuardModel(
        const TTAMachine::Guard& guard, MachineState& state) 
        throw (IllegalProgram);
    
    StateData* processSourceTerminal(
        const TTAProgram::Terminal& theTerminal, 
        MachineState& state) throw (IllegalProgram);

    ExecutableInstruction* processInstruction(
        const TTAProgram::Instruction& instruction, 
        MachineState& state) 
        throw (IllegalProgram);

    ExecutableMove* processMove(
        const TTAProgram::Move& move, 
        MachineState& state) 
        throw (IllegalProgram);

    // is the source program a sequential program
    bool sequentialProgram_;
};

#endif
