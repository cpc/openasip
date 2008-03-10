/**
 * @file DisassemblyAnnotation.hh
 *
 * Declaration of DisassemblyAnnotation class.
 *
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_ANNOTATION_HH
#define TTA_DISASSEMBLY_ANNOTATION_HH

#include <vector>

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an annotation.
 */
class DisassemblyAnnotation : public DisassemblyElement {
public:
    DisassemblyAnnotation(Word id, const std::vector<Byte>& payload);
    virtual ~DisassemblyAnnotation();
    virtual std::string toString() const;

private:
    /// Annotation id.
    Word id_;
    /// Name of the operation.
    std::vector<Byte> payload_;
};
#endif
