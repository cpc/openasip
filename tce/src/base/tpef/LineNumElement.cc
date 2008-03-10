/**
 * @file LineNumElement.cc
 *
 * Non-inline definitions of LineNumElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "LineNumElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
LineNumElement::LineNumElement() : SectionElement(),
    lineNumber_(0), instruction_(&SafePointer::null) {
}

/**
 * Destructor.
 */
LineNumElement::~LineNumElement() {
}

}
