/**
 * @file DisassemblyIntRegister.hh
 *
 * Declaration of DisassemblyIntRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_INT_REGISTER_HH
#define TTA_DISASSEMBLY_INT_REGISTER_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an int register in the disassembler.
 */
class DisassemblyIntRegister : public DisassemblyElement {
public:
    DisassemblyIntRegister(Word index);
    virtual ~DisassemblyIntRegister();
    virtual std::string toString() const;
private:
    /// Index of the register.
    Word index_;

};

#endif
