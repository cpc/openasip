/**
 * @file ExecutionPipelineBroker.cc
 *
 * Implementation of ExecutionPipelineBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

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

using namespace TTAMachine;
using std::pair;

/**
 * Constructor.
 */
ExecutionPipelineBroker::ExecutionPipelineBroker(std::string name): 
    ResourceBroker(name) {
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
ExecutionPipelineBroker::assign(int, MoveNode&, SchedulingResource&)
    throw (Exception) {

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
 * @param cycle Cycle.
 * @param node Node.
 * @return The earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node. Considers source and destination terminals independently
 * and compares the results. Returns -1 if no assignment is possible.
 */
int
ExecutionPipelineBroker::earliestCycle(int cycle, const MoveNode& node)
    const {
    
    int efs = earliestFromSource(cycle, node);
    int efd = earliestFromDestination(cycle, node);
    if (efs == -1 || efd == -1) {
        return -1;
    }
    return std::max(efs, efd);
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The latest cycle, starting from given cycle backwards, where a
 * resource of the type managed by this broker can be assigned to the
 * given node. Considers source and destination terminals independently
 * and compares the results. Returns -1 if no assignment is possible.
 */
int
ExecutionPipelineBroker::latestCycle(int cycle, const MoveNode& node) const {
    return std::min(
        latestFromSource(cycle, node), latestFromDestination(cycle, node));
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
ExecutionPipelineBroker::isAlreadyAssigned(int, const MoveNode&) const {
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
ExecutionPipelineBroker::isApplicable(const MoveNode&) const {
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

        int resourceNumber =
            fu->pipelineElementCount() + fu->operationPortCount();

        // there is one execution pipeline object per FU, so it has same name
        // with a prefix
        ExecutionPipelineResource* epResource =
            new ExecutionPipelineResource(
                "ep_" + fu->name(),
                fu->maxLatency(),
                resourceNumber);
        for (int j = 0; j < fu->operationCount(); j++) {
            ExecutionPipeline* ep = fu->operation(j)->pipeline();
            for (int l = 0; l < ep->latency(); l++ ) {
                for (int k = 0; k < fu->pipelineElementCount(); k++) {
                    PipelineElement *pe = fu->pipelineElement(k);
                    if (ep->isResourceUsed(pe->name(),l)){
                        epResource->setResourceUse(
                            StringTools::stringToUpper(
                            fu->operation(j)->name()), l, k);
                    }
                }
                for (int k = 0; k < fu->operationPortCount(); k++) {
                    FUPort* fuPort = fu->operationPort(k);
                    if (ep->isPortUsed(*fuPort,l)) {
                        epResource->setResourceUse(
                            StringTools::stringToUpper(
                                fu->operation(j)->name()),
                                l,fu->pipelineElementCount()+k);
                    }
                }
            }
        }
        ResourceBroker::addResource(*fu, epResource);
        fuPipelineMap_.insert(
            pair<SchedulingResource*, const FunctionUnit*>(epResource, fu));
    }

    ControlUnit* gcu = target.controlUnit();
    int resNumber = gcu->pipelineElementCount()+
            gcu->operationPortCount();
    ExecutionPipelineResource* epResource =
        new ExecutionPipelineResource(
            "ep_" + gcu->name(),
            gcu->maxLatency(),
            resNumber);

    for (int i = 0; i < gcu->operationCount(); i++) {
        ExecutionPipeline* ep = gcu->operation(i)->pipeline();
        for (int l = 0; l < ep->latency(); l++ ) {
            for (int k = 0; k < gcu->pipelineElementCount(); k++) {
                PipelineElement *pe =gcu->pipelineElement(k);
                if (ep->isResourceUsed(pe->name(),l)){
                    epResource->setResourceUse(
                        StringTools::stringToUpper(
                        gcu->operation(i)->name()), l, k);
                }
            }
            for (int k = 0; k < gcu->operationPortCount(); k++) {
                FUPort* fuPort = gcu->operationPort(k);
                if (ep->isPortUsed(*fuPort,l)) {
                    epResource->setResourceUse(
                        StringTools::stringToUpper(
                        gcu->operation(i)->name()), l,
                        gcu->pipelineElementCount()+k);
                }
            }
        }
    }

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
ExecutionPipelineBroker::latestFromSource(int cycle, const MoveNode& node)
    const{

    if (!node.isSourceOperation()) {
        return cycle;
    }
    ProgramOperation& sourceOp = node.sourceOperation();
    const MoveNode* triggerNode = NULL;
    const MoveNode* lastNode = NULL;
    int minCycle = -1;
    // Source is result read, we test all operands and next result
    for (int i = 0; i < sourceOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &sourceOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            minCycle = std::max(tempNode->cycle(), minCycle);
            lastNode = tempNode;
            if (tempNode->move().isTriggering()) {
                triggerNode = tempNode;
            }
        }
    }
    if (minCycle == -1) {
        return cycle;
    }
    if (minCycle > cycle) {
        // Some operand is already later then where we started to backtrack
        return -1;
    }
    if (triggerNode != NULL) {
        
        if (minCycle >= node.earliestResultReadCycle()) {
            throw InvalidData(__FILE__, __LINE__, __func__, 
                "Some operand move is written after the result move "
                "is ready!");
        }
        if (node.earliestResultReadCycle() > cycle) {
            // Result is available later then where we started to backtrack
            return -1;
        } else {
            minCycle = node.earliestResultReadCycle();
        }
    }
    // minCycle has latest of operand writes or earliest result read cycle
    // find next read of same FU with different PO
    SchedulingResource& res =
        resourceOf(lastNode->move().destination().functionUnit());
    ExecutionPipelineResource* ep =
        dynamic_cast<ExecutionPipelineResource*>(&res);
    // last available cycle is one lower then next write
    int result = ep->nextResultCycle(minCycle, node) - 1;
    if (cycle <= result) {
        return cycle;
    }
    return result;
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
    const MoveNode& node) const{

    if (!node.isDestinationOperation()) {
        return cycle;
    }
    ProgramOperation& destOp = node.destinationOperation();
    const MoveNode* triggerNode = NULL;
    for (int i = 0; i < destOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &destOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            if (tempNode->move().isTriggering()) {
                triggerNode = tempNode;
            }
        }
    }
    int maxTriggerCycle = INT_MAX;
    for (int i = 0; i < destOp.outputMoveCount(); i++) {
        const MoveNode* tempNode = &destOp.outputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            const TTAMachine::HWOperation& hwop =
                *node.move().destination().functionUnit().operation(
                    destOp.operation().name());
            const int outputIndex =
                tempNode->move().source().operationIndex();
            maxTriggerCycle = std::min(maxTriggerCycle,
                tempNode->cycle() - hwop.latency(outputIndex));
        }
    }
    if (triggerNode != NULL) {
        if (cycle < triggerNode->cycle()) {
            return -1;
        }
        return triggerNode->cycle();
    }
    if (maxTriggerCycle != INT_MAX) {
        if (cycle < maxTriggerCycle) {
            return -1;
        }
        return maxTriggerCycle;
    }
    return cycle;
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
ExecutionPipelineBroker::earliestFromSource(int cycle, const MoveNode& node)
    const {

    if (!node.isSourceOperation()) {
        return cycle;
    }
    ProgramOperation& sourceOp = node.sourceOperation();
    const MoveNode* triggerNode = NULL;
    int minCycle = -1;
    for (int i = 0; i < sourceOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &sourceOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            minCycle = std::max(tempNode->cycle(), minCycle);
            if (tempNode->move().isTriggering()) {
                triggerNode = tempNode;
            }
        }
    }
    if (minCycle == -1) {
        // no operands scheduled for result of given PO
        // so earliest cycle is 0 + latency or tested cycle
        const TTAMachine::HWOperation& hwop =
            *node.move().source().functionUnit().operation(
                sourceOp.operation().name());
        const int outputIndex =
                node.move().source().operationIndex();
        minCycle = cycle + hwop.latency(outputIndex);
    }
    if (minCycle < cycle) {
        // cycle gives lower bound of what we are interested in
        minCycle = cycle;
    }
    if (triggerNode != NULL) {
        // trigger was found, earliest read can be tested
        // using earliestResultReadCycle
        minCycle = node.earliestResultReadCycle();

        SchedulingResource& res =
            resourceOf(triggerNode->move().destination().functionUnit());
        ExecutionPipelineResource* ep =
                dynamic_cast<ExecutionPipelineResource*>(&res);
        int result = ep->nextResultCycle(minCycle, node);
        if (result <= cycle) {
            // result of other PO is written before we try to read result
            return -1;
        }
    }

    return std::max(minCycle, cycle);
}
/**
 * Return earliest cycle, starting from given parameter and going towards
 * INT_MAX, in which the node can be scheduled. Taking into account
 * destination terminal of Move (operand write).
 *
 * @param cycle Starting cycle for tests
 * @param node MoveNode to find earliest cycle for
 * @return The earliest cycle in which the MoveNode can be scheduled, -1 if
 * scheduling is not possible, 0 if destination is not FUPort
 */

int
ExecutionPipelineBroker::earliestFromDestination(
    int cycle,
    const MoveNode& node) const {

    if (!node.isDestinationOperation()) {
        return cycle;
    }
    ProgramOperation& destOp = node.destinationOperation();
    int minCycle = -1;
    for (int i = 0; i < destOp.inputMoveCount(); i++) {
        const MoveNode* tempNode = &destOp.inputMove(i);
        if (tempNode->isScheduled() && tempNode != &node) {
            minCycle = std::max(tempNode->cycle(), minCycle);

            if (tempNode->move().isTriggering()) {
                int triggerCycle = tempNode->cycle();
                if (triggerCycle < cycle) {
                // trying to schedule operand after trigger
                    return -1;
                }
                return cycle;
            }
        }
    }
    int minResultCycle = INT_MAX;
    for (int i = 0; i < destOp.outputMoveCount(); i++) {
        const MoveNode* tempNode = &destOp.outputMove(i);
        if (tempNode->isScheduled()) {
            const TTAMachine::HWOperation& hwop =
                *node.move().destination().functionUnit().operation(
                    destOp.operation().name());
            const int outputIndex =
                tempNode->move().source().operationIndex();
            minResultCycle =
                std::min(
                    minResultCycle,
                    tempNode->cycle() - hwop.latency(outputIndex));
        }
    }
    if (minResultCycle < cycle) {
        // tested cycle is larger then last trigger cycle for results
        // already scheduled
        return -1;
    }
    if (node.move().isTriggering() && minCycle >= cycle) {
        if (minCycle <= minResultCycle) {
        // Some of the operands are already scheduled in minCycle
            return minCycle;
        } else {
            return -1;
        }
    }
    return cycle;
}
