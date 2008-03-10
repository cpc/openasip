/**
 * @file DisassemblyInstruction.hh
 *
 * Declaration of DisassemblyInstruction class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_INSTRUCTION_HH
#define TTA_DISASSEMBLY_INSTRUCTION_HH

#include <string>
#include <vector>
#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyImmediateAssignment.hh"
#include "BaseType.hh"
#include "Exception.hh"

/**
 * Represents an instruction in the disassembler.
 */
class DisassemblyInstruction {
public:
    DisassemblyInstruction();
    ~DisassemblyInstruction();

    Word moveCount() const;
    void addMove(DisassemblyInstructionSlot* move);
    DisassemblyInstructionSlot& move(Word index) const throw (OutOfRange);

    Word longImmediateCount() const;
    void addLongImmediate(DisassemblyImmediateAssignment* longImm);
    DisassemblyImmediateAssignment& longImmediate(Word index) const throw (OutOfRange);

    std::string toString() const;

private:
    /// Vector of DisassemblyMoves.
    typedef std::vector<DisassemblyInstructionSlot*> MoveTable;

    /// Vector of DisassemblyMoves.
    typedef std::vector<DisassemblyImmediateAssignment*> LongImmediateTable;

    /// Copying not allowed.
    DisassemblyInstruction(const DisassemblyInstruction&);
    /// Assignment not allowed.
    DisassemblyInstruction operator=(const DisassemblyInstruction&);

    /// List of instruction moves.
    MoveTable moves_;

    /// List of instruction long immediates
    LongImmediateTable longImmediates_;
};
#endif
