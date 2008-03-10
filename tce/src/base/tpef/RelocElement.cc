/**
 * @file RelocElement.cc
 *
 * Non-inline definitions of RelocElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#include "RelocElement.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
RelocElement::RelocElement() :
    SectionElement(), type_(RT_NOREL), location_(&SafePointer::null), 
    destination_(&SafePointer::null), size_(0), bitOffset_(0), 
    symbol_(&SafePointer::null), aSpace_(&SafePointer::null), 
    chunked_(false) {
}

/**
 * Destructor.
 */
RelocElement::~RelocElement() {
}

}
