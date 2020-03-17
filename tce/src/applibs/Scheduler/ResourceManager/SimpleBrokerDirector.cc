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
 * @file SimpleBrokerDirector.cc
 *
 * Implementation of not so simple ;) BrokerDirector class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <climits>
#include <typeinfo>

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
#include "SchedulerCmdLineOptions.hh"
#include "AssocTools.hh"
#include "POMDisassembler.hh"
#include "ResourceManager.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "Move.hh"
#include "Bus.hh"
#include "Conversion.hh"

#ifdef DEBUG_RM
#include <typeinfo>
#endif

using std::string;
using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Target machine.
 * @param plan Resource assignment plan.
 * @param ii Initiation interval.
 */
SimpleBrokerDirector::SimpleBrokerDirector(
    const TTAMachine::Machine& machine,
    AssignmentPlan& plan,
    unsigned int ii):
    BrokerDirector(machine, plan),
    initiationInterval_(ii), schedulingWindow_(140),
    busCount_(machine.busNavigator().count()) {
    knownMaxCycle_ = -1;
    knownMinCycle_ = INT_MAX;

#if 0
   // Scheduling window is not supported in the devel branch at the moment.
    if (Application::cmdLineOptions() != NULL) {
        SchedulerCmdLineOptions* options =
            dynamic_cast<SchedulerCmdLineOptions*>(
                Application::cmdLineOptions());
        schedulingWindow_ = options->schedulingWindowSize();
    }
#endif
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
SimpleBrokerDirector::canAssign(
    int cycle, MoveNode& node,
    const TTAMachine::Bus* bus,
    const TTAMachine::FunctionUnit* srcFU,
    const TTAMachine::FunctionUnit* dstFU,
    int immWriteCycle,
    const TTAMachine::ImmediateUnit* immu,
    int immRegIndex) const {

#ifdef DEBUG_RM
    std::cerr << "testing canassign, cycle: " << cycle <<
        " node: " << node.toString() << std::endl;
#endif

    if (initiationInterval_ != 0 && cycle >= (int)(initiationInterval_ * 2)) {
        return false;
    }

    if (node.isScheduled()) {
        debugLogRM("isScheduled == true");
        return false;
    }

    // fast way to reject some cases. Makes scheduling faster
    // for bus-limited machines.
    std::map<int,int>::const_iterator i = moveCounts_.find(cycle);
    if (i != moveCounts_.end() && i->second >= busCount_) {
        debugLogRM("instruction full!");
        return false;
    }

    OriginalResources* oldRes = NULL;
    if (node.isMove()) {
        // Store original resources of node
        oldRes = new OriginalResources(
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
    }
    int placedInCycle = -1;
    if (node.isPlaced()) {
        placedInCycle = node.cycle();
    }
    plan_->setRequest(
	cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);

    bool success = false;
    while (!success) {
#ifdef DEBUG_RM
            Application::logStream()
                << "current broker: "
                << typeid(plan_->currentBroker()).name()
                << std::endl;
#endif
        if (!plan_->isTestedAssignmentPossible()) {
            if (&plan_->currentBroker() == &plan_->firstBroker()) {
                plan_->resetAssignments();
                debugLogRM("no assignment found");
                success = false; // no assignment found
                break;
            } else {
                debugLogRM("backtracking");
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
    if (node.isMove()) {
        // restore original resources of node
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
    }
    if (placedInCycle != -1) {
        node.setCycle(placedInCycle);
    }
    return success;
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
SimpleBrokerDirector::assign(int cycle, MoveNode& node,
                             const TTAMachine::Bus* bus,
                             const TTAMachine::FunctionUnit* srcFU,
                             const TTAMachine::FunctionUnit* dstFU,
                             int immWriteCycle,
                             const TTAMachine::ImmediateUnit* immu,
                             int immRegIndex) {
    if (cycle < 0) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
                          "Negative cycles not supported by RM");
    }
    if (cycle == INT_MAX) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
                          "INT_MAX cycle not supported by RM");
    }

    if (node.isPlaced() && node.cycle() != cycle) {
        string msg =
            "Node is already placed in a cycle different from given cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    if (node.isMove()) {
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
    }

    bool success = plan_->tryCachedAssignment(
        node, cycle, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);

    plan_->setRequest(cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
    assert( cycle != -1 );

    while (!success) {
        if (!plan_->isTestedAssignmentPossible()) {
            if (&plan_->currentBroker() == &plan_->firstBroker()) {
                string msg = "No resource assignment found for ";
                msg += node.toString() + "!\n" ;
                msg += "Instruction at cycle " + Conversion::toString(cycle);
                msg += " is: ";
                msg += POMDisassembler::disassemble(*instruction(cycle));
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
    int guardSlack = std::max(0, node.guardLatency() -1);
    if (knownMinCycle_ > cycle - guardSlack) {
        knownMinCycle_ = std::max(cycle - guardSlack,0);
            assert(knownMinCycle_ >= 0);
    }
    if (node.isMove()) {
        int iwc = immediateWriteCycle(node);
        if (node.isSourceImmediateRegister() &&
            knownMinCycle_ > iwc && iwc >= 0) {
            knownMinCycle_ = iwc;
            assert(knownMinCycle_ >= 0);
        }
        moveCounts_[instructionIndex(cycle)]++;
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
SimpleBrokerDirector::unassign(MoveNode& node) {
    if (!node.isPlaced()) {
        string msg = "Node " + node.toString();
        msg += " is not placed in any cycle.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    int nodeCycle = node.cycle();

    bool minCycleImmSrc = false;
    if (node.isMove()) {
        if (node.isSourceImmediateRegister() &&
            knownMinCycle_ == immediateWriteCycle(node)) {
            minCycleImmSrc = true;
        }
    }

    moveCounts_[instructionIndex(nodeCycle)]--;

    plan_->clearCache();
    plan_->resetAssignments(node);

    if (node.isMove()) {
        if (!MapTools::containsKey(origResMap_, &node)) {
            abortWithError("Original resources lost!");
        }
        OriginalResources* oldRes = MapTools::valueForKey<OriginalResources*>(
            origResMap_, &node);
    
        // restore original resources of node
        if (!node.move().source().isGPR()) {
            node.move().setSource(oldRes->src_);
        } else {
            delete oldRes->src_;
        }
        oldRes->src_ = NULL;
        if (!node.move().destination().isGPR()) {
            node.move().setDestination(oldRes->dst_);
        } else {
            delete oldRes->dst_;
        }
        oldRes->dst_ = NULL;
        node.move().setBus(*oldRes->bus_);
        if (oldRes->isGuarded_) {
            node.move().setGuard(oldRes->guard_);
            oldRes->guard_ = NULL;
        }
        delete oldRes;
        origResMap_.erase(&node);
    }
    // if unassigned move was known to be in highest cycle assigned so far
    // tries to decrease highest cycle checking for assigned moves and
    // immediates
    int cycleCounter = nodeCycle;
    if (cycleCounter == knownMaxCycle_) {
        while(cycleCounter >= knownMinCycle_) {
            // this may memory leak
            Instruction* tempIns = instruction(cycleCounter);
            if (tempIns->moveCount() == 0 && tempIns->immediateCount() == 0) {
                knownMaxCycle_--;
                cycleCounter--;
            } else {
                break;
            }
        }
        if (knownMaxCycle_ < knownMinCycle_) {
            knownMinCycle_ = INT_MAX;
            knownMaxCycle_ = -1;
        }
    }
    // if unassigned move was known to be in smallest cycle assigned so far
    // tries to increase smallest cycle checking for assigned moves and
    // immediates
    int guardSlack = std::max(0, node.guardLatency() -1);    
    cycleCounter = knownMinCycle_;
    if (cycleCounter == nodeCycle - guardSlack || minCycleImmSrc) {
        while(cycleCounter <= knownMaxCycle_) {
            // this may memory leak
            Instruction* tempIns = instruction(cycleCounter);
            if (tempIns->moveCount() == 0 && tempIns->immediateCount() == 0) {
                knownMinCycle_++;
                cycleCounter++;
            } else {
                for (int i = 0; i < tempIns->moveCount(); i++) {
                    int guardSlack = std::max(0, tempIns->move(i).guardLatency() -1);
                    if (cycleCounter - guardSlack < knownMinCycle_) {
                        knownMinCycle_ = cycleCounter - guardSlack;
                    }
                }
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
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The earliest cycle in the scope where all required resources
 * can be assigned to the given node. Returns -1 if assignment
 * is not possible starting from given cycle.
 */
int
SimpleBrokerDirector::earliestCycle(MoveNode& move,
                                    const TTAMachine::Bus* bus,
                                    const TTAMachine::FunctionUnit* srcFU,
                                    const TTAMachine::FunctionUnit* dstFU,
                                    int immWriteCycle,
                                    const TTAMachine::ImmediateUnit* immu,
                                    int immRegIndex) const {
    return earliestCycle(
	0, move, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
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
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node. Returns -1 if assignment
 * is not possible starting from given cycle.
 */
int
SimpleBrokerDirector::earliestCycle(int cycle, MoveNode& node,
                                    const TTAMachine::Bus* bus,
                                    const TTAMachine::FunctionUnit* srcFU,
                                    const TTAMachine::FunctionUnit* dstFU,
                                    int immWriteCycle,
                                    const TTAMachine::ImmediateUnit* immu,
                                    int immRegIndex) const {

    // limit the scheduling window.
    // makes code for minimal.adf schedule in reasonable time
    if (cycle < knownMaxCycle_ - schedulingWindow_) {
        cycle = knownMaxCycle_ - schedulingWindow_;
    }
    // TODO: is there need for similar test for knownMinCycle as well?

    int minCycle = executionPipelineBroker().earliestCycle(
        cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);

    if (minCycle == -1) {
        // No assignment possible
        debugLogRM("No assignment possible from executionPipelineBroker");
        return -1;
    }

    minCycle = std::max(minCycle, cycle);
    int lastCycleToTest;
    if (initiationInterval_ != 0) {
        lastCycleToTest = cycle + initiationInterval_ - 2;
    } else {
        lastCycleToTest = largestCycle();
    }

    while (!canAssign(minCycle, node, bus, srcFU, dstFU, immWriteCycle, immu,
		      immRegIndex)) {
        if (minCycle > lastCycleToTest + 1) {
            // Even on empty instruction it is not possible to assign
            debugLogRM(
                "A problem with ADF, cannot schedule even on "
                "an empty instruction.");
            return -1;
        }
        // find next cycle where exec pipeline could be free,
        // do not test every cycle with canassign.
        minCycle = executionPipelineBroker().earliestCycle(
            minCycle + 1, node, bus, srcFU, dstFU, immWriteCycle, immu,
	    immRegIndex);
        if (minCycle == -1) {
            debugLogRM("No assignment possible due to executionPipelineBroker.");
            return -1;
        }
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
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The latest cycle in the scope where all required resources
 * can be assigned to the given node. In case assignment is
 * not possible for given arguments returns -1. Returns INT_MAX if
 * there is no upper boundary for assignment.
 */
int
SimpleBrokerDirector::latestCycle(MoveNode& node,
                                  const TTAMachine::Bus* bus,
                                  const TTAMachine::FunctionUnit* srcFU,
                                  const TTAMachine::FunctionUnit* dstFU,
                                  int immWriteCycle,
                                  const TTAMachine::ImmediateUnit* immu,
                                  int immRegIndex) const {
    return latestCycle(
	INT_MAX, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
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
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The latest cycle starting from the given cycle in which
 * required resources can be assigned to given node. In case assignment is
 * not possible for given arguments returns -1. Returns INT_MAX if
 * there is no upper boundary for assignment.
 */
int
SimpleBrokerDirector::latestCycle(int cycle, MoveNode& node,
                                  const TTAMachine::Bus* bus,
                                  const TTAMachine::FunctionUnit* srcFU,
                                  const TTAMachine::FunctionUnit* dstFU,
                                  int immWriteCycle,
                                  const TTAMachine::ImmediateUnit* immu,
                                  int immRegIndex) const {

    int maxCycle = executionPipelineBroker().latestCycle(
	cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
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
    if (lastCycleToTest == 0) {
        lastCycleToTest = cycle;
    }
    // Define earliest cycle where to finish testing. If the move can not
    // be scheduled after this cycle, it can not be scheduled at all.            

    int knownMinCycle = knownMinCycle_ == INT_MAX ? cycle :
        std::min(cycle,knownMinCycle_);
    int earliestCycleLimit =
        (knownMinCycle > executionPipelineBroker().longestLatency() + 1) ?
        knownMinCycle - executionPipelineBroker().longestLatency() : 0;

    if (maxCycle <= lastCycleToTest) {
        for (int i = maxCycle; i >= earliestCycleLimit; i--) {
            if (canAssign(i, node, bus, srcFU, dstFU, immWriteCycle, immu,
			  immRegIndex)) {
                return i;
            }
        }
    } else {
        if (maxCycle > lastCycleToTest) {
            if (canAssign(maxCycle, node, bus, srcFU, dstFU, immWriteCycle,
			  immu, immRegIndex)) {
                return maxCycle;
            }
        }        
        for (int i = lastCycleToTest; i >= earliestCycleLimit; i--) {
            if (canAssign(i, node, bus, srcFU, dstFU, immWriteCycle, immu,
			  immRegIndex)) {
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
SimpleBrokerDirector::canTransportImmediate(
    const MoveNode& node, const TTAMachine::Bus* preAssignedBus) const {
    try {
        BusBroker& broker = busBroker();
        return broker.canTransportImmediate(node, preAssignedBus);
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
SimpleBrokerDirector::immediateUnitBroker() const {
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
SimpleBrokerDirector::instructionTemplateBroker() const {
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
SimpleBrokerDirector::busBroker() const {
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
SimpleBrokerDirector::executionPipelineBroker() const {
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
    const TTAMachine::Bus* bus,
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
 * Returns smallest cycle known to be used by any of the resources.
 *
 * @return Smallest cycle resource manager assigned any resource to.
 */
int
SimpleBrokerDirector::smallestCycle() const{
    // No need to check the execution pipeline resource here
    // since there are no negative pipelines
    return knownMinCycle_;
}

/**
 * Transfer the instruction ownership away from this object.
 *
 * If this method is called, resource manager does not delete it's
 * instructions when it it destroyed.
 */
void
SimpleBrokerDirector::loseInstructionOwnership(int cycle) {
    instructionTemplateBroker().loseInstructionOwnership(cycle);
}

/**
 * Finds the original terminal with value of immediate.
 *
 * @param node node with immediate register source
 * @return terminal with immediate value
 */
std::shared_ptr<TTAProgram::TerminalImmediate>
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
    std::shared_ptr<TTAProgram::Immediate> immediate) const {

    return instructionTemplateBroker().isTemplateAvailable(
        defCycle, immediate);
}

/**
 * Clears bookkeeping which is needed for unassigning previously assigned
 * moves. After this call these cannot be unassigned, but new moves which
 * are assigned after this call can still be unassigned.
 */
void
SimpleBrokerDirector::clearOldResources() {
    AssocTools::deleteAllValues(origResMap_);
    immediateUnitBroker().clearOldResources();
    instructionTemplateBroker().clearOldResources();
}

/**
 * Return the instruction index corresponding to cycle.
 *
 * If modulo scheduling is not used (ie. initiation interval is 0), then
 * index is equal to cycle.
 *
 * @param cycle Cycle to get instruction index.
 * @return Return the instruction index for cycle.
 */
unsigned int
SimpleBrokerDirector::instructionIndex(unsigned int cycle) const {

    if (initiationInterval_ != 0) {
        return cycle % initiationInterval_;
    } else {
        return cycle;
    }
}

/**
 * Clears the bookkeeping so that the same RM can be reused for different BB.
 * 
 * After this call the state of the RM should be identical to a new RM.
 */
void
SimpleBrokerDirector::clear() {
    knownMaxCycle_ = -1;
    knownMinCycle_ = INT_MAX;
    moveCounts_.clear();
    plan_->clear();
}

void
SimpleBrokerDirector::setDDG(const DataDependenceGraph* ddg) {
    executionPipelineBroker().setDDG(ddg);
}

void
SimpleBrokerDirector::setMaxCycle(unsigned int maxCycle) {
    executionPipelineBroker().setMaxCycle(maxCycle);
}
