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
 * @file ShortImmPSocketResource.cc
 *
 * Implementation of ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "ShortImmPSocketResource.hh"

/**
 * Constructor.
 *
 * @param name Name of socket.
 */
ShortImmPSocketResource::ShortImmPSocketResource(
    const std::string& name,
    int immediateWidth, bool signExtends):
        OutputPSocketResource(name), immediateWidth_(immediateWidth),
        signExtends_(signExtends) {}

/**
 * Destructor.
 */
ShortImmPSocketResource::~ShortImmPSocketResource() {}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
ShortImmPSocketResource::isShortImmPSocketResource() const {
    return true;
}

/**
 * Return the width of immediate carried by the related bus.
 *
 * @return The width of immediate carried by the related bus.
 */
int
ShortImmPSocketResource::immediateWidth() const {
    return immediateWidth_;
}

/**
 * Return true if the related bus sign extends.
 *
 * @return True if the related bus sign extends.
 */
bool
ShortImmPSocketResource::signExtends() const {
    return signExtends_;
}

/**
 * Return true if the related bus does not sign extend.
 *
 * @return True if the related bus does not sign extend.
 */
bool
ShortImmPSocketResource::zeroExtends() const {
    return !signExtends_;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types.
 *
 * @return true If all resources in related groups are bus resources.
 */
bool
ShortImmPSocketResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0; j < relatedResourceCount(i); j++) {
            if (!relatedResource(i, j).isBusResource()) {
                return false;
            }
        }
    }
    return true;
}
