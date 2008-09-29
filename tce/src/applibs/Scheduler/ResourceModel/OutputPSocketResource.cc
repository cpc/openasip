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
 * @file OutputPSocketResource.cc
 *
 * Implementation of OutputPSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */
#include <iostream>
#include "OutputPSocketResource.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "MoveNode.hh"

/**
 * Constructor.
 *
 * @param name Name of resource.
 */
OutputPSocketResource::OutputPSocketResource(const std::string& name) :
    PSocketResource(name) {}

/**
 * Destructor.
 */
OutputPSocketResource::~OutputPSocketResource() {}

/**
 * Test if resource OutputPSocketResource is available.
 *
 * @param cycle Cycle which to test.
 * @return Always true, multiple reads are possible.
 */
bool
OutputPSocketResource::isAvailable(const int) const {
    return true;
}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
OutputPSocketResource::isOutputPSocketResource() const {
    return true;
}
/**
 * Assign resource to given node for given cycle.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode to assign
 */
void
OutputPSocketResource::assign(const int cycle, MoveNode& node)
    throw (Exception) {
    PSocketResource::assign(cycle, node);
    if (node.move().source().isFUPort() ||
        node.move().source().isImmediateRegister()) {
        if (!MapTools::containsKey(storedPorts_, cycle)) {
            const TTAMachine::Port* newPort = &node.move().source().port();
            storedPorts_.insert(std::pair<int, const TTAMachine::Port*>(
                cycle, newPort));
        }
    }
}

/**
 * Unassign resource from given node for given cycle.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 */
void
OutputPSocketResource::unassign(const int cycle, MoveNode& node)
    throw (Exception) {
    PSocketResource::unassign(cycle, node);
    if (MapTools::containsKey(storedPorts_, cycle)) {
        storedPorts_.erase(cycle);
    }
}

/**
 * Return true if resource can be assigned for given resource in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
OutputPSocketResource::canAssign(const int cycle, const MoveNode& node)
    const {
    MoveNode& mNode = const_cast<MoveNode&>(node);
    if (mNode.move().source().isFUPort()) {
        if (MapTools::containsKey(storedPorts_, cycle)) {
            const TTAMachine::Port* storedP =
                MapTools::valueForKey<const TTAMachine::Port*>(
                    storedPorts_, cycle);
            if (&mNode.move().source().port() != storedP) {
                return false;
            }
        }
    }
    if (mNode.move().source().isImmediateRegister()) {
        if (MapTools::containsKey(storedPorts_, cycle)) {
            return false;
        }
    }
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types.
 *
 * @return True if all dependent groups are empty.
 */
bool
OutputPSocketResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        if (dependentResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types.
 *
 * @return True if all resources in related groups are Segment or OutputFU
 * or IU resources.
 */
bool
OutputPSocketResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!(relatedResource(i, j).isOutputFUResource() ||
                relatedResource(i, j).isSegmentResource() ||
                relatedResource(i, j).isIUResource())) {
                return false;
            }
        }
    }
    return true;
}
