/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ResourceSection.cc
 *
 * Definition of ResourceSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
