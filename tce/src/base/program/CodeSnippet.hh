/**
 * @file CodeSnippet.hh
 *
 * Declaration of CodeSnippet class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODESNIPPET_HH
#define TTA_CODESNIPPET_HH

#include <vector>

#include "Instruction.hh"
#include "Exception.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

    class Program;

/**
 * A code snippet is an ordered sequence of adjacent instructions.
 *
 * It is a helper class for representing pieces of code that are not
 * necessarily full procedures, for example basic blocks. Code snippet
 * doesn't care, whether the sequence of instructions in it makes sense
 * or not. That's the responsibility of the client that created the
 * snippet.
 */
class CodeSnippet {
public:
    CodeSnippet();
    CodeSnippet(const TTAProgram::Address& start);

    virtual ~CodeSnippet();

    virtual void clear();

    virtual void removeLastInstruction();
    virtual int instructionCount() const;

    virtual Program& parent() const throw (IllegalRegistration);
    virtual void setParent(Program& prog);
    virtual bool isInProgram() const;

    virtual Address address(const Instruction& ins) const
        throw (IllegalRegistration);

    virtual Address startAddress() const;
    virtual void setStartAddress(Address start);
    virtual Address endAddress() const;

    virtual Instruction& firstInstruction() const throw (InstanceNotFound);
    virtual Instruction& instructionAt(UIntWord address) const
        throw (KeyNotFound);

    virtual Instruction& instructionAtIndex(int index) const;

    virtual bool hasNextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    virtual Instruction& nextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    virtual Instruction& previousInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    virtual Instruction& lastInstruction() const 
        throw (IllegalRegistration);

    virtual void addFront(Instruction* ins) 
        throw (IllegalRegistration);
    virtual void add(Instruction* ins) 
        throw (IllegalRegistration);
    virtual void insertAfter(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);
    virtual void insertBefore(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);

    virtual void remove(Instruction& ins) 
        throw (IllegalRegistration);

    virtual void deleteInstructionAt(InstructionAddress address)
        throw (KeyNotFound);

    virtual CodeSnippet* copy() const;

    virtual void prepend(const CodeSnippet& cs);
    virtual void prepend(CodeSnippet* cs);
    virtual void append(const CodeSnippet& cs);
    virtual void append(CodeSnippet* cs);
    
    virtual void insertBefore(const Instruction& pos, const CodeSnippet& cs);
    virtual void insertBefore(const Instruction& pos, CodeSnippet* cs);
    virtual void insertAfter(const Instruction& pos, const CodeSnippet& cs);
    virtual void insertAfter(const Instruction& pos, CodeSnippet* cs);

protected:
    /// List of instructions.
    typedef std::vector<Instruction*> InsList;
    /// Iterator for the instruction list.
    typedef InsList::iterator InsIter;

    /// The instructions in this procedure.
    InsList instructions_;
    /// The parent program of the procedure.
    Program* parent_;
    /// The start (lowest) address of the procedure.
    Address start_;


};

}

#endif
