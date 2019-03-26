/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file BusResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the BusResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Heikki Kultala 2014 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "BusResource.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Conversion.hh"
#include "MoveNode.hh"
#include "Port.hh"
#include "Terminal.hh"
#include "Move.hh"

/**
 * Constructor
 * Creates new resource with defined name
 * @param name Name of resource
 * @param width short immediate width of the bus
 * @param limmSlotCount how many itemplates use this bus in limm field
 */
BusResource::BusResource(
    const std::string& name, int width, int limmSlotCount, int guardCount,
    int immSize, int socketCount, unsigned int initiationInterval) : 
    SchedulingResource(name, initiationInterval), busWidth_(width), 
    limmSlotCount_(limmSlotCount), guardCount_(guardCount), 
    immSize_(immSize), socketCount_(socketCount) {
}

/**
 * Empty destructor
 */
BusResource::~BusResource() {}

/**
 * Test if resource BusResource is used in given cycle
 * @param cycle Cycle which to test
 * @return True if any of segments of bus isInUse in given cycle
 */
bool
BusResource::isInUse(const int cycle) const {
    ResourceRecordType::const_iterator i = 
        resourceRecord_.find(instructionIndex(cycle));
    if (i != resourceRecord_.end() && i->second != 0) {
        return true;
    }
    return false;
}

/**
 * Test if resource BusResource is available
 * @param cycle Cycle which to test
 * @return False if all the segments of bus are InUse
 */
bool
BusResource::isAvailable(const int cycle) const {
    return !isInUse(cycle);
}

/**
 * Test if resource BusResource is available.
 * @param cycle Cycle which to test
 * @return False if all the segments of bus are InUse
 */
bool
BusResource::isAvailable(
    const int cycle,
    const SchedulingResource& inputPSocket,
    const SchedulingResource& outputPSocket) const {

    if (!(hasRelatedResource(inputPSocket) &&
        hasRelatedResource(outputPSocket))) {
        // BusBroker does not need to test if sockets
        // are connected to the bus...
       return false;
    }

    return isAvailable(cycle);
}

/**
 * Assign resource to given node for given cycle without testing
 * Input or Output PSocket, assign ALL segments of bus.
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * @throw In case assignment can not be done
 */
void
BusResource::assign(const int cycle, MoveNode& node)
    throw (Exception) {

   if (canAssign(cycle, node)) {
        resourceRecord_[instructionIndex(cycle)] = 1;
        increaseUseCount();
        return;
    }
}

/**
 * Unassign resource from given node for given cycle without testing
 * Input or Output PSocket, unassign all segments of bus
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @throw In case bus was not assigned
 */
void
BusResource::unassign(const int cycle, MoveNode&)
    throw (Exception) {
    if (isInUse(cycle)) {
        resourceRecord_[instructionIndex(cycle)] = 0;
        return;
    } else{
        std::string msg = "Bus ";
        msg += name();
        msg += " can not be unassigned in cycle ";
        msg += Conversion::toString(cycle);
        msg += ", it was not in use!";
        throw ModuleRunTimeError(__FILE__, __LINE__,__func__, msg);
    }
}

/**
 * Return true if resource can be assigned for given node in given cycle
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
BusResource::canAssign(const int cycle, const MoveNode&) const {
    return isAvailable(cycle);
}

/**
 * Return true if resource can be assigned for given node in given cycle
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param inputPSocket PSocket connected to source of move
 * @param outputPSocket PSocket connected to destination of move
 * @return true if node can be assigned to cycle
 */
bool
BusResource::canAssign(
    const int cycle,
    const MoveNode& node,
    const SchedulingResource& inputPSocket,
    const SchedulingResource& outputPSocket) const {

    if (!isAvailable(cycle, inputPSocket, outputPSocket)) {
        return false;
    }
    
    // TODO: find better way how to check various combinations of different
    // source and destination port widths, such as writes to boolean register
    bool sourceWontFit = false;
    if (!node.isSourceConstant()) {
        if (node.move().source().port().width() > busWidth_) {
            sourceWontFit = true;
        }
    } else {
        sourceWontFit = true; // invalid extension of immediate
    }
    if (sourceWontFit &&
        node.move().destination().port().width() > busWidth_) {
        return false;
    }

    if (!inputPSocket.isOutputPSocketResource() ||
        !hasRelatedResource(inputPSocket)) {
        return false;
    }
    if (!outputPSocket.isInputPSocketResource() ||
        !hasRelatedResource(outputPSocket)) {
        return false;
    }

    return true;
}

/**
 * Return allways true
 * @return true
 */
bool
BusResource::isBusResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true If all resources in dependent groups are
 *              Segment resources
 */
bool
BusResource::validateDependentGroups() {
    return true;
}

/**
 * Tests if all resources in related resource groups are of
 * proper types
 * @return true If all resources in related resource groups are
 *              PSockets
 */
bool
BusResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i);
            j < count;
            j++) {
            if (!(relatedResource(i, j).isInputPSocketResource() ||
                relatedResource(i, j).isOutputPSocketResource()))
                return false;
        }
    }
    return true;
}

/**
 * Comparison operator.
 * 
 * Favours busses which have less limm slots associated to then in
 * instruction templates, ie. busses which do not get into way of
 * limm writes.
 */
bool 
BusResource::operator< (const SchedulingResource& other) const {
    const BusResource *busr = static_cast<const BusResource*>(&other);
    if (busr == NULL) {
        return false;
    }

    // then try to use busses without guards, if possible.
    if (guardCount_ < busr->guardCount_) {
        return true;
    }
    if (guardCount_ > busr->guardCount_) {
        return false;
    }

    // then favour busses with less sockets.
    if (socketCount_ < busr->socketCount_) {
        return true;
    }
    if (socketCount_ > busr->socketCount_) {
        return false;
    }

    // first priority are limm slots.
    if (limmSlotCount_ < busr->limmSlotCount_) {
        return true;
    } 
    if (limmSlotCount_ > busr->limmSlotCount_) {
        return false;
    }

    // then favour busses with smallest immediate.
    if (immSize_ < busr->immSize_) {
        return true;
    }
    if (immSize_ > busr->immSize_) {
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
BusResource::clear() {
    SchedulingResource::clear();
    resourceRecord_.clear();
}
