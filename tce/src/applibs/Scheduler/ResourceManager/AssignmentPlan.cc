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
 * @file AssignmentPlan.cc
 *
 * Implementation of AssignmentPlan class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "AssignmentPlan.hh"
#include "PendingAssignment.hh"
#include "AssocTools.hh"
#include "ResourceBroker.hh"
#include "MoveNode.hh"

using std::string;

/**
 * Constructor.
 */
AssignmentPlan::AssignmentPlan():
    node_(NULL), cycle_(0), currentBroker_(0), resourceFound_(false) {
}

/**
 * Destructor.
 */
AssignmentPlan::~AssignmentPlan(){
    AssocTools::deleteAllItems(assignments_);
}

/**
 * Insert a resource broker in the sequence of brokers.
 *
 * @param broker Resource broker to be inserted.
 */
void
AssignmentPlan::insertBroker(ResourceBroker& broker) {
    assignments_.push_back(new PendingAssignment(broker));
    brokers_.push_back(&broker);
}

/**
 * Record the input node to which resources have to be assigned or allocated
 * and the cycle in which the node should be placed.
 *
 * @param node The node of current assignment request.
 * @param cycle The cycle in which the node should be placed.
 */
void
AssignmentPlan::setRequest(int cycle, MoveNode& node) {

    if (node.isPlaced() && node.cycle() != cycle) {
        string msg = "Node already placed on different cycle!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    currentBroker_ = 0;
    node.setCycle(cycle);

    cycle_ = cycle;
    node_ = &node;

    // optimise broker sequence by disabling not applicable brokers
    applicableAssignments_.clear();
    for (unsigned int i = 0; i < assignments_.size(); i++) {
        if (assignments_[i]->broker().isApplicable(node)) {
			assignments_[i]->setRequest(cycle, node);
            applicableAssignments_.push_back(assignments_[i]);
        }
    }
    if (applicableAssignments_.empty()) {
        string msg = "No applicable brokers found for assignment!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Return the first broker to be evaluated during an assignment process.
 *
 * @return The first broker to be evaluated during an assignment process.
 */
ResourceBroker&
AssignmentPlan::firstBroker() {
    if (applicableAssignments_.empty()) {
        string msg = "No applicable brokers found for assignment!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    return applicableAssignments_[0]->broker();
}

ResourceBroker&
AssignmentPlan::nextBroker(ResourceBroker& pos) {

    if (applicableAssignments_.empty()) {
        string msg = "No applicable brokers found for assignment!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    unsigned int i = 0;
    while (i < applicableAssignments_.size() - 1) {
        if (&applicableAssignments_[i]->broker() == &pos) {
            return applicableAssignments_[++i]->broker();
        }
        i++;
    }

    string msg = "Given broker not found!";
    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return the last broker to be evaluated during an assignment process.
 *
 * @return The last broker to be evaluated during an assignment process.
 */
ResourceBroker&
AssignmentPlan::lastBroker() {
    if (applicableAssignments_.empty()) {
        string msg = "No applicable brokers found for assignment!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    return applicableAssignments_[applicableAssignments_.size() - 1]->broker();
}

/**
 * Return the current broker, that is, the broker whose resources are
 * being evaluated during an assignment process.
 *
 * @return The current broker.
 */
ResourceBroker&
AssignmentPlan::currentBroker() {
    if (applicableAssignments_.empty()) {
        string msg = "No applicable brokers found for assignment!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    return applicableAssignments_[currentBroker_]->broker();
}

/**
 * Move to the next resource broker in the sequence.
 */
void
AssignmentPlan::advance() {
    if (resourceFound_) {
        currentBroker_++;
    } else {
        string msg = "Tried to advance before a valid assignment was made!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    if (currentBroker_ >= static_cast<int>(applicableAssignments_.size())) {
        string msg = "Advanced beyond last resource broker!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Unassign (if needed) the resource of current broker from the node
 * and then move to previous resource broker in the sequence.
 *
 * The pending assignments of the current node are forgotten.
 */
void
AssignmentPlan::backtrack() {
    // forget assignment history and assignments for current broker
    PendingAssignment* assignment = applicableAssignments_[currentBroker_];
    assignment->forget();
    if (assignment->broker().isAlreadyAssigned(cycle_, *node_)) {
        assignment->undoAssignment();
    }
    currentBroker_--;
    if (currentBroker_ < 0) {
        string msg = "Backtracked beyond first resource broker!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    } else {
        // undo possible assignments for broker we backtracked to
        assignment = applicableAssignments_[currentBroker_];
        if (assignment->broker().isAlreadyAssigned(cycle_, *node_)) {
            assignment->undoAssignment();
        }
    }
}

/**
 * Unassign (if needed) the resource of current broker from the node
 * and then assign the next resource from the same list of pending
 * assignments.
 */
void
AssignmentPlan::tryNextAssignment() {
    PendingAssignment* assignment = applicableAssignments_[currentBroker_];
    if (assignment->broker().isAlreadyAssigned(cycle_, *node_)) {
        assignment->undoAssignment();
    }
    assignment->tryNext();
    resourceFound_ = true;
}

/**
 * Return true if at least one tentative assignment is possible with
 * the current broker and the tentative assignments currently applied
 * from the preceding resource brokers.
 *
 * There are two reasons why no possible assignments are left with the
 * current broker: because all pending assignments have been tried, or
 * because no valid assignment at all is possible.
 *
 * @return True if at least one tentative assignment is possible with
 * the current broker
 */
bool
AssignmentPlan::isTestedAssignmentPossible() {
    return applicableAssignments_[currentBroker_]->isAssignmentPossible();
}

/**
 * Unassign as needed all the resources tentatively assigned by all brokers.
 *
 * Reset the current broker position to the first one.
 */
void
AssignmentPlan::resetAssignments() {
    for (unsigned int i = 0; i < applicableAssignments_.size(); i++) {
        applicableAssignments_[i]->forget();
    }
    currentBroker_ = 0;
    node_->unsetCycle();
}

/**
 * Unassign as needed all the resources tentatively assigned to the
 * given node.
 *
 * Given node needs to be placed on a cycle.
 *
 * @param node Node to unassign.
 * @exception InvalidData If node is not placed on a cycle.
 */
void
AssignmentPlan::resetAssignments(MoveNode& node) throw (InvalidData) {
    if (!node.isPlaced()) {
        string msg = "Node is not placed in a cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->isApplicable(node)) {
            brokers_[i]->unassign(node);
        }
    }
    node.unsetCycle();
}

/**
 * Return the number of brokers in assignment plan.
 *
 * @return The number of brokers in assignment plan.
 */
int
AssignmentPlan::brokerCount() const {
    return brokers_.size();
}

/**
 * Return the broker in the given index.
 *
 * @param index Index of broker.
 * @return The broker in the given index.
 * @exception OutOfRange if index is out of bounds.
 */
ResourceBroker&
AssignmentPlan::broker(int index) const throw (OutOfRange) {
    if (index < 0 || index >= static_cast<int>(brokers_.size())) {
        string msg = "Broker index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    } else {
        return *brokers_[index];
    }
}
