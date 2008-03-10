/**
 * @file NullProcedure.hh
 *
 * Declaration of NullProcedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_PROCEDURE_HH
#define TTA_NULL_PROCEDURE_HH

#include "BaseType.hh"
#include "Procedure.hh"
#include "Address.hh"

namespace TTAProgram {

class Instruction;
class CodeSnippet;

/**
 * A singleton class that represents a null procedure.
 *
 * Calling any method causes the program to abort.
 */
class NullProcedure : public Procedure {
public:
    virtual ~NullProcedure();
    static NullProcedure& instance();

    Program& parent() const throw (IllegalRegistration);
    void setParent(const Program& prog);
    bool isInProgram() const;
    std::string name() const;
    int alignment() const;

    Address address(const Instruction& ins) const
        throw (IllegalRegistration);

    Address startAddress() const;
    void setStartAddress(Address start);
    Address endAddress() const;

    int instructionCount() const;
    Instruction& firstInstruction() const throw (InstanceNotFound);
    Instruction& instructionAt(UIntWord address) const
        throw (KeyNotFound);
    bool hasNextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& nextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& previousInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& lastInstruction() const throw (IllegalRegistration);

    void addInstruction(Instruction& ins) throw (IllegalRegistration);
    void insertInstructionAfter(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);

protected:
    NullProcedure();

private:
    /// Copying not allowed.
    NullProcedure(const NullProcedure&);
    /// Assignment not allowed.
    NullProcedure& operator=(const NullProcedure&);

    /// Unique instance of NullProcedure.
    static NullProcedure instance_;
};

}

#endif
