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
 * @file AssignmentPlan.cc
 *
 * Implementation of AssignmentPlan class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "TCEString.hh"
#include "Application.hh"
#include "AssignmentPlan.hh"
#include "PendingAssignment.hh"
#include "AssocTools.hh"
#include "ResourceBroker.hh"
#include "MoveNode.hh"
#include "ResourceManager.hh"
#include "Move.hh"

using std::string;

/**
 * Constructor.
 */
AssignmentPlan::AssignmentPlan():
    node_(NULL), cycle_(0), currentBroker_(0), resourceFound_(false),
    lastTriedNode_(NULL), lastTriedCycle_(-1) {
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
AssignmentPlan::setRequest(int cycle, MoveNode& node,
                           const TTAMachine::Bus* bus,
                           const TTAMachine::FunctionUnit* srcFU,
                           const TTAMachine::FunctionUnit* dstFU,
                           int immWriteCycle,
                           const TTAMachine::ImmediateUnit* immu,
                           int immRegIndex) {

    if (node.isPlaced() && node.cycle() != cycle) {
        TCEString msg = "Node: " + node.toString();
        msg << "already placed on different cycle, cur cycle: " << cycle;
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    bus_ = bus;
    srcFU_ = srcFU;
    dstFU_ = dstFU;
    immWriteCycle_ = immWriteCycle;
    immu_ = immu;
    immRegIndex_ = immRegIndex;

    currentBroker_ = 0;
    node.setCycle(cycle);

    cycle_ = cycle;
    node_ = &node;

    // optimise broker sequence by disabling not applicable brokers
    applicableAssignments_.clear();
    for (unsigned int i = 0; i < assignments_.size(); i++) {
        if (assignments_[i]->broker().isApplicable(node, bus)) {
            assignments_[i]->setRequest(
		cycle, node, bus_, srcFU_, dstFU_, immWriteCycle_, immu_,
		immRegIndex_);
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
    if (assignment->broker().isAlreadyAssigned(cycle_, *node_, bus_)) {
        assignment->undoAssignment();
    }
    currentBroker_--;
    if (currentBroker_ < 0) {
        string msg = "Backtracked beyond first resource broker!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    } else {
        // undo possible assignments for broker we backtracked to
        assignment = applicableAssignments_[currentBroker_];
        if (assignment->broker().isAlreadyAssigned(cycle_, *node_, bus_)) {
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
    if (assignment->broker().isAlreadyAssigned(cycle_, *node_, bus_)) {
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
 * If the assignment being tested was last of the assignments in the plan
 * (which means move can be assigned completely) stores the information
 * about used resources into the last working assignment cache.
 *
 * @return True if at least one tentative assignment is possible with
 * the current broker
 */
bool
AssignmentPlan::isTestedAssignmentPossible() {
    bool possible = 
        applicableAssignments_[currentBroker_]->isAssignmentPossible();
    if (possible) {
        // is this the last broker?
        if (currentBroker_ == (int)(applicableAssignments_.size())-1) {
            // add this to the cache. 
            // first clear the cache.
            lastTestedWorkingAssignment_.clear();
            // then set the cache for this assignment.
            lastTriedNode_ = node_;
            lastTriedCycle_ = cycle_;
            for (unsigned int i = 0; i < applicableAssignments_.size()-1; 
                 i++) {
                PendingAssignment& pa = *applicableAssignments_[i];
                lastTestedWorkingAssignment_.push_back(
                    std::pair<ResourceBroker*, SchedulingResource*>(
                        &pa.broker(),
                        &pa.resource(pa.lastTriedAssignment())));
            }
            // Last one need to be handled separately because
            // tryNext has not increased lastTriedAssignment.
            if (!applicableAssignments_.empty()) {
                PendingAssignment& pa = *applicableAssignments_[
                    applicableAssignments_.size()-1];
                lastTestedWorkingAssignment_.push_back(
                    std::pair<ResourceBroker*, SchedulingResource*>(
                        &pa.broker(),
                        &pa.resource(pa.lastTriedAssignment()+1)));
            }
        }
    } else {
        debugLogRM("No applicable assignments at all:");
        debugLogRM(currentBroker().brokerName());
    }
    return possible;
}

/**
 * Tries to assign with same resources that which previous canassign
 * succeeded.
 *
 * If cannot assign with these resources, clear the information of previous
 * successfull canassign and return false.
 * If succeeds, the node is assigned.
 */
bool
AssignmentPlan::tryCachedAssignment(MoveNode& node, int cycle,
                                    const TTAMachine::Bus* bus,
                                    const TTAMachine::FunctionUnit* srcFU,
                                    const TTAMachine::FunctionUnit* dstFU,
                                    int immWriteCycle,
                                    const TTAMachine::ImmediateUnit* immu,
                                    int immRegIndex) {
    // is the cache valid for this node?
    if (lastTriedNode_ != &node || 
        lastTriedCycle_ != cycle ||
        immRegIndex_ != immRegIndex ||
        immu != immu_ ||
        immWriteCycle_ != immWriteCycle ||
        srcFU_ != srcFU ||
        dstFU_ != dstFU ||
        lastTestedWorkingAssignment_.empty()) {
        clearCache();
        return false;
    }

    // test that same brokers are applicable for both cached and current.
    // may change in case of bypassing.
    for (size_t j = 0, i = 0; j < brokers_.size(); j++) {
        ResourceBroker* broker = brokers_[j];
        if (broker->isApplicable(node, bus)) {
            if (i >= lastTestedWorkingAssignment_.size() || 
                lastTestedWorkingAssignment_[i].first != broker) {
                clearCache();
                return false;
            }
            i++;
        } else {
            // if not applicable, must not found in the cached brokers.
            if (i < lastTestedWorkingAssignment_.size() &&
                lastTestedWorkingAssignment_[i].first == broker) {
                clearCache();
                return false;
            }
        }
    }
    node.setCycle(cycle);

    for (int i = 0; i < (int)lastTestedWorkingAssignment_.size(); i++) {
        std::pair<ResourceBroker*, SchedulingResource*>& ca =
            lastTestedWorkingAssignment_[i];
        if (!ca.first->isAvailable(
                *ca.second, node, cycle, bus, srcFU, dstFU, immWriteCycle,
		immu, immRegIndex)) {
            // failed. backtrack all previous brokers.
            // unassign, clear cache and return false.
            for (i--; i >= 0; i--) {
                std::pair<ResourceBroker*, SchedulingResource*>& ca =
                    lastTestedWorkingAssignment_[i];
                ca.first->unassign(node);
            }
            node.unsetCycle();
            clearCache();
            return false;
        } else {
            // can assign. do the assign.
            ca.first->assign(cycle, node, *ca.second, immWriteCycle, immRegIndex);
        }
    }
    // everything succeeded. clear cache(state of rm changed) and return true.
    clearCache();
    return true;
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
    bus_ = NULL;
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
AssignmentPlan::resetAssignments(MoveNode& node) {
    if (!node.isPlaced()) {
        string msg = "Node is not placed in a cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    bus_ = &node.move().bus();
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->isApplicable(node, bus_)) {
            brokers_[i]->unassign(node);
        }
    }
    node.unsetCycle();
    bus_ = NULL;
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
AssignmentPlan::broker(int index) const {
    if (index < 0 || index >= static_cast<int>(brokers_.size())) {
        string msg = "Broker index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    } else {
        return *brokers_[index];
    }
}

void
AssignmentPlan::clear() {
    node_= NULL;
    cycle_ = 0;
    currentBroker_ = 0;
    resourceFound_ = false;
    for (size_t i = 0; i < assignments_.size(); i++) {
        assignments_[i]->clear();
    }
    bus_ = NULL;
    srcFU_ = NULL;
    dstFU_ = NULL;
    immWriteCycle_ = -1;
    immu_ = NULL;
    immRegIndex_ = -1;
    clearCache();
    applicableAssignments_.clear();
}

void AssignmentPlan::clearCache() {
    lastTestedWorkingAssignment_.clear();
    lastTriedCycle_ = -1;
    lastTriedNode_ = NULL;
}
