/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */
#include <iostream>
#include "OutputPSocketResource.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "MoveNode.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "Terminal.hh"
#include "Port.hh"

/**
 * Constructor.
 *
 * @param name Name of resource.
 */
OutputPSocketResource::OutputPSocketResource(const std::string& name, unsigned int initiationInterval) :
    PSocketResource(name, initiationInterval), activeCycle_(-1) {}

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
{
    PSocketResource::assign(cycle, node);
    auto i = storedPorts_.find(instructionIndex(cycle));
    if (i == storedPorts_.end()) {
        const TTAMachine::Port* newPort = &node.move().source().port();
        storedPorts_[instructionIndex(cycle)] = 
            std::make_pair(newPort,1);
    } else {
        i->second.second++;
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
{
    PSocketResource::unassign(cycle, node);
    auto i = storedPorts_.find(instructionIndex(cycle));
    if (i != storedPorts_.end()) {
        if (i->second.second == 1) {
            storedPorts_.erase(i);
        } else {
            i->second.second--;
        }
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
    auto i = storedPorts_.find(instructionIndex(cycle));
    if (mNode.move().source().isFUPort()) {
        if (i != storedPorts_.end()) {
            const TTAMachine::Port* storedP = i->second.first;
            if (&mNode.move().source().port() != storedP) {
                return false;
            }
        }
    }
    if (mNode.move().source().isImmediateRegister()) {
        if (i != storedPorts_.end()) {
            return false;
        }
    }
    if (node.move().source().isGPR()) {
        if (i != storedPorts_.end()) {
            const TTAMachine::Port* storedP =
                i->second.first;
            if (mNode.move().source().port().parentUnit() != 
                storedP->parentUnit()) {
                return false;
            }
        }

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
                    if (node.move().source().port().parentUnit() !=
                        mn->move().source().port().parentUnit() ||
                            node.move().source().index() !=
                            mn->move().source().index()) {
                        return false;
                    } else {
                        continue;
                    }
                    
                }
                if (!node.move().guard().guard().isOpposite(
                        mn->move().guard().guard())) {
                    if (!node.move().source().equals(
                            mn->move().source())) {
                        return false;
                    }
                }
#endif
            }
        }
    }
    activeCycle_ = cycle;
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
                relatedResource(i, j).isBusResource() ||
                relatedResource(i, j).isIUResource())) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Comparison operator.
 * 
 * Favours sockets which have less connections.
 *
 * TODO: precalc/cache these to optimize scheduling time?
 */
bool 
OutputPSocketResource::operator< (const SchedulingResource& other) const {
    const OutputPSocketResource *opsr = 
        dynamic_cast<const OutputPSocketResource*>(&other);
    if (opsr == NULL) {
        return false;
    }

    // first priority is to put reads from same source to same psocket.
    // so favour the one with most moves in current cycle.
    int myCount = 0;
    int otherCount = 0;
    ResourceRecordType::const_iterator myIter = resourceRecord_.find(activeCycle_);
    if (myIter != resourceRecord_.end()) {
        myCount = myIter->second.size();
    }

    ResourceRecordType::const_iterator otherIter = opsr->resourceRecord_.find(opsr->activeCycle_);
    if (otherIter != opsr->resourceRecord_.end()) {
        otherCount = otherIter->second.size();
    }
    if (myCount  < otherCount) {
        return false;
    } else if (myCount > otherCount) {
        return true;
    }

    // favour sockets which have connections to busses with fewest connections
    // calculate the connections..
    int connCount = 0;
    int connCount2 = 0;

    int rrCount2 = relatedResourceCount(2);
    for (int i = 0; i < rrCount2; i++) {
        SchedulingResource& r = relatedResource(2,i);
        connCount += r.relatedResourceCount(0);
    }

    int oRRCount2 = other.relatedResourceCount(2);

    for (int i = 0; i < oRRCount2; i++) {
        SchedulingResource& r = other.relatedResource(2,i);
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
    if (rrCount2 < oRRCount2) {
        return true;
    }
    if (rrCount2 > oRRCount2) {
        return false;
    }

    // then use the default use count, name comparison,
    // but in opposite direction, facouring already used 
    return other.SchedulingResource::operator<(*this);
}

/**
 * Clears bookkeeping of the scheduling resource. 
 * 
 * After this call the state of the resource should be identical to a 
 * newly-created and initialized resource.
 */
void
OutputPSocketResource::clear() {
    PSocketResource::clear();
    storedPorts_.clear();
}
