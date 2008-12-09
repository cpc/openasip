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
 * @file InstructionMemory.hh
 *
 * Declaration of InstructionMemory class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_MEMORY_HH
#define TTA_INSTRUCTION_MEMORY_HH

#include <vector>
#include "SimulatorConstants.hh"
#include "Exception.hh"
#include "BaseType.hh"

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
