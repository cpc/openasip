/**
 * @file SymbolElement.cc
 *
 * Non-inline definitions of SymbolElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#include "SymbolElement.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
SymbolElement::SymbolElement() :
    SectionElement(),
    absolute_(false),
    bind_(STB_LOCAL),
    name_(&SafePointer::null),
    section_(&SafePointer::null) {
}

/**
 * Destructor.
 */
SymbolElement::~SymbolElement() {
}

}
