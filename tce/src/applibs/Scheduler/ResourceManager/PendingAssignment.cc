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
 * @file PendingAssignment.cc
 *
 * Implementation of PendingAssignment class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "ResourceBroker.hh"
#include "PendingAssignment.hh"
#include "MoveNode.hh"

using std::string;

/**
 * Constructor.
 */
PendingAssignment::PendingAssignment(ResourceBroker& broker):
    broker_(broker), cycle_(-1), node_(NULL), assignmentTried_(false),
    lastTriedAssignment_(-1) {
}

/**
 * Destructor.
 */
PendingAssignment::~PendingAssignment(){
}

/**
 * Return the broker of this pending assignment.
 *
 * @return The broker of this pending assignment.
 */
ResourceBroker&
PendingAssignment::broker() {
    return broker_;
}

/**
 * Record the input node to which resources have to be assigned or
 * allocated, and the cycle in which the node should be placed.
 *
 * @param cycle Cycle.
 * @param node Node.
 */
void
PendingAssignment::setRequest(int cycle, MoveNode& node,
                              const TTAMachine::Bus* bus,
                              const TTAMachine::FunctionUnit* srcFU,
                              const TTAMachine::FunctionUnit* dstFU,
                              int immWriteCycle,
                              const TTAMachine::ImmediateUnit* immu,
                              int immRegIndex) {
    cycle_ = cycle;
    node_ = &node;
    bus_ = bus;
    srcFU_ = srcFU;
    dstFU_ = dstFU;
    immWriteCycle_ = immWriteCycle;
    immu_ = immu;
    immRegIndex_ = immRegIndex;
    lastTriedAssignment_ = -1;
}

/**
 * Return true if, for the currently applied assignments, there exists
 * at least one tentative assignment which has not been tried yet.
 *
 * @return True if, for the currently applied assignments, there exists
 * at least one tentative assignment which has not been tried yet.
 */
bool
PendingAssignment::isAssignmentPossible() {
    if (lastTriedAssignment_ == -1) {
        availableResources_ = broker_.allAvailableResources(
	    cycle_, *node_, bus_, srcFU_, dstFU_, immWriteCycle_, immu_,
	    immRegIndex_);
        // Sorts candidate set of resources by their use count or name if
        // the use counts are identical
        availableResources_.sort();
    }
    return lastTriedAssignment_ < availableResources_.count() - 1;
}

/**
 * Try to assign the next possible assignment found by current
 * broker.
 *
 * @exception ModuleRunTimeError If this PendingAssignment has run out of
 * possible assignments.
 */
void
PendingAssignment::tryNext() {

    lastTriedAssignment_ += 1;

    if (availableResources_.count() == 0 ||
        lastTriedAssignment_ >= availableResources_.count()) {
        string msg = "Ran out of possible assignments!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    broker_.assign(
        cycle_, *node_, availableResources_.resource(lastTriedAssignment_),
        immWriteCycle_, immRegIndex_);
    assignmentTried_ = true;
}

/**
 * Unassign the resource of this pending assignment currently assigned
 * to the node.
 *
 * Last tried assignment is remembered.
 *
 * @exception ModuleRunTimeError If no resource in the set of
 * resources of this pending assignment is currently assigned to the
 * node.
 */
void
PendingAssignment::undoAssignment() {
    if (broker_.isAlreadyAssigned(cycle_, *node_, bus_)) {
        broker_.unassign(*node_);
    } else {
        string msg = "No resource in the set of resources of this pending"
            "assignment was assigned to the given node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Clear out the record of the possible resource assignments of the
 * current broker that have been already tried.
 *
 * If one of the possible resources is still assigned to the node,
 * unassign it.
 */
void
PendingAssignment::forget() {
    if (broker_.isAlreadyAssigned(cycle_, *node_, bus_)) {
        broker_.unassign(*node_);
    }
    lastTriedAssignment_ = -1;
}

/**
 * Clears the pending assignment. 
 *
 * Does not unassign anything.
 */
void
PendingAssignment::clear() {
    cycle_ = -1;
    node_ = NULL;
    immu_ = NULL;
    immWriteCycle_ = -1;
    immRegIndex_ = -1;

    assignmentTried_ = false;
    lastTriedAssignment_ = -1;
    availableResources_.clear();
    bus_ = NULL;
}
