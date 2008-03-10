/**
 * @file Procedure.hh
 *
 * Declaration of Procedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCEDURE_HH
#define TTA_PROCEDURE_HH

#include <vector>
#include <string>

#include "Exception.hh"
#include "Address.hh"
#include "CodeSnippet.hh"

namespace TTAProgram {

class Instruction;
class Program;


/**
 * Represents a TTA procedure.
 */
class Procedure : public CodeSnippet {

    public:
        Procedure(const std::string& name,
              const TTAMachine::AddressSpace& space);
    Procedure(
        const std::string& name,
        const TTAMachine::AddressSpace& space,
        UIntWord startLocation);
    virtual ~Procedure();

    std::string name() const;

    int alignment() const;

    Address address(const Instruction& ins) const
        throw (IllegalRegistration);

    void addFront(Instruction*) 
        throw (IllegalRegistration) {
        abortWithError("Not Implemented yet.");
    }

    void add(Instruction* ins) 
        throw (IllegalRegistration);
    void insertAfter(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);
    void insertBefore(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);

    using CodeSnippet::insertAfter;
    using CodeSnippet::insertBefore;
    
    void remove(Instruction& ins) 
        throw (IllegalRegistration);

    CodeSnippet* copy() const;

private:
    /// Copying not allowed.
    Procedure(const Procedure&);
    /// Assignment not allowed.
    Procedure& operator=(const Procedure&);

    /// The name of the procedure.
    const std::string name_;
    /// The alignment of instructions.
    int alignment_;
    /// The default alignment of instructions.
    static const int INSTRUCTION_INDEX_ALIGNMENT;
};

#include "Procedure.icc"

}

#endif
