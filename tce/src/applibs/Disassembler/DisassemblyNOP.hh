/**
 * @file DisassemblyNOP.hh
 *
 * Declaration of DisassemblyNOP class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_NOP_HH
#define TTA_DISASSEMBLY_NOP_HH

#include "DisassemblyInstructionSlot.hh"

/**
 * Represents No-operation in disassembly instructions.
 */
class DisassemblyNOP : public DisassemblyInstructionSlot {
public:
    DisassemblyNOP();
    virtual ~DisassemblyNOP();
    virtual std::string toString() const;

private:
    /// Coying not allowed.
    DisassemblyNOP(const DisassemblyNOP&);
    /// Assignment not allowed.
    DisassemblyNOP& operator=(const DisassemblyNOP&);
};

#endif
