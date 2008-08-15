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
 * @file Program.hh
 *
 * Declaration of Program class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_HH
#define TTA_PROGRAM_HH

#include <vector>

#include "Address.hh"
#include "BaseType.hh"
#include "Exception.hh"
#include "GlobalScope.hh"
#include "InstructionReferenceManager.hh"

namespace TTAMachine {
    class Machine;
}

class UniversalMachine;

namespace TTAProgram {

class Procedure;
class Instruction;
class DataMemory;
class Move;

/**
 * Represents a TTA program.
 */
class Program {
public:
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

    Address startAddress() const;
    void setStartAddress(Address start);
    Address entryAddress() const;
    void setEntryAddress(Address address);

    void addProcedure(Procedure* proc) throw (IllegalRegistration);
    void addInstruction(Instruction* ins) throw (IllegalRegistration);

    void moveProcedure(Procedure& proc, int howMuch);

    Procedure& firstProcedure() const throw (InstanceNotFound);
    Procedure& lastProcedure() const throw (InstanceNotFound);
    Procedure& nextProcedure(const Procedure& proc) const
        throw (IllegalRegistration);
    int procedureCount() const;
    Procedure& procedure(int index) const throw (OutOfRange);
    Procedure& procedure(const std::string& name) const
        throw (KeyNotFound);

    Instruction& firstInstruction() const throw (InstanceNotFound);
    Instruction& instructionAt(InstructionAddress address) const
        throw (KeyNotFound);
    
    const Move& moveAt(int number) const throw (KeyNotFound);
    int moveCount() const;

    const Procedure& procedureAtIndex(int index) const;

    Instruction& nextInstruction(const Instruction&) const
        throw (IllegalRegistration);
    Instruction& lastInstruction() const throw (InstanceNotFound);

    InstructionReferenceManager& instructionReferenceManager() const;

    Program& operator=(const Program& old);
    Program* copy() const;

    void removeProcedure(Procedure& proc) throw (IllegalRegistration);

    int dataMemoryCount() const;
    DataMemory& dataMemory(int index) const throw (OutOfRange);
    DataMemory& dataMemory(const std::string& aSpaceName) const 
        throw (KeyNotFound);
    void addDataMemory(DataMemory* dataMem) throw (IllegalRegistration);

    void replaceUniversalAddressSpaces(const TTAMachine::AddressSpace& space);

    static Program* loadFromTPEF(
        const std::string& tpefFileName,
        const TTAMachine::Machine& theMachine,
        const UniversalMachine& umach)
        throw (Exception);

    static Program* loadFromTPEF(
        const std::string& tpefFileName,
        const TTAMachine::Machine& theMachine)
        throw (Exception);
    
    static Program* loadFromTPEF(
        const std::string& tpefFileName,
        const UniversalMachine& umach)
        throw (Exception);

    static void writeToTPEF(
        const TTAProgram::Program& program,
        const std::string& tpefFileName)
        throw (Exception);

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
};

}

#endif
