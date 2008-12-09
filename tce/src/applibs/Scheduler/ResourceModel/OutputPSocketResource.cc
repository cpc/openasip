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
