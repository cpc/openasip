/**
 * @file NoTypeSymElement.cc
 *
 * Definition of NoTypeSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "NoTypeSymElement.hh"

namespace TPEF {

/**
 * Constructor.
 */
NoTypeSymElement::NoTypeSymElement() : SymbolElement() {
    setAbsolute(true);
}

/**
 * Destructor.
 */
NoTypeSymElement::~NoTypeSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
NoTypeSymElement::type() const {
    return STT_NOTYPE;
}

}
