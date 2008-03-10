/**
 * @file DisassemblyImmediate.hh
 *
 * Declaration of DisassemblyImmediate class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_IMMEDIATE_HH
#define TTA_DISASSEMBLY_IMMEDIATE_HH

#include "DisassemblyElement.hh"
#include "SimValue.hh"

/**
 * Represents an inline immediate value in the disassembler.
 */
class DisassemblyImmediate : public DisassemblyElement {
public:
    DisassemblyImmediate(SimValue value);
    virtual ~DisassemblyImmediate();
    virtual std::string toString() const;
private:
    /// Immediate value.
    SimValue value_;
};

#endif
