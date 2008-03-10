/**
 * @file DisassemblyImmediateAssignment.hh
 *
 * Declaration of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_IMMEDIATE_ASSIGNMENT_HH
#define TTA_DISASSEMBLY_IMMEDIATE_ASSIGNMENT_HH

#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyElement.hh"
#include "SimValue.hh"

/**
 * Represents piece of long immediate assignment in the disassembler.
 *
 * If the long immediate value is given using the constructor with value
 * parameter, disassembly string displays the value.
 */
class DisassemblyImmediateAssignment: public DisassemblyInstructionSlot {
public:
    DisassemblyImmediateAssignment(
	SimValue value,
	DisassemblyElement* destination);
    explicit DisassemblyImmediateAssignment(DisassemblyElement* destination);

    virtual ~DisassemblyImmediateAssignment();
    virtual std::string toString() const;

private:
    /// Coying not allowed.
    DisassemblyImmediateAssignment(const DisassemblyImmediateAssignment&);
    /// Assignment not allowed.
    DisassemblyImmediateAssignment& operator=(
	const DisassemblyImmediateAssignment&);

    /// Value of the immediate.
    SimValue value_;
    /// The destination register.
    DisassemblyElement* destination_;
    /// True, if the immediate has value set.
    bool hasValue_;
};

#endif
