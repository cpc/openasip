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
 * @file BusResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the BusResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "BusResource.hh"
#include "SegmentResource.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Conversion.hh"
#include "MoveNode.hh"
#include "Port.hh"
/**
 * Constructor
 * Creates new resource with defined name
 * @param name Name of resource
 */
BusResource::BusResource(const std::string& name, int width) : 
    SchedulingResource(name), busWidth_(width) {    
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
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i);
            j < count;
            j++) {
            if (dependentResource(i, j).isInUse(cycle)) {
                return true;
            }
        }
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
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i);
            j < count;
            j++) {
            if (!dependentResource(i, j).isInUse(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Test if resource BusResource is used in given cycle, using input
 * and output PSocket - testing range between them.
 *
 * @param cycle Cycle which to test
 * @param inputPSocket Source PSocket of move
 * @param outputPSocket Destination PSocket of move
 * @return True if any of segments of bus isInUse in given cycle
 */
bool
BusResource::isInUse(
    const int cycle,
    const SchedulingResource& inputPSocket,
    const SchedulingResource& outputPSocket) const {

    if (!(hasRelatedResource(inputPSocket) &&
        hasRelatedResource(outputPSocket))) {
        std::string msg = "Bus \"";
        msg += name();
        msg += "\" is not connected to given PSockets!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i);
            j < count;
            j++) {
            if (dependentResource(i, j).hasRelatedResource(inputPSocket)) {
                SegmentResource* start = NULL;
                start =
                    dynamic_cast<SegmentResource*>(&dependentResource(i,j));
                if (start->isInUse(cycle)) {
                    return true;
                }
                SegmentResource* destination = start;
                while (!destination->hasRelatedResource(outputPSocket)) {
                    if (destination->isInUse(cycle)) {
                        return true;
                    }
                    if (destination->dependentResourceGroupCount() < 2 ||
                        destination->dependentResourceCount(1) == 0) {
                        return false;
                    }
                    // get next destination - terrible semantics
                    // asks destinations for it's neighbour segment
                    destination =
                        dynamic_cast<SegmentResource*>(
                        &destination->dependentResource(1, 0));
                }
                return false;
            }
            if (dependentResource(i, j).hasRelatedResource(outputPSocket)) {
                SegmentResource* start = NULL;
                start = dynamic_cast<SegmentResource*>
                    (&dependentResource(i, j));
                if (start->isInUse(cycle)) {
                    return true;
                }
                SegmentResource* source = start;
                while (!source->hasRelatedResource(inputPSocket)) {
                    if (source->isInUse(cycle)) {
                        return true;
                    }
                    if (source->dependentResourceGroupCount() < 1 ||
                        source->dependentResourceCount(0) == 0) {
                        return false;
                    }
                    // get next destination - terrible semantics
                    // ask destination for it's neighbour segment
                    source = dynamic_cast<SegmentResource*>
                        (&source->dependentResource(1, 0));
                }
                return false;
            }
        }
    }
    return false;
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

    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).hasRelatedResource(inputPSocket)) {
                SegmentResource* start = NULL;
                start =
                    dynamic_cast<SegmentResource*>(&dependentResource(i, j));
                if (!start->isAvailable(cycle)) {
                    return false;
                }
                SegmentResource* destination = start;
                while (!(destination->isAvailable(cycle) &&
                    destination->hasRelatedResource(outputPSocket))) {
                    if (!destination->isAvailable(cycle)) {
                        return false;
                    }
                    if (destination->dependentResourceGroupCount() < 2 ||
                        destination->dependentResourceCount(1) == 0) {
                        return false;
                    }
                    // get next destination - terrible semantics
                    destination =
                        dynamic_cast<SegmentResource*>
                            (&destination->dependentResource(1, 0));
                }
                return true;
            }
            if (dependentResource(i, j).hasRelatedResource(outputPSocket)) {
                SegmentResource* start = NULL;
                start = dynamic_cast<SegmentResource*>
                    (&dependentResource(i, j));
                if (!start->isAvailable(cycle)) {
                    return false;
                }
                SegmentResource* source = start;
                while (!(source->isAvailable(cycle) &&
                    source->hasRelatedResource(inputPSocket))) {
                    if (!source->isAvailable(cycle)) {
                        return false;
                    }
                    if (source->dependentResourceGroupCount() < 1 ||
                        source->dependentResourceCount(0) == 0) {
                        return false;
                    }
                    // get next source - terrible semantics
                    source = dynamic_cast<SegmentResource*>
                        (&source->dependentResource(0, 0));
                }
                return true;
            }
        }
    }
    return false;
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
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i);
            j < count ;
            j++) {
            dependentResource(i, j).assign(cycle, node);
        }
    }
    increaseUseCount();
}

/**
 * Assign resource to given node for given cycle from inputPSocket
 * to OutputPSocket
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * @param inputPSocket PSocket connected to source of move
 * @param outputPSocket PSocket connected to destination of move
 * @throw In case assignment can not be done
 */
void
BusResource::assign(
    const int cycle,
    MoveNode& node,
    const SchedulingResource& inputPSocket,
    const SchedulingResource& outputPSocket)
    throw (Exception) {
    std::vector<SegmentResource*> segmentPath;
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count  ; j++) {
            SegmentResource* start = NULL;
            start = dynamic_cast<SegmentResource*>
                (&dependentResource(i, j));
            if (start->canAssign(cycle, node, inputPSocket)) {
                SegmentResource* destination = start;
                while (!destination->canAssign(cycle, node, outputPSocket)) {
                    destination = dynamic_cast<SegmentResource*>
                        (&destination->dependentResource(1, 0));
                    segmentPath.push_back(destination);
                }
                start->assign(cycle, node);
                for (unsigned int i = 0; i < segmentPath.size(); i++) {
                    segmentPath[i]->assign(cycle, node);
                }
                increaseUseCount();
                return;
            } else {
                // if first segment found is connected to output socket
                // proceed as above in reversed direction
                if (start->canAssign(cycle, node, outputPSocket)) {
                    SegmentResource* source = start;
                    while (!source->canAssign(cycle, node, inputPSocket)) {
                        source = dynamic_cast<SegmentResource*>
                            (&source->dependentResource(0, 0));
                        segmentPath.push_back(source);
                    }
                    start->assign(cycle, node);
                    for (unsigned int i = 0; i < segmentPath.size(); i++) {
                        segmentPath[i]->assign(cycle, node);
                    }
                    increaseUseCount();
                    return;
                }
            }
        }
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
BusResource::unassign(const int cycle, MoveNode& node)
    throw (Exception) {
    if (isInUse(cycle)) {
        for (int i = 0; i < dependentResourceGroupCount(); i++) {
            for (int j = 0, count = dependentResourceCount(i);
                j < count;
                j++) {
                dependentResource(i, j).unassign(cycle, node);
                decreaseUseCount();
            }
        }
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
 * Unassign resource from given node for given cycle from InputPSocket
 * to OutputPSocket
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @param inputPSocket PSocket connected to source of move
 * @param outputPSocket PSocket connected to destination of move
 * @throw In case bus was not assigned
 */
void
BusResource::unassign(
    const int cycle,
    MoveNode& node,
    const SchedulingResource& inputPSocket,
    const SchedulingResource& outputPSocket)
    throw (Exception) {

    if (isInUse(cycle, inputPSocket, outputPSocket)) {
        // There is no chance to test if this node was actually assigned
        // to this range - it must be done in broker
        for (int i = 0; i < dependentResourceGroupCount(); i++) {
            for (int j = 0, count = dependentResourceCount(i);
                j < count;
                j++) {
                SegmentResource* start = dynamic_cast<SegmentResource*>
                    (&dependentResource(i, j));
                if (start->hasRelatedResource(inputPSocket)) {
                    SegmentResource* destination = start;
                    start->unassign(cycle, node);
                    while (!destination->hasRelatedResource(outputPSocket)) {
                        destination = dynamic_cast<SegmentResource*>
                            (&destination->dependentResource(1, 0));
                        destination->unassign(cycle, node);
                    }
                    decreaseUseCount();
                    return;
                } else {
                    // if first segment found is connected to output socket
                    // proceed as above in reversed direction
                    if (start->hasRelatedResource(outputPSocket)) {
                        SegmentResource* source = start;
                        start->unassign(cycle, node);
                        while (!source->hasRelatedResource(inputPSocket)) {
                            source = dynamic_cast<SegmentResource*>
                                (&source->dependentResource(0, 0));
                            source->unassign(cycle, node);
                        }
                        decreaseUseCount();
                        return;
                    }
                }
            }
        }
    } else {
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
    }
    if (sourceWontFit &&
        node.move().destination().port().width() > busWidth_) {
        return false;
    }

    // Dependent segments are searched for source and destination psocket
    // connection, segment chain is unidrectional!
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            SegmentResource* start = dynamic_cast<SegmentResource*>
                (&dependentResource(i, j));
            if (start->canAssign(cycle, node, inputPSocket)) {
                SegmentResource* destination = start;
                while (!destination->canAssign(cycle, node, outputPSocket)) {
                    if (!destination->isAvailable(cycle)) {
                        return false;
                    }
                    // segment has no destination segment, it is last
                    // in chain of segments for bus
                    if (destination->dependentResourceGroupCount() < 2 ||
                        destination->dependentResourceCount(1) == 0) {
                        return false;
                    }
                    // try next destination - terrible semantics
                    destination = dynamic_cast<SegmentResource*>
                        (&destination->dependentResource(1, 0));
                }
                return true;
            } else {
                // if first segment found is connected to output socket
                // proceed as above in reversed direction
                if (start->canAssign(cycle, node, outputPSocket)) {
                    SegmentResource* source = start;
                    while (!source->canAssign(cycle, node, inputPSocket)) {
                        if (! source->isAvailable(cycle)) {
                            return false;
                        }
                        // segment has no source segment, it is first
                        // in chain of segments for bus
                        if (source->dependentResourceGroupCount() < 1 ||
                            source->dependentResourceCount(0) == 0) {
                            return false;
                        }
                        // try next destination - terrible semantics
                        source = dynamic_cast<SegmentResource*>
                            (&source->dependentResource(0, 0));
                    }
                    return true;
                }
            }
        }
    }
    return false;
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
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i);
            j < count;
            j++) {
            if (!dependentResource(i, j).isSegmentResource())
                return false;
        }
    }
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
