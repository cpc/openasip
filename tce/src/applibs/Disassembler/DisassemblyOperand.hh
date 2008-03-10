/**
 * @file DisassemblyOperand.hh
 *
 * Declaration of DisassemblyOperand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_OPERAND_HH
#define TTA_DISASSEMBLY_OPERAND_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an operand of unuiversal machine
 * in the disassembler.
 */
class DisassemblyOperand : public DisassemblyElement {
public:
    DisassemblyOperand(std::string operationName, Word operand);
    virtual ~DisassemblyOperand();
    virtual std::string toString() const;
private:
    /// Name of the operation.
    std::string operationName_;
    /// Operand index.
    Word operand_;
};
#endif
