/**
 * @file SectionSymElement.hh
 *
 * Declaration of SectionSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_SYM_ELEMENT_HH
#define TTA_SECTION_SYM_ELEMENT_HH

#include "SymbolElement.hh"

namespace TPEF {

/**
 * Section symbol element.
 */
class SectionSymElement : public SymbolElement {
 public:
    SectionSymElement();
    virtual ~SectionSymElement();

    virtual SymbolType type() const;

    Word value() const;
    void setValue(Word aValue);

    Word size() const;
    void setSize(Word aSize);

private:
    /// Value of the symbol.
    Word value_;
    /// Size of referred object in MAUs.
    Word size_;
};
}

#endif
