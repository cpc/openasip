/**
 * @file SafePointable.cc
 *
 * Implementation of the SafePointable class.
 *
 * @author Pekka Jääskeläinen 2003 (pjaaskel@cs.tut.fi)
 * @note Reviewed 29 Aug 2003 by Risto Mäkinen, Mikael Lepistö, Andrea Cilio,
 * Tommi Rantanen
 *
 * @note rating: yellow
 */
#include "SafePointer.hh"

namespace TPEF {

/**
 *
 * Destructor.
 *
 * Informs of deletion of itself to the reference manager.
 *
 */
SafePointable::~SafePointable() {
    ReferenceManager::SafePointer::notifyDeleted(this);
}

/**
 *
 * Constructor.
 *
 */
SafePointable::SafePointable() {
}

}
