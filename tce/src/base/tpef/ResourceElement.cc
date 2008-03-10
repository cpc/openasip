/**
 * @file ResourceElement.cc
 *
 * Non-inline definitions of ResourceElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "ResourceElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

const std::string
ResourceElement::RETURN_ADDRESS_NAME = "return-address";

/**
 * Constructs empty element.
 */
ResourceElement::ResourceElement() : SectionElement(),
    id_(0), type_(MRT_NULL), name_(&SafePointer::null), info_(0) {

}

/**
 * Handles destruction element.
 */
ResourceElement::~ResourceElement() {
}

}
