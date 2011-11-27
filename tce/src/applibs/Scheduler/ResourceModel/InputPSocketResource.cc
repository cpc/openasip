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
 * @file InputPSocketResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the InputPSocketResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "InputPSocketResource.hh"
#include "MoveNode.hh"
#include "MoveGuard.hh"
#include "Guard.hh"

/**
 * Constructor defining resource name
 * @param name Name of resource
 */
InputPSocketResource::InputPSocketResource(const std::string& name, unsigned int initiationInterval) :
    PSocketResource(name, initiationInterval) {}

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

/**
 * Return true if resource can be assigned for given resource in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
InputPSocketResource::canAssign(const int cycle, const MoveNode& node)
    const {

    ResourceRecordType::const_iterator iter = resourceRecord_.find(cycle);
    if (iter != resourceRecord_.end()) {
        std::set<MoveNode*> movesInCycle = iter->second;
        for (std::set<MoveNode*>::iterator it = movesInCycle.begin();
             it != movesInCycle.end(); it++) {
#ifdef NO_OVERCOMMIT
            return false;
#else
            MoveNode* mn = *it;
            if (node.move().isUnconditional() || 
                mn->move().isUnconditional()) {
                return false;
            }
            if (!node.move().guard().guard().isOpposite(
                    mn->move().guard().guard())) {
                return false;
            }
#endif
        }
    }
    return true;
}        

/**
 * Comparison operator.
 * 
 * Favours sockets which have less connections.
 */
bool 
InputPSocketResource::operator< (const SchedulingResource& other) const {
    const InputPSocketResource *ipsr = 
        dynamic_cast<const InputPSocketResource*>(&other);
    if (ipsr == NULL) {
        return false;
    }
    
    // favour sockets which have connections to busses with fewest connections
    // calculate the connections..
    int connCount = 0;
    int connCount2 = 0;

    for (int i = 0; i < relatedResourceCount(1); i++) {
        SchedulingResource& r = relatedResource(1,i);
        connCount += r.relatedResourceCount(0);
    }

    for (int i = 0; i < other.relatedResourceCount(1); i++) {
        SchedulingResource& r = other.relatedResource(1,i);
        connCount2 += r.relatedResourceCount(0);
    }
    
    // then the comparison.
    if (connCount < connCount2) {
        return true;
    }

    if (connCount > connCount2) {
        return false;
    }


    // favour sockets with less buses.
    if (relatedResourceCount(1) < ipsr->relatedResourceCount(1)) {
        return true;
    }
    if (relatedResourceCount(1) > ipsr->relatedResourceCount(1)) {
        return false;
    }

    // then use the default use count, name comparison,
    // but in opposite direction, facouring already used 
    return other.SchedulingResource::operator<(*this);
}
