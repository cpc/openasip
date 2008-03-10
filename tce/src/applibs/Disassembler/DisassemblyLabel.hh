/**
 * @file DisassemblyLabel.hh
 *
 * Declaration of DisassemblyLabel class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_LABEL_HH
#define TTA_DISASSEMBLY_LABEL_HH

#include "DisassemblyElement.hh"

/**
 * Represents a code label in the disassembler.
 */
class DisassemblyLabel : public DisassemblyElement {
public:
    DisassemblyLabel(std::string label);
    virtual ~DisassemblyLabel();
    virtual std::string toString() const;
private:
    /// Name of the label.
    std::string label_;

};

#endif
