/**
 * @file ProcedSymElement.cc
 *
 * Definition of ProcedSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "ProcedSymElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
ProcedSymElement::ProcedSymElement() :
    CodeSymElement() {
}

/**
 * Destructor.
 */
ProcedSymElement::~ProcedSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
ProcedSymElement::type() const {
    return STT_PROCEDURE;
}

}
