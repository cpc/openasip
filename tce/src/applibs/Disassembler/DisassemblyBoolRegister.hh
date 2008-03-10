/**
 * @file DisassemblyBoolRegister.hh
 *
 * Declaration of DisassemblyBoolRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_BOOL_REGISTER_HH
#define TTA_DISASSEMBLY_BOOL_REGISTER_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an universal machine bool register in the disassembler.
 */
class DisassemblyBoolRegister : public DisassemblyElement {
public:
    DisassemblyBoolRegister();
    virtual ~DisassemblyBoolRegister();
    virtual std::string toString() const;
private:

};

#endif
