/**
 * @file DisassemblyImmediateRegister.hh
 *
 * Declaration of DisassemblyImmediateRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_IMMEDIATE_REGISTER_HH
#define TTA_DISASSEMBLY_IMMEDIATE_REGISTER_HH

#include <string>
#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an immediate register in the disassembler.
 */
class DisassemblyImmediateRegister : public DisassemblyElement {
public:
    DisassemblyImmediateRegister(std::string rfName, Word index);
    virtual ~DisassemblyImmediateRegister();
    std::string toString() const;
private:
    /// Name of the immediate unit.
    std::string immName_;
    /// Index of the register in the immediate unit.
    Word index_;
};

#endif
