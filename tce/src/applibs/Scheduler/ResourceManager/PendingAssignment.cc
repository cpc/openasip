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
 * @file PendingAssignment.cc
 *
 * Implementation of PendingAssignment class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
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
    broker_(broker), cycle_(0), node_(NULL), assignmentTried_(false),
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
PendingAssignment::setRequest(int cycle, MoveNode& node) {
	cycle_ = cycle;
	node_ = &node;
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
        availableResources_ = broker_.allAvailableResources(cycle_, *node_);
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
PendingAssignment::tryNext()
    throw (ModuleRunTimeError) {

    lastTriedAssignment_ += 1;

    if (availableResources_.count() == 0 ||
        lastTriedAssignment_ >= availableResources_.count()) {
        string msg = "Ran out of possible assignments!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

	broker_.assign(
        cycle_, *node_, availableResources_.resource(lastTriedAssignment_));
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
    if (broker_.isAlreadyAssigned(cycle_, *node_)) {
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
    if (broker_.isAlreadyAssigned(cycle_, *node_)) {
        broker_.unassign(*node_);
    }
    lastTriedAssignment_ = -1;
}
