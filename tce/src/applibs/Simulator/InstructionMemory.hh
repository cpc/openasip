/**
 * @file InstructionMemory.hh
 *
 * Declaration of InstructionMemory class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_MEMORY_HH
#define TTA_INSTRUCTION_MEMORY_HH

#include <vector>
#include "SimulatorConstants.hh"
#include "Exception.hh"

class ExecutableInstruction;

/**
 * Container for ExecutableInstructions.
 */
class InstructionMemory {
public:
    InstructionMemory(InstructionAddress startAddress);
    virtual ~InstructionMemory();
    
    void addExecutableInstruction(ExecutableInstruction* instruction);
    ExecutableInstruction& instructionAt(InstructionAddress address) 
        throw (OutOfRange);
    const ExecutableInstruction& instructionAtConst(
        InstructionAddress address) const
        throw (OutOfRange);

    void resetExecutionCounts();

private:
    /// Copying not allowed.
    InstructionMemory(const InstructionMemory&);
    /// Assignment not allowed.
    InstructionMemory& operator=(const InstructionMemory&);

    /// Container for instructions.
    typedef std::vector<ExecutableInstruction*> InstructionContainer;

    /// The starting address of the instruction memory address space.
    InstructionAddress startAddress_;

    /// All the instructions of the memory.
    InstructionContainer instructions_;
};

#include  "InstructionMemory.icc"

#endif
