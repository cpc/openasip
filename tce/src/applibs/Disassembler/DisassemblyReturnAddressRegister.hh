/**
 * @file DisassemblyReturnAddressRegister.hh
 *
 * Declaration of DisassemblyReturnAddressRegister class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_RETURN_ADDRESS_REGISTER_HH
#define TTA_DISASSEMBLY_RETURN_ADDRESS_REGISTER_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an GCU's special return address register in
 * the disassembler.
 */
class DisassemblyReturnAddressRegister : public DisassemblyElement {
public:
    DisassemblyReturnAddressRegister();
    virtual ~DisassemblyReturnAddressRegister();
    virtual std::string toString() const;
private:

};

#endif
