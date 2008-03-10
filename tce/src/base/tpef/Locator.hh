/**
 * @file Locator.hh
 *
 * Declaration of Locator class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LOCATOR_HH
#define TTA_LOCATOR_HH

#include <set>

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "RelocElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Class who knows how to convert addresses depending on relocation
 * type.
 *
 * There should be possibility to register new relocation types, like
 * section readers and binary readers are implemented.
 */
class Locator {
public:
    static AddressImage absoluteAddress(
        AddressImage relocationValue,
        RelocElement::RelocType relocType);

};
}

#endif
