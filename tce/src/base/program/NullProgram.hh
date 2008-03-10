/**
 * @file NullProgram.hh
 *
 * Declaration of NullProgram class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_PROGRAM_HH
#define TTA_NULL_PROGRAM_HH

#include "Program.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null program.
 *
 * Calling any method causes the program to abort.
 */
class NullProgram : public Program {
public:
    virtual ~NullProgram();
    static NullProgram& instance();

    GlobalScope& globalScope();
    const GlobalScope& globalScopeConst() const;

    TTAMachine::Machine& targetProcessor() const;

    Address startAddress() const;
    Address entryAddress() const;
    void setEntryAddress(Address address);

    void addProcedure(Procedure& proc) throw (IllegalRegistration);
    void addInstruction(Instruction& ins) throw (IllegalRegistration);

    void relocate(const Procedure& proc, UIntWord howMuch);

    Procedure& firstProcedure() const throw (InstanceNotFound);
    Procedure& lastProcedure() const throw (InstanceNotFound);
    Procedure& nextProcedure(const Procedure& proc) const
        throw (IllegalRegistration);
    int procedureCount() const;
    Procedure& procedure(int index) const throw (OutOfRange);
    Procedure& procedure(const std::string& name) const
        throw (KeyNotFound);

    Instruction& firstInstruction() const throw (InstanceNotFound);
    const Instruction& instructionAt(UIntWord address) const
        throw (KeyNotFound);
    const Instruction& nextInstruction(const Instruction&) const
        throw (IllegalRegistration);
    Instruction& lastInstruction() const throw (InstanceNotFound);

    InstructionReferenceManager& instructionReferenceManager();

protected:
    NullProgram();

private:
    /// Copying not allowed.
    NullProgram(const NullProgram&);
    /// Assignment not allowed.
    NullProgram& operator=(const NullProgram&);

    /// Unique instance of NullProgram.
    static NullProgram instance_;
};

}

#endif
