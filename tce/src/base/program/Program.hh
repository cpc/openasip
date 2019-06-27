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
 * @file Program.hh
 *
 * Declaration of Program class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_HH
#define TTA_PROGRAM_HH

#include <vector>

#include "Address.hh"
#include "BaseType.hh"
#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

class UniversalMachine;

namespace TTAProgram {

    class Instruction;
class Procedure;
class DataMemory;
class Move;
class InstructionReferenceManager;
class TerminalImmediate;
class Terminal;
class GlobalScope;

/**
 * Represents a TTA program.
 */
class Program {
public:
    /// Vector for instructions.
    typedef std::vector<Instruction*> InstructionVector;

    Program(const TTAMachine::AddressSpace& space);
    Program(const TTAMachine::AddressSpace& space, Address start);
    Program(
        const TTAMachine::AddressSpace& space,
        Address start,
        Address entry);
    virtual ~Program();

    GlobalScope& globalScope();
    const GlobalScope& globalScopeConst() const;

    TTAMachine::Machine& targetProcessor() const;
    UniversalMachine& universalMachine() const;
    void setUniversalMachine(UniversalMachine* umach) { umach_ = umach; }

    Address startAddress() const;
    void setStartAddress(Address start);
    Address entryAddress() const;
    void setEntryAddress(Address address);

    void addProcedure(Procedure* proc);
    void addInstruction(Instruction* ins);

    void moveProcedure(Procedure& proc, int howMuch);

    Procedure& firstProcedure() const;
    Procedure& lastProcedure() const;
    Procedure& nextProcedure(const Procedure& proc) const;
    int procedureCount() const;
    Procedure& procedure(int index) const;
    Procedure& operator[](size_t index);
    Procedure& procedure(const std::string& name) const;
    bool hasProcedure(const std::string& name) const;

    Instruction& firstInstruction() const;
    Instruction& instructionAt(InstructionAddress address) const;
    int instructionCount() const;

    const Move& moveAt(int number) const;
    int moveCount() const;

    const Procedure& procedureAtIndex(int index) const;

    Instruction& nextInstruction(const Instruction&) const;
    Instruction& lastInstruction() const;

    InstructionReferenceManager& instructionReferenceManager() const;

    Program& operator=(const Program& old);
    Program* copy() const;

    void removeProcedure(Procedure& proc);

    int dataMemoryCount() const;
    DataMemory& dataMemory(int index) const;
    DataMemory& dataMemory(const std::string& aSpaceName) const;
    void addDataMemory(DataMemory* dataMem);

    void replaceUniversalAddressSpaces(const TTAMachine::AddressSpace& space);

    InstructionVector instructionVector() const;

    static Program* loadFromUnscheduledTPEF(
        const std::string& tpefFileName, const TTAMachine::Machine& theMachine);

    static Program* loadFromTPEF(
        const std::string& tpefFileName, const TTAMachine::Machine& theMachine);

    static Program* loadFromUnscheduledTPEF(const std::string& tpefFileName);

    static void writeToTPEF(
        const TTAProgram::Program& program, const std::string& tpefFileName);

    void convertSymbolRefsToInsRefs();
private:
    /// List for procedures.
    typedef std::vector<Procedure*> ProcList;
    /// Iterator for the procedure list.
    typedef ProcList::const_iterator ProcIter;
    /// List for data memories.
    typedef std::vector<DataMemory*> DataMemList;
    /// List for moves
    typedef std::vector<Move*> MoveList;

    /// Copying not allowed.
    Program(const Program&);

    void fixInstructionReferences();
    void copyDataMemoriesFrom(const Program& srcProg);
    void copyCodeLabelsFrom(const Program& srcProg);
    void copyDataLabelsFrom(const Program& srcProg);
    void copyFrom(const Program& source);
    void cleanup();

    TerminalImmediate* convertSymbolRef(Terminal& tsr);

    /// Global scope of the program.
    GlobalScope* globalScope_;

    /// The procedures in the program.
    ProcList procedures_;

    /// The data memories in the program.
    DataMemList dataMems_;
    
    /// List of all the moves of the program.
    MoveList moves_;

    /// The start address of the program.
    Address start_;
    /// The entry address of the program.
    Address entry_;

    /// Keeps book of all instruction to instruction (jumps and calls)
    /// references in the program.
    InstructionReferenceManager* refManager_;

    /// The UniversalMachine instance used to refer to in case of the 
    /// unscheduled/unassigned parts of the program.
    mutable UniversalMachine* umach_;
};

}

#endif
