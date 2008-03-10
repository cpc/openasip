/**
 * @file CodeSymElement.hh
 *
 * Declaration of CodeSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_CODE_SYM_ELEMENT_HH
#define TTA_CODE_SYM_ELEMENT_HH

#include "SymbolElement.hh"
#include "InstructionElement.hh"

namespace TPEF {

/**
 * Code symbol.
 */
class CodeSymElement : public SymbolElement {
 public:
    CodeSymElement();
    virtual ~CodeSymElement();

    virtual SymbolType type() const;

    InstructionElement* reference() const;
    void setReference(InstructionElement* aReference);
    void setReference(const ReferenceManager::SafePointer* aReference);

    Word size() const;
    void setSize(Word aSize);

 private:
    /// Referred object.
    const ReferenceManager::SafePointer* reference_;
    /// Size of referred object in MAUs.
    Word size_;
};
}

#endif
