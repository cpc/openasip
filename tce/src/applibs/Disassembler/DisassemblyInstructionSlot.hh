/**
 * @file DisassemblyInstructionSlot.hh
 *
 * Declaration of DisassemblyInstructionSlot class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_INSTRUCTION_SLOT_HH
#define TTA_DISASSEMBLY_INSTRUCTION_SLOT_HH

#include <string>

#include "DisassemblyAnnotation.hh"

/**
 * An abstract base class for instruction disassembly slots.
 */
class DisassemblyInstructionSlot {
public:
    virtual ~DisassemblyInstructionSlot();
    virtual std::string toString() const = 0;

    DisassemblyAnnotation& annotation(int index) const;
    void addAnnotation(DisassemblyAnnotation* annotation);
    int annotationCount() const;

protected:
    DisassemblyInstructionSlot();

private:
    /// Copying not allowed.
    DisassemblyInstructionSlot(const DisassemblyInstructionSlot&);
    /// Assignment not allowed.
    DisassemblyInstructionSlot& operator=(const DisassemblyInstructionSlot&);
    /// Annotationes of move or long immediates.
    std::vector<DisassemblyAnnotation*> annotationes_;
};

#endif
