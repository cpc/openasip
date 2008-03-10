/**
 * @file DisassemblyRegister.hh
 *
 * Declaration of DisassemblyRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_REGISTER_HH
#define TTA_DISASSEMBLY_REGISTER_HH

#include <string>
#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents a register in the disassembler.
 */
class DisassemblyRegister : public DisassemblyElement {
public:
    DisassemblyRegister(std::string rfName, Word index);
    virtual ~DisassemblyRegister();
    std::string toString() const;
private:
    /// Name of the register file.
    std::string rfName_;
    /// Index of the register in the register file.
    Word index_;
};

#endif
