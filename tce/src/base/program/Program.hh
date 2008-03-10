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
