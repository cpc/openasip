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
 * @file ExecutionPipelineBroker.cc
 *
 * Implementation of ExecutionPipelineBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <climits>

#include "ExecutionPipelineBroker.hh"
#include "ExecutionPipelineResource.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "HWOperation.hh"
#include "ResourceMapper.hh"
#include "MapTools.hh"
#include "StringTools.hh"
#include "PSocketResource.hh"
#include "TCEString.hh"
#include "ResourceManager.hh"
#include "Terminal.hh"
#include "ProgramOperation.hh"
#include "Operation.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"

using namespace TTAMachine;
using std::pair;

/**
 * Constructor.
 */
ExecutionPipelineBroker::ExecutionPipelineBroker(std::string name,
        unsigned int initiationInterval): 
    ResourceBroker(name, initiationInterval), longestLatency_(0), ddg_(NULL) {
        
    // change ii for broker's resources also
    for (FUPipelineMap::iterator i = fuPipelineMap_.begin();
            i != fuPipelineMap_.end(); ++i) {
        i->first->setInitiationInterval(initiationInterval);
    }
}

void
ExecutionPipelineBroker::setMaxCycle(unsigned int maxCycle) {
    // change ii for broker's resources also
    for (FUPipelineMap::iterator i = fuPipelineMap_.begin();
            i != fuPipelineMap_.end(); ++i) {
        i->first->setMaxCycle(maxCycle);
    }
}

/**
 * Destructor.
 */
ExecutionPipelineBroker::~ExecutionPipelineBroker(){
}

/**
 * Mark given resource as in use for the given node, and assign the
 * corresponding machine part (if applicable) to the node's move.
 *
 * If the node is already assigned to given resource, this method does
 * nothing.
 *
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 * @note The execution pipeline broker is used only to construct resources.
 */
void
ExecutionPipelineBroker::assign(int, MoveNode&, SchedulingResource&, int, int)
{
    abortWithError("Not implemented.");
}

/**
 * Free the resource type managed by this broker and unassign it from
 * given node.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does
 * nothing.
 *
 * @param node Node.
 */
void
ExecutionPipelineBroker::unassign(MoveNode&) {
    abortWithError("Not implemented.");
}

/**
 * Return the earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * Considers source and destination terminals independently
 * and compares the results. Returns -1 if no assignment is possible.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 */
int
ExecutionPipelineBroker::earliestCycle(int cycle, const MoveNode& node,
                                       const TTAMachine::Bus*,
                                       const TTAMachine::FunctionUnit* srcFU,
                                       const TTAMachine::FunctionUnit* dstFU,
                                       int,
                                       const TTAMachine::ImmediateUnit*,
                                       int)
    const {
    
    int efs = earliestFromSource(cycle, node, srcFU);
    if (efs == -1 || efs == INT_MAX) {
        debugLogRM("returning -1");
        return -1;
    }
    int efd = earliestFromDestination(efs, node, dstFU);
    if (efd == -1 || efd == INT_MAX) {
        debugLogRM("returning -1");
        return -1;
    }
    
    // Loop as long as we find cycle which work for both source and 
    // destination.
    while (efd != efs) {
        efs = earliestFromSource(efd, node, srcFU);
        if (efs == -1 || efs == INT_MAX) {
            debugLogRM("returning -1");
            return -1;
        }
        if (efs == efd) {
            return efd;
        }
        efd = earliestFromDestination(efs, node, dstFU);
        if (efd == -1 || efd == INT_MAX) {
            debugLogRM("returning -1");
            return -1;
        }
    }
    return efd;
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The latest cycle, starting from given cycle backwards, where a
 * resource of the type managed by this broker can be assigned to the
 * given node. Considers source and destination terminals independently
 * and compares the results. Returns -1 if no assignment is possible.
 */
int
ExecutionPipelineBroker::latestCycle(int cycle, const MoveNode& node,
                                     const TTAMachine::Bus*,
                                     const TTAMachine::FunctionUnit* srcFU,
                                     const TTAMachine::FunctionUnit* dstFU,
                                     int,
                                     const TTAMachine::ImmediateUnit*,
                                     int) const {
    int src = latestFromSource(cycle,node, srcFU);
    if (src == -1) {
        return -1;
    }
    int dst = latestFromDestination(src, node, dstFU);
    return dst;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 */
bool
ExecutionPipelineBroker::isAlreadyAssigned(
    int, const MoveNode&, const TTAMachine::Bus*) const {
    abortWithError("Not implemented.");
    return false;
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * @param node Node.
 * @return Always false. Broker is not used standalone, only for creating
 * resources and setting up resource links.
 * @note Broker is used only to construct resources, the assignment
 * and unassignment is done via ExecutionPipelineResource attached to
 * InputFuResource.
 */
bool
ExecutionPipelineBroker::isApplicable(
    const MoveNode&, const TTAMachine::Bus*) const {
    return false;
}

/**
 * Build all resource objects of the controlled type required to model
 * scheduling resources of the given target processor.
 *
 * This method cannot set up the resource links (dependent and related
 * resources) of the constructed resource objects.
 *
 * @param target Target machine.
 */
void
ExecutionPipelineBroker::buildResources(const TTAMachine::Machine& target) {

    Machine::FunctionUnitNavigator navi = target.functionUnitNavigator();
    for (int i = 0; i < navi.count(); i++) {
        FunctionUnit* fu = navi.item(i);
        ExecutionPipelineResource* epResource =
            new ExecutionPipelineResource(
                *fu, initiationInterval_);
        
        ResourceBroker::addResource(*fu, epResource);
        fuPipelineMap_.insert(
            pair<SchedulingResource*, const FunctionUnit*>(epResource, fu));
        longestLatency_ = 
            (longestLatency_ < fu->maxLatency()) 
                ? fu->maxLatency() : longestLatency_;            
    }

    ControlUnit* gcu = target.controlUnit();

    ExecutionPipelineResource* epResource =
	new ExecutionPipelineResource(*gcu, initiationInterval_);

    ResourceBroker::addResource(*gcu, epResource);
    fuPipelineMap_.insert(pair<SchedulingResource*, const
            FunctionUnit*>(epResource, gcu));
}

/**
 * Complete resource initialisation by creating the references to
 * other resources due to a dependency or a relation. Use the given
 * resource mapper to lookup dependent and related resources using
 * machine parts as keys.
 *
 * @param mapper Resource mapper.
 */
void
ExecutionPipelineBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const FunctionUnit* fuOriginal =
            dynamic_cast<const FunctionUnit*>((*resIter).first);
        if (fuOriginal == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "ExecutionPipelineBroker has something "
                "else then FU registered!");
        }

        // resMap_ is local for broker, so it can only contain fu->epResource
        SchedulingResource* epResource = (*resIter).second;

        const FunctionUnit* fu = NULL;

        try {
            fu = MapTools::valueForKey<const FunctionUnit*>(
                    fuPipelineMap_, epResource);
        } catch (const Exception& e) {
            std::string msg = "Pipeline resource \'";
            msg += epResource->name();
            msg += "\' not found";
            abortWithError(msg);
        }

        for (int i = 0; i < fu->portCount(); i++) {
            BaseFUPort* port = fu->port(i);
            if (port->isTriggering()) {
                if (port->outputSocket() != NULL) {
                    try {
                        SchedulingResource& relatedRes =
                            mapper.resourceOf(*port->outputSocket());
                        epResource->addToRelatedGroup(0, relatedRes);
                    } catch (const KeyNotFound& e) {
                        std::string msg = "ExecutionPipelineBroker: finding ";
                        msg += " resource for Socket ";
                        msg += " failed with error: ";
                        msg += e.errorMessageStack();
                        msg += " for resource " + epResource->name();
                        throw KeyNotFound(
                            __FILE__, __LINE__, __func__, msg);
                    }                                
                    
                } else {
                    try {
                        SchedulingResource& relatedRes =
                            mapper.resourceOf(*port->inputSocket());
                        epResource->addToRelatedGroup(0, relatedRes);
                    } catch (const KeyNotFound& e) {
                        std::string msg = "ExecutionPipelineBroker: finding ";
                        msg += " resource for Socket ";
                        msg += " failed with error: ";
                        msg += e.errorMessageStack();
                        msg += " for resource " + epResource->name();
                        throw KeyNotFound(
                            __FILE__, __LINE__, __func__, msg);
                    }                                                    
                }
            }
        }
    }
}

/**
 * Return the highest cycle any of the pipeline is known to be used
 *
 * @return A highest cycle in which any of the pipelines is used.
 */
int
ExecutionPipelineBroker::highestKnownCycle() const {
    int localMax = 0;
    for (ResourceMap::const_iterator resIter = resMap_.begin();
        resIter != resMap_.end(); resIter++) {
        ExecutionPipelineResource* ep =
            (dynamic_cast<ExecutionPipelineResource*>((*resIter).second));
        localMax = std::max(ep->highestKnownCycle(), localMax);
    }
    return localMax;
}

bool
ExecutionPipelineBroker::isExecutionPipelineBroker() const {
    return true;
}

/**
 * Set initiation interval, if ii = 0 then initiation interval is not used.
 *
 * @param ii initiation interval
 */
void
ExecutionPipelineBroker::setInitiationInterval(unsigned int ii)
{
    initiationInterval_ = ii;

    // change ii for broker's resources also
    for (FUPipelineMap::iterator i = fuPipelineMap_.begin();
            i != fuPipelineMap_.end(); ++i) {
        i->first->setInitiationInterval(ii);
    }
}

bool ExecutionPipelineBroker::isLoopBypass(const MoveNode& node) const {

    if (ddg_ == NULL || !ddg_->hasNode(node)) {
        return false;
    }

    auto inEdges = ddg_->operationInEdges(node);
    for (auto e : inEdges) {
        if (e->isBackEdge()) {
            return true;
        }
    }
    return false;
}


/**
 * Returns latest cycle, starting from given parameter and going towards
 * zero, in which the node can be scheduled. Taking into account source
 * terminal of Move (result read).
 *
 * @param cycle Starting cycle for tests
 * @param node MoveNode to find latest cycle for
 * @return The latest cycle in which the MoveNode can be scheduled, -1 if
 * scheduling is not possible, 'cycle' if source is not FUPort
 */
int
ExecutionPipelineBroker::latestFromSource(
    int cycle, const MoveNode& node, const TTAMachine::FunctionUnit* srcFU)
    const{

    if (!node.isSourceOperation()) {
        return cycle;
    }
    // kludge for looop bypass.
    ProgramOperation& sourceOp = node.sourceOperation();
    if (isLoopBypass(node)) {
        cycle+=initiationInterval_;
    }
    const MoveNode* triggerNode = NULL;
    const MoveNode* lastOperandNode = NULL;
    const MoveNode* lastResultNode = NULL;    
    int minCycle = -1;
    // Source is result read, we test all operands
    for (int i = 0; i < sourceOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &sourceOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            minCycle = std::max(tempNode->cycle(), minCycle);
            lastOperandNode = tempNode;
            if (tempNode->move().isTriggering()) {
                triggerNode = tempNode;
            }
        }
    }
    // Source is result read, we test all results    
    for (int i = 0; i < sourceOp.outputMoveCount(); i++) {
        const MoveNode* tempNode = &sourceOp.outputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            int tempCycle = tempNode->cycle();
            if (isLoopBypass(*tempNode)) {
                tempCycle += initiationInterval_;
            }
            minCycle = std::max(tempCycle, minCycle);
            lastResultNode = tempNode;
        }
    }
    
    if (minCycle == -1) {
        return cycle;
    }
    if (minCycle > cycle && lastResultNode == NULL) {
        // Some operand is already later then where we started to backtrack.
        debugLogRM("returning -1");
        return -1;
    }
    if (triggerNode != NULL) {
        
        if (minCycle >= node.earliestResultReadCycle() 
            && lastResultNode == NULL) {
            // If we do have minCycle from operands, this is valid test,
            // otherwise it is not.
            throw InvalidData(__FILE__, __LINE__, __func__, 
                "Some operand move is written after the result move "
                "is ready! " + node.toString());
        }
        if (node.earliestResultReadCycle() > cycle) {
            // Result is available later then where we started to backtrack
            debugLogRM("returning -1");
            return -1;
        } else {
            minCycle = node.earliestResultReadCycle();
        }
    }
    // minCycle has latest of operand writes or earliest result read cycle
    // find next read of same FU with different PO
    assert(lastOperandNode != NULL || lastResultNode != NULL);
    const TTAMachine::FunctionUnit* fu;
    if (lastOperandNode != NULL) {
        fu = &lastOperandNode->move().destination().functionUnit();
    } else {
        assert(lastResultNode->isSourceOperation());
        fu = &lastResultNode->move().source().functionUnit();
    }

    if (srcFU != NULL && fu != srcFU) {
        return -1;
    }
    HWOperation& hwop = *fu->operation(sourceOp.operation().name());
    const TTAMachine::Port& port =
        *hwop.port(node.move().source().operationIndex());
    SchedulingResource& res = *resourceOf(*fu);
    ExecutionPipelineResource* ep =
        static_cast<ExecutionPipelineResource*>(&res);
    int triggerCycle = triggerNode != NULL && triggerNode->isScheduled() ? 
        triggerNode->cycle() : 
        INT_MAX;
    // last available cycle is one lower then next write

    int nextResult = ep->nextResultCycle(
        port, minCycle, node, triggerNode, triggerCycle);
    if (cycle < nextResult || nextResult < minCycle) {
        return cycle;
    }
    return nextResult-1;
}
/**
 * Returns latest cycle, starting from given parameter and going towards
 * zero, in which the node can be scheduled. Taking into account destination
 * terminal of Move (operand write).
 *
 * @param cycle Starting cycle for tests
 * @param node MoveNode to find latest cycle for
 * @return The latest cycle in which the MoveNode can be scheduled, -1 if
 * scheduling is not possible, 'cycle' if destination is not FUPort
 */
int
ExecutionPipelineBroker::latestFromDestination(
    int cycle,
    const MoveNode& node, const TTAMachine::FunctionUnit* dstUnit) const {

    // TODO: this not do full reuslt overwrite another tests.
    // It is however handled in canassign(). 
    // Doing it here would make scheduling faster.
    int latest = cycle;
    for (unsigned int j = 0; j < node.destinationOperationCount(); j++) {
        ProgramOperation& destOp = node.destinationOperation(j);
        for (int i = 0; i < destOp.inputMoveCount(); i++) {
            const MoveNode* tempNode = &destOp.inputMove(i);
            if (tempNode->isScheduled() && tempNode != &node) {
                if (tempNode->move().isTriggering()) {
                    latest = std::min(tempNode->cycle(), latest);
                }
            }
        }

        for (int i = 0; i < destOp.outputMoveCount(); i++) {
            const MoveNode* tempNode = &destOp.outputMove(i);
            if (tempNode->isScheduled() && tempNode != &node) {
                const TTAMachine::FunctionUnit* fu =
                    &node.move().destination().functionUnit();
                if (dstUnit != NULL && dstUnit != fu) {
                    return -1;
                }
                const TTAMachine::HWOperation& hwop =
                    *fu->operation(
                        destOp.operation().name());
                const int outputIndex =
                    tempNode->move().source().operationIndex();

                int tempCycle = tempNode->cycle();
                if (isLoopBypass(*tempNode)) {
                    tempCycle += initiationInterval_;
                }

                latest = std::min(latest,
                                  tempCycle - hwop.latency(outputIndex));
            }
        }
    }

    return latest;
}
/**
 * Returns earliest cycle, starting from given parameter and going towards
 * INT_MAX, in which the node can be scheduled. Taking into account
 * source Terminal of Move (result read) and other operands already
 * scheduled.
 *
 * @param cycle Starting cycle for tests
 * @param node MoveNode to find earliest cycle for
 * @return The earliest cycle in which the MoveNode can be scheduled, 0 if
 * source is not FUPort
 */
int
ExecutionPipelineBroker::earliestFromSource(
    int cycle, const MoveNode& node, const TTAMachine::FunctionUnit* srcFU)
    const {
    
    if (!node.isSourceOperation()) {
        return cycle;
    }
    ProgramOperation& sourceOp = node.sourceOperation();
    const MoveNode* triggerNode = NULL;
    int minCycle = -1;

    const FunctionUnit* fu = NULL;
    assert(node.move().source().isFUPort());

    const HWOperation* hwop = NULL;
    const int outputIndex =
        node.move().source().operationIndex();
    int latency = 1;
    
    for (int i = 0; i < sourceOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &sourceOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            fu = &tempNode->move().destination().functionUnit();
            if (srcFU != NULL&& fu != srcFU) {
                return -1;
            }
            minCycle = std::max(tempNode->cycle()+1, minCycle);
            if (tempNode->move().isTriggering()) {
                triggerNode = tempNode;
                hwop = fu->operation(sourceOp.operation().name());
                latency = hwop->latency(outputIndex);
                minCycle = std::max(triggerNode->cycle() + latency, cycle);
            }
        }
    }
    if (minCycle == -1) {
        // no operands scheduled for result of given PO
        // so earliest cycle is 0 + latency or tested cycle
        if (initiationInterval_ != 0 && latency >= (int)initiationInterval_) {
            debugLogRM("returning -1");
            return -1;
        }
        minCycle = std::max(cycle,1); 
    }

    if (triggerNode != NULL && triggerNode->isScheduled()) {
        int triggerCycle = triggerNode->cycle();

        const TTAMachine::Port& port = *hwop->port(outputIndex);
        SchedulingResource& res = *resourceOf(*fu);
        ExecutionPipelineResource* ep =
            static_cast<ExecutionPipelineResource*>(&res);

        if (!ep->resultNotOverWritten(
                minCycle, node.earliestResultReadCycle(), node,
                port, triggerNode, triggerCycle)) {
            return -1;
        }
    }

    return std::max(minCycle, cycle);
}

// TODO: if some result scheudled?
bool ExecutionPipelineBroker::isMoveTrigger(const MoveNode& node) const{
    if (!node.isDestinationOperation()) {
        return false;
    }

    for (unsigned int j = 0; j < node.destinationOperationCount(); j++) {
//        const HWOperation* hwop = NULL;

        // Test other inputs to the operation.
        ProgramOperation& destOp = node.destinationOperation(j);
        
        for (int i = 0; i < destOp.inputMoveCount(); i++) {
            const MoveNode* tempNode = &destOp.inputMove(i);
            if (tempNode->isScheduled()) {
                if (tempNode->move().isTriggering()) {
                    if (tempNode != &node) {
                        return false;
                        break;
                    } else {
                        return true;
                    }
                }
                auto fu = &tempNode->move().destination().functionUnit();
                auto triggeringPort = fu->triggerPort();
                auto hwop = fu->operation(
                    node.destinationOperation().operation().name());
                auto port = hwop->port(
                    node.move().destination().operationIndex());
                if (port == triggeringPort) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
}

/**
 * Return earliest cycle, starting from given parameter and going towards
 * INT_MAX, in which the node can be scheduled. Taking into account
 * destination terminal of Move (operand write).
 *
 * This method may return too early for some cases but there is canassign test
 * after this always at higher level in RM so this does not matter;
 * having this method is just performance optimization to avoid calling
 * slow canAssign() for too small values.
 *
 * @param cycle Starting cycle for tests
 * @param node MoveNode to find earliest cycle for
 * @return The earliest cycle in which the MoveNode can be scheduled, -1 if
 * scheduling is not possible, 0 if destination is not FUPort
 */
int
ExecutionPipelineBroker::earliestFromDestination(
    int cycle,
    const MoveNode& node, const TTAMachine::FunctionUnit* dstFU) const {

    if (!node.isDestinationOperation()) {
        return cycle;
    }
    const FunctionUnit* fu = NULL;

    int minCycle = cycle;

    bool triggers = isMoveTrigger(node);

    for (unsigned int j = 0; j < node.destinationOperationCount(); j++) {
        
        const HWOperation* hwop = NULL;
        // Test other inputs to the operation.
        ProgramOperation& destOp = node.destinationOperation(j);
        
        for (int i = 0; i < destOp.inputMoveCount(); i++) {
            const MoveNode* tempNode = &destOp.inputMove(i);
            if (tempNode->isScheduled() && tempNode != &node) {
                fu = &tempNode->move().destination().functionUnit();
                hwop = fu->operation(destOp.operation().name());
                if (dstFU != NULL && fu != dstFU) {
                    return -1;
                }

                if (triggers) {
                    // TODO: operand slack
                    minCycle = std::max(tempNode->cycle(), minCycle);
                }
                if (tempNode->move().isTriggering()) {
                    // TODO: operand slack
                    int triggerCycle = tempNode->cycle();
                    if (triggerCycle < cycle) {
                        // trying to schedule operand after trigger
                        debugLogRM("returning -1");
                        return -1;
                    }
                }
            }
        }

        // Then check the already scheduled results, do they limit the cycle
        // where this can be scheduled.
        int minResultCycle = INT_MAX;
        for (int i = 0; i < destOp.outputMoveCount(); i++) {
            const MoveNode* tempNode = &destOp.outputMove(i);
            if (tempNode->isScheduled()) {
                if (fu == NULL) {
                    fu = &tempNode->move().source().functionUnit();
                    hwop = fu->operation(destOp.operation().name());
                }

                if (dstFU != NULL && fu != dstFU) {
                    return -1;
                }

                const int outputIndex =
                    tempNode->move().source().operationIndex();

                // TODO: slack
                
                unsigned int latency = hwop->latency(outputIndex);
                if (initiationInterval_ != 0 && latency >= initiationInterval_) {
                    debugLogRM("returning -1");
                    return -1;
                }
                minResultCycle =
                    std::min(
                        minResultCycle,
                        tempNode->cycle() - hwop->latency(outputIndex));
            }
        }
        if (minResultCycle < minCycle) {
            // tested cycle is larger then last trigger cycle for results
            // already scheduled
            debugLogRM("returning -1");
            return -1;
        }  
    }

    return minCycle;
}

void
ExecutionPipelineBroker::setDDG(const DataDependenceGraph* ddg) {
    for (ResourceMap::iterator i = resMap_.begin(); i != resMap_.end(); i++) {
        (static_cast<ExecutionPipelineResource*>(i->second))->setDDG(ddg);
    }
    ddg_ = ddg;
}

