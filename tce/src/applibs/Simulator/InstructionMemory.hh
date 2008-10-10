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
