/**
 * @file Locator.cc
 *
 * Definition of Locator class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "Locator.hh"

namespace TPEF {

/**
 * Hard coded to return absolute address of relocation.
 *
 * @param relocationValue Value to be converted to an absolute address.
 * @param relocType Type of relocation.
 * @return Absolute address of relocated value.
 */
AddressImage
Locator::absoluteAddress(
    AddressImage relocationValue,
    RelocElement::RelocType relocType) {

    if (relocType != RelocElement::RT_SELF) {
        bool unsupportedRelocationType = false;
        assert(unsupportedRelocationType);
    }

    return relocationValue;
}

}
