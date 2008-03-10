/**
 * @file DisassemblySequentialGuard.hh
 *
 * Declration of DisassemblySequentialGuard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_SEQUENTIAL_GUARD
#define TTA_DISASSEMBLY_SEQUENTIAL_GUARD

#include "DisassemblyGuard.hh"

/**
 * Represents a guard for the Disassembler.
 */
class DisassemblySequentialGuard : public DisassemblyGuard {
public:
    DisassemblySequentialGuard(bool inverted);
    virtual ~DisassemblySequentialGuard();
    virtual std::string toString() const;
};

#endif
