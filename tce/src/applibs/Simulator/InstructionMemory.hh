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
 * @file InstructionMemory.hh
 *
 * Declaration of InstructionMemory class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_MEMORY_HH
#define TTA_INSTRUCTION_MEMORY_HH

#include <vector>
#if __cplusplus < 201103L
#include <map>
#else
#include <unordered_map>
#endif

#include "SimulatorConstants.hh"
#include "Exception.hh"
#include "BaseType.hh"

class ExecutableInstruction;

/**
 * Container for ExecutableInstructions which represent the actions performed
 * by the simulated program's instructions to the machine's state.
 */
class InstructionMemory {
public:
    /// Container for instructions.
    typedef std::vector<ExecutableInstruction*> InstructionContainer;

    InstructionMemory(InstructionAddress startAddress);
    virtual ~InstructionMemory();

    void addExecutableInstruction(
        InstructionAddress addr, ExecutableInstruction* instruction);
    void addImplicitExecutableInstruction(
        InstructionAddress addr, ExecutableInstruction* instruction);
    ExecutableInstruction& instructionAt(InstructionAddress address);
    const ExecutableInstruction& instructionAtConst(
        InstructionAddress address) const;

    void resetExecutionCounts();

    bool hasInstructionAt(InstructionAddress addr) const;

    bool hasImplicitInstructionsAt(InstructionAddress addr) const;
    const InstructionContainer& implicitInstructionsAt(
        InstructionAddress addr) const;

private:
    /// Copying not allowed.
    InstructionMemory(const InstructionMemory&);
    /// Assignment not allowed.
    InstructionMemory& operator=(const InstructionMemory&);

    /// The starting address of the instruction memory address space.
    InstructionAddress startAddress_;

    /// All the instructions of the memory.
    InstructionContainer instructions_;

    InstructionContainer emptyInstructions_;


#if __cplusplus < 201103L
    /// Stores the explicit instruction addresses.
    std::map<InstructionAddress, ExecutableInstruction*> instructionMap_;
    /// Stores implicit instructions that should be executed after the explicit
    /// one in the same address.
    std::map<InstructionAddress, InstructionContainer*> implicitInstructions_;
#else
    /// Stores the explicit instruction addresses.
    std::unordered_map<InstructionAddress, ExecutableInstruction*> instructionMap_;
    /// Stores implicit instructions that should be executed after the explicit
    /// one in the same address.
    std::unordered_map<InstructionAddress, InstructionContainer*> implicitInstructions_;
#endif


};

#include  "InstructionMemory.icc"

#endif
