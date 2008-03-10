/**
 * @file NoTypeSymElement.hh
 *
 * Declaration of NoTypeSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_NO_TYPE_SYM_ELEMENT_HH
#define TTA_NO_TYPE_SYM_ELEMENT_HH

#include "SymbolElement.hh"

namespace TPEF {

/**
 * No type symbol element.
 */
class NoTypeSymElement : public SymbolElement {
 public:
    NoTypeSymElement();
    virtual ~NoTypeSymElement();

    virtual SymbolType type() const;

 private:
};
}

#endif
