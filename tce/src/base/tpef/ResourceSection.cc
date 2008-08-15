/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ResourceSection.cc
 *
 * Definition of ResourceSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "ResourceSection.hh"

namespace TPEF {

ResourceSection ResourceSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
ResourceSection::ResourceSection(bool init) : Section() {
    if (init) {
        Section::registerSection(this);
    }

    unsetFlagNoBits();
    unsetFlagVLen();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
ResourceSection::~ResourceSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
ResourceSection::type() const {
    return ST_MR;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
ResourceSection::clone() const {
    return new ResourceSection(false);
}

/**
 * Returns machine resource by id and type.
 *
 * @todo Instance not found exception. After all exceptions are implemented.
 *
 * @param aType Type of requested resource.
 * @param anId Id of resource to find.
 * @return Matching resource.
 */
ResourceElement&
ResourceSection::findResource(
    ResourceElement::ResourceType aType, HalfWord anId) const {

    for (Word i = 0; i < elementCount(); i++) {
        ResourceElement *elem = dynamic_cast<ResourceElement*>(element(i));

        if (elem->type() == aType && elem->id() == anId) {
            return *elem;
        }
    }

    // just to remind about exception to throw.
    std::cerr << "Can't find resource type: " << aType
              << "\tid: " << anId << std::endl;

    assert(false);

    // added throw clause to stop compiler from complaining --Pekka
    throw new int;
}

/**
 * Checks if queried resource if found from section.
 *
 * @param aType Type of requested resource.
 * @param anId Id of resource to find.
 * @return True if resource was found.
 */
bool
ResourceSection::hasResource(
    ResourceElement::ResourceType aType, HalfWord anId) const {

    for (Word i = 0; i < elementCount(); i++) {
        ResourceElement *elem = dynamic_cast<ResourceElement*>(element(i));

        if (elem->type() == aType && elem->id() == anId) {
            return true;
        }
    }

    return false;
}

}
