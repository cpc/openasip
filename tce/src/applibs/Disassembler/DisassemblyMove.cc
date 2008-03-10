/**
 * @file DisassemblyMove.cc
 *
 * Implementation of DisassemblyMove class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyMove.hh"

/**
 * Creates disassembler of a move.
 *
 * @param source The source element of the move.
 * @param destination The destination element of the move.
 * @param guard Move guard, or NULL if the move is not guarded.
 */
DisassemblyMove::DisassemblyMove(
    DisassemblyElement* source,
    DisassemblyElement* destination,
    DisassemblyGuard* guard):
    DisassemblyInstructionSlot(),
    source_(source), destination_(destination), guard_(guard) {

}


/**
 * The destructor.
 */
DisassemblyMove::~DisassemblyMove() {
    delete source_;
    delete destination_;
    if (guard_ != NULL) {
	delete guard_;
    }
}


/**
 * Disassembles the move.
 *
 * @return Disassembled move as a string.
 */
std::string
DisassemblyMove::toString() const {

    std::string disassembly;
    if (guard_ != NULL) {
	disassembly = guard_->toString();
    }

    disassembly = disassembly + source_->toString() + " -> " +
        destination_->toString();

    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
