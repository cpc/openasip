/**
 * @file DisassemblyFUOperand.hh
 *
 * Declaration of DisassemblyFUOperand class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_FU_OPERAND_HH
#define TTA_DISASSEMBLY_FU_OPERAND_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an fu.operand.index referencein the disassembler.
 */
class DisassemblyFUOperand : public DisassemblyElement {
public:
    DisassemblyFUOperand(
        std::string fuName, std::string operationName, Word operand);

    virtual ~DisassemblyFUOperand();

    virtual std::string toString() const;

private:
    /// Name of the function unit.
    std::string fuName_;
    /// Name of the operation.
    std::string operationName_;
    /// Operand index.
    Word operand_;
};
#endif
