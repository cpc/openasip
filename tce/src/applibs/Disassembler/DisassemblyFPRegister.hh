/**
 * @file DisassemblyFPRegister.hh
 *
 * Declaration of DisassemblyFPRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_FP_REGISTER_HH
#define TTA_DISASSEMBLY_FP_REGISTER_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents a floating point register in the disassembler.
 */
class DisassemblyFPRegister : public DisassemblyElement {
public:
    DisassemblyFPRegister(Word index);
    virtual ~DisassemblyFPRegister();
    virtual std::string toString() const;
private:
    /// Index of the register.
    Word index_;

};

#endif
