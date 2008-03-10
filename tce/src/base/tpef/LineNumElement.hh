/**
 * @file LineNumElement.hh
 *
 * Declaration of LineNumElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LINE_NUM_ELEMENT_HH
#define TTA_LINE_NUM_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "InstructionElement.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Line number element.
 */
class LineNumElement : public SectionElement {
public:
    LineNumElement();
    virtual ~LineNumElement();

    void setLineNumber(HalfWord lineNum);
    HalfWord lineNumber() const;

    void setInstruction(ReferenceManager::SafePointer* aRef);
    void setInstruction(InstructionElement* aRef);
    InstructionElement* instruction() const;

private:
    /// Line number which line in the procedure this element refer.
    HalfWord lineNumber_;
    /// Instruction element of source line.
    const ReferenceManager::SafePointer* instruction_;

};
}

#include "LineNumElement.icc"

#endif
