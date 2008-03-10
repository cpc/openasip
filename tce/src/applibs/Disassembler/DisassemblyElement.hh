/**
 * @file DisassemblyElement.hh
 *
 * Declaration of DisassemvlyElement class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_ELEMENT_HH
#define TTA_DISASSEMBLY_ELEMENT_HH

#include <string>

/**
 * Represents a source or destination of a move in the disassembler.
 */
class DisassemblyElement {
public:
    virtual ~DisassemblyElement();
    virtual std::string toString() const = 0;
protected:
    DisassemblyElement();
};

#endif
