/**
 * @file DisassemblyMove.hh
 *
 * Declaration of DisassemblyMove class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_MOVE_HH
#define TTA_DISASSEMBLY_MOVE_HH

#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyElement.hh"
#include "DisassemblyGuard.hh"

/**
 * Represents a move in the disassembler.
 */
class DisassemblyMove : public DisassemblyInstructionSlot {
public:
    DisassemblyMove(
        DisassemblyElement* source,
        DisassemblyElement* destination,
        DisassemblyGuard* guard = NULL);        
    
    virtual ~DisassemblyMove();
    virtual std::string toString() const;

private:
    /// Coying not allowed.
    DisassemblyMove(const DisassemblyMove&);
    /// Assignment not allowed.
    DisassemblyMove& operator=(const DisassemblyMove&);

    /// The source of the move.
    DisassemblyElement* source_;
    /// The destination of the move.
    DisassemblyElement* destination_;
    /// Possible guard of the move.
    DisassemblyGuard* guard_;
};

#endif
