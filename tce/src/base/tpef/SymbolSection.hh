/**
 * @file SymbolSection.hh
 *
 * Declaration of SymbolSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SYMBOL_SECTION_HH
#define TTA_SYMBOL_SECTION_HH

#include "Section.hh"

namespace TPEF {

/**
 * Symbol table section.
 */
class SymbolSection : public Section {
public:
    virtual ~SymbolSection();

    virtual SectionType type() const;

protected:
    SymbolSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static SymbolSection proto_;
};
}

#endif
