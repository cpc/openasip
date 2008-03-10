/**
 * @file BrokerDirector.cc
 *
 * Implementation of BrokerDirector class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "SimpleBrokerDirector.hh"
#include "AssignmentPlan.hh"
#include "Exception.hh"
#include "MoveNode.hh"
#include "MoveNodeSet.hh"
#include "Instruction.hh"
#include "ResourceBroker.hh"
#include "BusBroker.hh"
#include "IUBroker.hh"
#include "ITemplateBroker.hh"
#include "ExecutionPipelineBroker.hh"
#include "IUResource.hh"
#include "Immediate.hh"
#include "MapTools.hh"

#include "POMDisassembler.hh"

using std::string;
using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Target machine.
 * @param plan Resource assignment plan.
 */
SimpleBrokerDirector::SimpleBrokerDirector(
    const TTAMachine::Machine& machine,
    AssignmentPlan& plan):
    BrokerDirector(machine, plan) {
    knownMaxCycle_ = -1;
}

/**
 * Destructor.
 */
SimpleBrokerDirector::~SimpleBrokerDirector(){
    MapTools::deleteAllValues(origResMap_);
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
SimpleBrokerDirector::canAssign(int cycle, MoveNode& node) const {

    if (node.isScheduled()) {
        return false;
    }
    // Store original resources of node
    OriginalResources oldRes(
        node.move().source().copy(),
        node.move().destination().copy(),
        &node.move().bus(),
        NULL,
        false);
    if (node.move().isUnconditional()) {
        oldRes.isGuarded_ = false;
    } else {
        oldRes.isGuarded_ = true;
        oldRes.guard_ = node.move().guard().copy();
    }
    int placedInCycle = -1;
    if (node.isPlaced()) {
        placedInCycle = node.cycle();
    }
    plan_->setRequest(cycle, node);
    
    bool success = false;
    while (!success) {
        if (!plan_->isTestedAssignmentPossible()) {
            if (&plan_->currentBroker() == &plan_->firstBroker()) {
                plan_->resetAssignments();
                success = false; // no assignment found
                break;
            } else {
                plan_->backtrack();
            }
        } else if (&plan_->currentBroker() == &plan_->lastBroker()) {
            plan_->resetAssignments();
            success = true;

        } else {
            plan_->tryNextAssignment();
            plan_->advance();
        }
    }
    // restore original resources of node
    node.move().setSource(oldRes.src_);
    oldRes.src_ = NULL;
    node.move().setDestination(oldRes.dst_);
    oldRes.dst_ = NULL;
    node.move().setBus(*oldRes.bus_);
    if (oldRes.isGuarded_) {
        node.move().setGuard(oldRes.guard_);
        oldRes.guard_ = NULL;
    }
    if (placedInCycle != -1) {
        node.setCycle(placedInCycle);
    }
    if (success) {
        return true;
    } else {
        return false;
    }
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
SimpleBrokerDirector::assign(int cycle, MoveNode& node)
    throw (Exception) {
    if (node.isPlaced() && node.cycle() != cycle) {
        string msg =
            "Node is already placed in a cycle different from given cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    OriginalResources* oldRes = new OriginalResources(
        node.move().source().copy(),
        node.move().destination().copy(),
        &node.move().bus(),
        NULL,
        false);
    if (node.move().isUnconditional()) {
        oldRes->isGuarded_ = false;
    } else {
        oldRes->isGuarded_ = true;
        oldRes->guard_ = node.move().guard().copy();
    }
    origResMap_.insert(
        std::pair<const MoveNode*, OriginalResources*>(&node, oldRes));

    plan_->setRequest(cycle, node);

    bool success = false;
    while (!success) {
        if (!plan_->isTestedAssignmentPossible()) {
            if (&plan_->currentBroker() == &plan_->firstBroker()) {
                string msg = "No resource assignment found for ";
                msg += node.toString() + "!";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);

            } else {
                plan_->backtrack();
            }

        } else {
            plan_->tryNextAssignment();
            if (&plan_->currentBroker() == &plan_->lastBroker()) {
                success = true;
            } else {
                plan_->advance();
            }
        }
    }
    if (knownMaxCycle_ < cycle) {
        knownMaxCycle_ = cycle;
    }

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
SimpleBrokerDirector::unassign(MoveNode& node)
    throw (Exception) {

    if (!node.isPlaced()) {
        string msg = "Node " + node.toString();
        msg += " is not placed in any cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    int nodeCycle = node.cycle();

    plan_->resetAssignments(node);

    if (!MapTools::containsKey(origResMap_, &node)) {
        abortWithError("Original resources lost!");
    }
    OriginalResources* oldRes = MapTools::valueForKey<OriginalResources*>(
        origResMap_, &node);
    
    node.move().setSource(oldRes->src_);
    oldRes->src_ = NULL;
    node.move().setDestination(oldRes->dst_);
    oldRes->dst_ = NULL;
    node.move().setBus(*oldRes->bus_);
    if (oldRes->isGuarded_) {
        node.move().setGuard(oldRes->guard_);
        oldRes->guard_ = NULL;
    }
    delete oldRes;
    origResMap_.erase(&node);

    // if unassigned move was known to be in highest cycle assigned so far
    // tries to decrease highest cycle checking for assigned moves and
    // immediates
    if (nodeCycle == knownMaxCycle_) {
        while(nodeCycle >= 0) {
            Instruction* tempIns = instruction(nodeCycle);
            if (tempIns->moveCount() == 0 && tempIns->immediateCount() == 0) {
                knownMaxCycle_--;
                nodeCycle--;
            } else {
                break;
            }
        }
    }
}

/**
 * Return the earliest cycle in the scope where all required resources
 * can be assigned to the given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param move The move.
 * @return The earliest cycle in the scope where all required resources
 * can be assigned to the given node. Returns -1 if assignment
 * is not possible starting from given cycle.
 */
int
SimpleBrokerDirector::earliestCycle(MoveNode& move) const
    throw (Exception) {
    return earliestCycle(0, move);
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
 * @param node MoveNode.
 * @return The earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node. Returns -1 if assignment
 * is not possible starting from given cycle.
 */
int
SimpleBrokerDirector::earliestCycle(int cycle, MoveNode& node) const
    throw (Exception) {

    int minCycle = executionPipelineBroker().earliestCycle(cycle, node);

    if (minCycle == -1) {
        // No assignment possible
        return -1;
    }

    minCycle = std::max(minCycle, cycle);
    int lastCycleToTest = -1;
    lastCycleToTest = largestCycle();

    while (!canAssign(minCycle, node)) {
        if (minCycle > lastCycleToTest + 1) { 
            // Even on empty instruction it is not possible to assign
            return -1;
        }
        minCycle++;
    }
    return minCycle;
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
 * can be assigned to the given node. In case assignment is
 * not possible for given arguments returns -1. Returns INT_MAX if
 * there is no upper boundary for assignment.
 */
int
SimpleBrokerDirector::latestCycle(MoveNode& node) const {
    return latestCycle(INT_MAX, node);
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
 * required resources can be assigned to given node. In case assignment is
 * not possible for given arguments returns -1. Returns INT_MAX if
 * there is no upper boundary for assignment.
 */
int
SimpleBrokerDirector::latestCycle(int cycle, MoveNode& node) const {

    int maxCycle = executionPipelineBroker().latestCycle(cycle, node);
    if (maxCycle == -1) {
        // Assignment not possible at all
        return maxCycle;
    }
    if (maxCycle == INT_MAX || maxCycle == INT_MAX - 1) {
        if (cycle == INT_MAX) {
            // There is no upper limit for assignment and it was not set
            // either
            return cycle;
        }
    }
    if (maxCycle > cycle) {
        maxCycle = cycle;
    }
    int lastCycleToTest = -1;
    lastCycleToTest =
        std::max(knownMaxCycle_,
                executionPipelineBroker().highestKnownCycle());
    if (maxCycle <= lastCycleToTest) {
        for (int i = maxCycle; i >= 0; i--) {
            if (canAssign(i, node)) {
                return i;
            }
        }
    } else {
        if (maxCycle > lastCycleToTest) {
            if (canAssign(maxCycle, node)) {
                return maxCycle;
            }
        }
        for (int i = lastCycleToTest; i >= 0; i--) {
            if (canAssign(i, node)) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Return true if immediate in given node can be transported by some bus
 * in the machine.
 *
 * @param node Node that contains immediate read.
 * @return True if immediate in given node can be transported by some bus
 * in the machine.
 * @exception ModuleRunTimeError If required resource brokers are not found
 * in assignment plan.
 */
bool
SimpleBrokerDirector::canTransportImmediate(const MoveNode& node) const {
    try {
        BusBroker& broker = busBroker();
        return broker.canTransportImmediate(node);
    } catch (const InstanceNotFound& e) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }
}

 /**
 * Return the immediate unit broker.
 *
 * @return The immediate unit broker.
 * @exception InstanceNotFound If immediate unit broker is not found.
 */
IUBroker&
SimpleBrokerDirector::immediateUnitBroker() const throw (InstanceNotFound) {
    ResourceBroker* broker = NULL;
    for (int i = 0; i < plan_->brokerCount(); i++) {
        broker = &plan_->broker(i);
        if (broker->isIUBroker()) {
            return dynamic_cast<IUBroker&>(*broker);
        }
    }
    string msg = "Immediate unit broker not found.";
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return the instruction template broker.
 *
 * @return The instruction template broker.
 * @exception InstanceNotFound If instruction template broker is not found.
 */
ITemplateBroker&
SimpleBrokerDirector::instructionTemplateBroker() const
    throw (InstanceNotFound) {

    ResourceBroker* broker = NULL;
    for (int i = 0; i < plan_->brokerCount(); i++) {
        broker = &plan_->broker(i);
        if (broker->isITemplateBroker()) {
            return dynamic_cast<ITemplateBroker&>(*broker);
        }
    }
    string msg = "Instruction template broker not found.";
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return the bus broker.
 *
 * @return The bus broker.
 * @exception InstanceNotFound If bus broker is not found.
 */
BusBroker&
SimpleBrokerDirector::busBroker() const throw (InstanceNotFound) {
    ResourceBroker* broker = NULL;
    for (int i = 0; i < plan_->brokerCount(); i++) {
        broker = &plan_->broker(i);
        if (broker->isBusBroker()) {
            return dynamic_cast<BusBroker&>(*broker);
        }
    }
    string msg = "Bus broker not found.";
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return the execution pipeline broker.
 *
 * @return The execution pipeline broker.
 * @exception InstanceNotFound If execution pipeline broker is not found.
 */
ExecutionPipelineBroker&
SimpleBrokerDirector::executionPipelineBroker() const
    throw (InstanceNotFound) {
    ResourceBroker* broker = NULL;
    for (int i = 0; i < plan_->brokerCount(); i++) {
        broker = &plan_->broker(i);
        if (broker->isExecutionPipelineBroker()) {
            return dynamic_cast<ExecutionPipelineBroker&>(*broker);
        }
    }
    string msg = "Execution Pipeline broker not found.";
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Constructor for embedded structure for storing the original resources
 */
SimpleBrokerDirector::OriginalResources::OriginalResources(
    TTAProgram::Terminal* src,
    TTAProgram::Terminal* dst,
    TTAMachine::Bus*    bus,
    TTAProgram::MoveGuard* guard,
    bool isGuarded) {

    src_ = src;
    dst_ = dst;
    bus_ = bus;
    guard_ = guard;
    isGuarded_ = isGuarded;
}

/**
 * Destructor for embedded structure storing original resources
 */
SimpleBrokerDirector::OriginalResources::~OriginalResources() {
    if (src_!= NULL) {
        delete src_;
    }
    if (dst_!= NULL) {
        delete dst_;
    }
    if (guard_!= NULL) {
        delete guard_;
    }
}

/**
 * Test if the MoveSet can be schedule with machine connectivity.
 *
 * @param nodes The MoveNodeSet to test.
 * @return true if MoveNodeSet can be scheduled on this machine.
 * @todo Only tries to assign nodes in same order as they are in MoveNodeSet,
 * does not try to backtrack in case it is not possible.
 */
bool
SimpleBrokerDirector::hasConnection(MoveNodeSet& nodes) {

    int lastCycleToTest = -1;
    lastCycleToTest = largestCycle();
    int testedCycle = lastCycleToTest + 1;
    int notAssigned = nodes.count();
    int i = 0;
    bool success = true;
    while (notAssigned > 0) {
        if (canAssign(testedCycle, nodes.at(i))) {
            assign(testedCycle, nodes.at(i));
            notAssigned--;
            i++;
        } else {
            testedCycle++;
        }
        if (testedCycle > lastCycleToTest + 20){
            success = false;
            break;
        }
    }

    for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i).isScheduled()) {
            unassign(nodes.at(i));
        }
    }
    return success;
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
SimpleBrokerDirector::hasGuard(const MoveNode& node) const {
    Move& move = const_cast<MoveNode&>(node).move();
    if (move.isUnconditional()) {
        return true;
    }
    return busBroker().hasGuard(node);
}
/**
 * Returns a copy of instruction that holds a moves for given cycle
 * and has correct template set.
 *
 * @param cycle Cycle for which to get instruction
 * @return the Instruction with moves and template assigned
 * @note This method should be called when whole scope is scheduled
 */
TTAProgram::Instruction*
SimpleBrokerDirector::instruction(int cycle) const {
    return instructionTemplateBroker().instruction(cycle);
}

/**
 * Defines if Broker Director implementation supports node with resources
 * pre assigned by clients.
 *
 * @return For SimpleBrokerDirector allways false.
 */
bool
SimpleBrokerDirector::supportsExternalAssignments() const {
    return false;
}

/**
 * Returns largest cycle known to be used by any of the resources.
 *
 * @return Largest cycle resource manager assigned any resource to.
 */
int
SimpleBrokerDirector::largestCycle() const{
    return std::max(knownMaxCycle_,
        executionPipelineBroker().highestKnownCycle());
}

/**
 * Transfer the instruction ownership away from this object.
 *
 * If this method is called, resource manager does not delete it's
 * instructions when it it destroyed.
 */
void 
SimpleBrokerDirector::loseInstructionOwnership() {
    instructionTemplateBroker().loseInstructionOwnership();
}

/**
 * Finds the original terminal with value of immediate.
 * 
 * @param node node with immediate register source
 * @return terminal with immediate value
 */
TTAProgram::Terminal*
SimpleBrokerDirector::immediateValue(const MoveNode& node) {
    return immediateUnitBroker().immediateValue(node);
}

/**
 * Finds cycle in which the immediate that is read by node is written.
 * 
 * @param node with source immediate register
 * @return cycle in which immediate is written to register
 */
int
SimpleBrokerDirector::immediateWriteCycle(const MoveNode& node) const {
    return immediateUnitBroker().immediateWriteCycle(node);
}

bool
SimpleBrokerDirector::isTemplateAvailable(
    int defCycle, 
    TTAProgram::Immediate* immediate) const {
    
    return instructionTemplateBroker().isTemplateAvailable(
        defCycle, immediate);
}
