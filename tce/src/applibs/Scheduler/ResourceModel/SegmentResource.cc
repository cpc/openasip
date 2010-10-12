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
 * @file SegmentResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the SegmentResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "SegmentResource.hh"
#include "MapTools.hh"
#include "Conversion.hh"
#include "Exception.hh"
/**
 * Constructor
 * Creates new resource with defined resource name
 * @param name Name of resource
 */
SegmentResource::SegmentResource(
    const std::string& name) : SchedulingResource(name) {}

/**
 * Empty destructor
 */
SegmentResource::~SegmentResource() {}

/**
 * Test if resource SegmentResource is used in given cycle
 * @param cycle Cycle which to test
 * @return True if SegmentResource is already used in cycle
 */
bool
SegmentResource::isInUse(const int cycle) const {
    if (MapTools::containsKey(resourceRecord_, cycle)) {
        if (MapTools::valueForKey<int>(resourceRecord_, cycle) != 0) {
            return true;
        }
    }
    return false;
}

/**
 * Test if resource SegmentResource is available
 * @param cycle Cycle which to test
 * @return True if SegmentResource is available in cycle
 */
bool
SegmentResource::isAvailable(const int cycle) const {
    return !isInUse(cycle);
}

/**
 * Assign resource to given node for given cycle, without checking PSocket
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * @throw In case assignment is invalid - canAssign fails
 */
void
SegmentResource::assign(const int cycle, MoveNode& node)
    throw (Exception) {
    if (canAssign(cycle, node)) {
        resourceRecord_[cycle] = 1;
        return;
    }
    std::string msg = "Segment ";
    msg += name();
    msg += " can not be assigned in cycle ";
    msg += Conversion::toString(cycle);
    msg += "!";
    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
}

/**
 * Unassign resource from given node for given cycle without checking PSocket
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @throw In case segment was not assigned for given cycle
 */
void
SegmentResource::unassign(const int cycle, MoveNode&)
    throw (Exception) {
    if (isInUse(cycle)) {
        resourceRecord_[cycle] = 0;
        return;
    }
    std::string msg = "Socket ";
    msg += name();
    msg += " was not in use in cycle ";
    msg += Conversion::toString(cycle);
    msg += ", unassign failed!";
    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return true if resource can be assigned for given resource in given cycle
 * without checking PSocket
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
SegmentResource::canAssign(const int cycle, const MoveNode&) const {
    return isAvailable(cycle);
}

/**
 * Return true if resource can be assigned for given resource in given cycle
 * with checking PSocket
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param pSocket Socket that is source or destination and is connected
 * to the segment
 * @return true if node can be assigned to cycle
 */
bool
SegmentResource::canAssign(
    const int cycle,
    const MoveNode& ,
    const SchedulingResource& pSocket) const {
    if (!isAvailable(cycle)) {
        return false;
    } else {
        if (pSocket.isInputPSocketResource() ||
            pSocket.isOutputPSocketResource()) {
            if (hasRelatedResource(pSocket)) {
                return true;
            }
        }
        return false;
    }
}

/**
 * Always return true
 * @return true
 */
bool
SegmentResource::isSegmentResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true If all resources in dependent groups are
 *              Segment resources
 */
bool
SegmentResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (!dependentResource(i, j).isSegmentResource()) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types
 * @return true If all resources in related groups are
 *              Input or Output PSockets
 */
bool
SegmentResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!(relatedResource(i, j).isOutputPSocketResource() ||
                relatedResource(i, j).isInputPSocketResource())) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Clears bookkeeping of the scheduling resource. 
 * 
 * After this call the state of the resource should be identical to a 
 * newly-created and initialized resource.
 */
void
SegmentResource::clear() {
    SchedulingResource::clear();
    resourceRecord_.clear();
}
