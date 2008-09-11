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
 * @file FUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the abstract FUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUResource.hh"
#include "Application.hh"
#include "Conversion.hh"
/**
 * Constructor defining resource name
 * @param name Name of resource
 */
FUResource::FUResource(const std::string& name) : SchedulingResource(name) {}

/**
 * Empty destructor
 */
FUResource::~FUResource() {}

/**
 * Test if resource FUResource is used in given cycle, meaning
 * InputPSocket or OutputPSocket is in use or ExecutionPipeline
 * is inUse.
 * @param cycle Cycle which to test
 * @return True if FUResource is already used in cycle
 */
bool
FUResource::isInUse(const int cycle) const {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isInUse(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Test if resource FUResource is available.
 * Not all of the PSocket are inUse.
 * @param cycle Cycle which to test
 * @return True if FUResource is available in cycle
 */
bool
FUResource::isAvailable(const int cycle) const {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isInputPSocketResource() ||
                dependentResource(i, j).isOutputPSocketResource()) {
                if (!dependentResource(i, j).isInUse(cycle)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Assign resource to given node for given cycle
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 */
void
FUResource::assign(const int, MoveNode&)
    throw (Exception) {
    // Implemented in derived classes
    abortWithError("assign of FUResource called!");
}

/**
 * Unassign resource from given node for given cycle
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 */
void
FUResource::unassign(const int, MoveNode&)
    throw (Exception) {
    // Implemented in derived classes
    abortWithError("unassign of FUResource called!");
}

/**
 * Return true if resource can be assigned for given resource in given cycle
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
FUResource::canAssign(const int, const MoveNode&) const {
    // Implemented in derived classes
    abortWithError("canAssign of FUResource called!");
    return false;
}
