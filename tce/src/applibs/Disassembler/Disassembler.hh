/**
 * @file Disassembler.hh
 *
 * Declaration of Disassembler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLER_HH
#define TTA_DISASSEMBLER_HH

#include "BaseType.hh"

class DisassemblyInstruction;

/**
 * An abstract baseclass for disassemblers.
 */
class Disassembler {
public:
    virtual ~Disassembler();

    /// Creates disassembler instruction.
    virtual DisassemblyInstruction* createInstruction(
        Word instructionIndex) const = 0;

    /// Returns address of the first instruction.
    virtual Word startAddress() const = 0;

    /// Returns number of instructions that are available.
    virtual Word instructionCount() const = 0;

protected:
    Disassembler();
};
#endif
