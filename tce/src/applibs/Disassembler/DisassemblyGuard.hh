/**
 * @file DisassemblyGuard.hh"
 *
 * Declration of DisassemblyGuard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_GUARD
#define TTA_DISASSEMBLY_GUARD

#include "DisassemblyElement.hh"

/**
 * Represents a guard for the Disassembler.
 */
class DisassemblyGuard {
public:
    DisassemblyGuard(DisassemblyElement* guarded, bool inverted);
    virtual ~DisassemblyGuard();
    const DisassemblyElement& guardedElement() const;
    bool inverted() const;
    virtual std::string toString() const;

protected:
    /// True, if the guard is inverted.
    bool inverted_;
private:
    /// The element which is guarded.
    DisassemblyElement* guardedElement_;
};

#endif
