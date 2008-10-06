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
 * @file SimpleResourceManager.cc
 *
 * Implementation of SimpleResourceManager class.
 *
 * @author Ari Mets�alme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "SimpleResourceManager.hh"
#include "InputFUBroker.hh"
#include "OutputFUBroker.hh"
#include "ExecutionPipelineBroker.hh"
#include "InputPSocketBroker.hh"
#include "OutputPSocketBroker.hh"
#include "BusBroker.hh"
#include "SegmentBroker.hh"
#include "IUBroker.hh"
#include "ITemplateBroker.hh"
#include "Instruction.hh"
#include "SequenceTools.hh"

using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Target machine.
 */
SimpleResourceManager::SimpleResourceManager(
    const TTAMachine::Machine& machine):
    ResourceManager(machine), director_(NULL) {

    // instantiate brokers

    std::vector<ResourceBroker*> brokers;

    brokers.push_back(new InputFUBroker("InputFUBroker"));
    brokers.push_back(new OutputFUBroker("OutputFUBroker"));
    brokers.push_back(new ExecutionPipelineBroker("ExecutionPipelineBroker"));
    brokers.push_back(new InputPSocketBroker("InputPSocketBroker"));
    brokers.push_back(new IUBroker("IUBroker", this));
    brokers.push_back(new OutputPSocketBroker("OutputPSocketBroker"));
    brokers.push_back(new BusBroker("BusBroker"));
    brokers.push_back(new SegmentBroker("SegmentBroker"));
    brokers.push_back(new ITemplateBroker("ITemplateBroker", this));

    // build resource model and assignment plan

    for (unsigned int i = 0; i < brokers.size(); i++) {
        buildDirector_.addBroker(*brokers[i]);
        plan_.insertBroker(*brokers[i]);    
    }
    
    buildDirector_.build(machine);

    director_ = new SimpleBrokerDirector(machine, plan_);
}

/**
 * Destructor.
 */
SimpleResourceManager::~SimpleResourceManager(){
    delete director_;
}

/**
 * Return true if given node can be assigned without resource
 * conflicts in given cycle.
 *
 * For those parts of the node that have
 * been already assigned to a resource, the manager simply keeps the
 * assignment and verifies that the resource is available. For those
 * parts that are not yet assigned, the resource manager looks for any
 * compatible resource that could be assigned.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if given node can be assigned without resource
 * conflicts in given cycle, false otherwise.
 */
bool
SimpleResourceManager::canAssign(int cycle, MoveNode& node) const {
    return director_->canAssign(cycle, node);
}

/**
 * Assign all resources needed by a given node starting from given
 * cycle, and place the node in that cycle.
 *
 * The manager keeps any pre-existing assignment and verifies that
 * assigned resource is available. For all unassigned parts of the
 * node, the resource manager looks for and then assigns the necessary
 * resources. If the node is not fully assigned after the invocation
 * of this operation, it means the assignment attempt failed.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @exception InvalidData exception if given node is already placed in a
 * cycle different from given cycle.
 */
void
SimpleResourceManager::assign(int cycle, MoveNode& node)
    throw (Exception) {
    director_->assign(cycle, node);
}

/**
 * Free all resource assignments of the given node.
 *
 * If the node is only partially assigned, the resource manager
 * ignores those parts of the node that are already unassigned.
 *
 * @param node Node to unassign.
 * @exception InvalidData If the given node is not placed in any
 * cycle. Assigned but not placed nodes are not considered by the
 * resource manager.
 */
void
SimpleResourceManager::unassign(MoveNode& node)
    throw (Exception) {
    director_->unassign(node);
}

/**
 * Return the earliest cycle in the scope where all required resources
 * can be assigned to the given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param node Node.
 * @return The earliest cycle in the scope where all required resources
 * can be assigned to the given node. -1 if assignment is not possible
 *
 */
int
SimpleResourceManager::earliestCycle(MoveNode& node) const
    throw (Exception) {
    return director_->earliestCycle(node);
}

/**
 * Return the earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param cycle Cycle to start from.
 * @param node Node.
 * @return The earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node. Returns -1 if assignment
 * is not possible.
 */
int
SimpleResourceManager::earliestCycle(int cycle, MoveNode& node) const
    throw (Exception) {
    return director_->earliestCycle(cycle, node);
}

/**
 * Return the latest cycle in the scope where all required resources
 * can be assigned to the given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param node Node.
 * @return The latest cycle in the scope where all required resources
 * can be assigned to the given node. -1 if assignment is not possible.
 * INT_MAX if there is no upper boundary for assignment.
 */
int
SimpleResourceManager::latestCycle(MoveNode& node) const {
    return director_->latestCycle(node);
}

/**
 * Return the latest cycle starting from the given cycle in which
 * required resources can be assigned to given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param cycle Cycle to start from.
 * @param node Node.
 * @return The latest cycle starting from the given cycle in which
 * required resources can be assigned to given node. -1 if assignment is not
 * possible.
 */
int
SimpleResourceManager::latestCycle(int cycle, MoveNode& node) const {
    return director_->latestCycle(cycle, node);
}

/**
 * Tests if immediate of MoveNode can be transported by any of
 * buses of present machine.
 *
 * @param node MoveNode that contains immediate that we test
 * @return true if the immediate in node can be transported by
 * some bus
 */
bool
SimpleResourceManager::canTransportImmediate(const MoveNode& node) const {
    return director_->canTransportImmediate(node);
}
/**
 * Tests if for given node set machine has possible connection from
 * sources to destinations.
 *
 * Considers all possible sources and destinations.
 * Tests all moveNodes of Program Operation at once, or single moveNode in set
 * if it is not part of any Program Operation.
 *
 * @param nodes MoveNodeSet to test
 * @return true if MoveNodeSet can be scheduled on present machine.
 */
bool
SimpleResourceManager::hasConnection(MoveNodeSet& nodes) {
    return director_->hasConnection(nodes);
}

/**
 * Tests if any of a buses of machine supports guard needed
 * by a node. Should always return true! Otherwise, scheduler generated
 * code for different machine.
 *
 * @param node MoveNode to test
 * @return true if any of buses supports guard needed by node, or if move
 * is not conditional.
 */
bool
SimpleResourceManager::hasGuard(const MoveNode& node) const {
    return director_->hasGuard(node);
}

/**
 * Returns instruction that holds a moves for given cycle
 * and has correct template set.
 *
 * The instruction ownedship stays in the resource manager until
 * loseInstructionOwnership() is called.
 *
 * @param cycle Cycle for which to get instruction
 * @return the Instruction with moves and template assigned
 * @note This method should be called when whole scope is scheduled
 */
TTAProgram::Instruction*
SimpleResourceManager::instruction(int cycle) const {
    return director_->instruction(cycle);
}

/**
 * Defines if Resource Manager implementation supports node with resources
 * pre assigned by clients.
 *
 * @return For SimpleResourceManager allways false.
 */
bool
SimpleResourceManager::supportsExternalAssignments() const {
    return director_->supportsExternalAssignments();
}

/**
 * Returns largest cycle known to be used by any of the resources.
 *
 * @return Largest cycle resource manager assigned any resource to.
 */
int
SimpleResourceManager::largestCycle() const{
    return director_->largestCycle();
}

/**
 * Transfer the instruction ownership away from this object,
 *
 * If this method is called, resource manager does not delete it's
 * instructions when it it destroyed.
 */
void 
SimpleResourceManager::loseInstructionOwnership() {
    director_->loseInstructionOwnership();
}

/**
 * Finds the original terminal with value of immediate.
 * 
 * @param node node with immediate register source
 * @return terminal with immediate value
 */
TTAProgram::Terminal*
SimpleResourceManager::immediateValue(const MoveNode& node) {
    return director_->immediateValue(node);
}

/**
 * Finds cycle in which the immediate that is read by node is written.
 * 
 * @param node with source immediate register
 * @return cycle in which immediate is written to register
 */
int
SimpleResourceManager::immediateWriteCycle(const MoveNode& node) const {
    return director_->immediateWriteCycle(node);
}

bool 
SimpleResourceManager::isTemplateAvailable(
    int defCycle, 
    TTAProgram::Immediate* immediate) const {
    
    return director_->isTemplateAvailable(defCycle, immediate);
}
