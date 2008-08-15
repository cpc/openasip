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
 * @file InputPSocketResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the InputPSocketResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "InputPSocketResource.hh"

/**
 * Constructor defining resource name
 * @param name Name of resource
 */
InputPSocketResource::InputPSocketResource(const std::string& name) :
    PSocketResource(name) {}

/**
 * Empty destructor
 */
InputPSocketResource::~InputPSocketResource() {}

/**
 * Allways return true
 * @return true
 */
bool
InputPSocketResource::isInputPSocketResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true Allways true, dep. groups are empty
 */
bool
InputPSocketResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        if (dependentResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types
 * @return true If all resources in related groups are
 *              Segment or InputFU resources
 */
bool
InputPSocketResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!(relatedResource(i, j).isInputFUResource() ||
                relatedResource(i, j).isSegmentResource()))
                return false;
        }
    }
    return true;
}
