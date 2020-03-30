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
 * @file InputFUBroker.cc
 *
 * Implementation of InputFUBroker class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "InputFUBroker.hh"
#include "InputFUResource.hh"
#include "ResourceMapper.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "ExecutionPipeline.hh"
#include "HWOperation.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Operation.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"
#include "ProgramOperation.hh"
#include "MapTools.hh"
#include "ProgramAnnotation.hh"
#include "AssocTools.hh"
#include "TCEString.hh"
#include "StringTools.hh"
#include "UniversalFunctionUnit.hh"
#include "ResourceManager.hh"
#include "SchedulingResource.hh"
#include "TerminalFUPort.hh"
#include "MoveNodeSet.hh"
#include "InputPSocketResource.hh"
#include "MachineConnectivityCheck.hh"
#include "DataDependenceGraph.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 */
InputFUBroker::InputFUBroker(std::string name,
        unsigned int initiationInterval):
    FUBroker(name, initiationInterval) {
}

/**
 * Destructor.
 */
InputFUBroker::~InputFUBroker(){
}

/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 */
SchedulingResourceSet
InputFUBroker::allAvailableResources(int cycle, const MoveNode& node,
                                     const TTAMachine::Bus*,
                                     const TTAMachine::FunctionUnit*,
                                     const TTAMachine::FunctionUnit* dstFU,
                                     int,
                                     const TTAMachine::ImmediateUnit*,
                                     int) const {
    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    Move& move = const_cast<MoveNode&>(node).move();
    TerminalFUPort& dst = static_cast<TerminalFUPort&>(move.destination());

    SchedulingResourceSet resourceSet;
    ResourceMap::const_iterator resIter = resMap_.begin();

    if (dynamic_cast<const SpecialRegisterPort*>(&dst.port()) != NULL) {
        // only gcu applies
        while (resIter != resMap_.end()) {
            const ControlUnit* gcu =
                dynamic_cast<const ControlUnit*>((*resIter).first);
            if (gcu != NULL) {
                InputFUResource* fuRes =
                    static_cast<InputFUResource*>((*resIter).second);
                if (fuRes->isAvailable(cycle)) {
                    resourceSet.insert(*fuRes);
                }
                return resourceSet;
            }
            resIter++;
        }
        abortWithError("No GCU found!");
    }

    int opIndex = dst.operationIndex();
    Operation& op = dst.hintOperation();
    std::string opName = StringTools::stringToLower(op.name());

    // check if a unit has already been assigned to some node
    // of the same operation and use it.
    const FunctionUnit* foundFU = dstFU;

    DataDependenceGraph::NodeSet processedInputNodes;
    auto a = findDstFUOfMove(node, foundFU, processedInputNodes);
    if (a.first) {
        foundFU = a.second;
    } else { // conflicting FUs, cannot schedule
        if (foundFU != nullptr) {
            return SchedulingResourceSet();
        }
    }

    if (foundFU) {
        InputFUResource& foundFURes =
            static_cast<InputFUResource&>(*resourceOf(*foundFU));

        // Find what is the port on a new FU for given
        // operation index. Find a socket for testing.
        HWOperation* hwOp = foundFU->operationLowercase(opName);
        Socket* soc = hwOp->port(opIndex)->inputSocket();

        FUPort* tempPort =
            dynamic_cast<FUPort*>(hwOp->port(opIndex));
        if (tempPort == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Target is missing necessary FUPort!");
        }
        bool triggering = false;
        if (tempPort->isTriggering()) {
            triggering = true;
        }

        InputPSocketResource* pSocket = NULL;
        try {
            pSocket = &static_cast<InputPSocketResource&>(
                resourceMapper().resourceOf(*soc));
        } catch (const KeyNotFound& e) {
            std::string msg = "InputFUBroker: finding ";
            msg += " resource for Socket ";
            msg += " failed with error: ";
            msg += e.errorMessageStack();
            throw KeyNotFound(
                __FILE__, __LINE__, __func__, msg);
        }
        if (foundFURes.canAssign(
                cycle, node, *pSocket, triggering)) {
            resourceSet.insert(foundFURes);
        }
        return resourceSet;
    }

    std::set<TCEString> candidateFUs;
    std::set<TCEString> allowedFUs;
    std::set<TCEString> rejectedFUs;
    // not all nodes have dest operation info set as the RM can be called
    // for single moves 
    if (node.isDestinationOperation()) {
        // check if the move or other moves in the same program operation have 
        // candidate FUs set which limits the choice of FU for the node
        ProgramOperation& destOp = node.destinationOperation();
        for (int in = 0; in < destOp.inputMoveCount(); ++in) {
            MoveNode& n = destOp.inputMove(in);
            MachineConnectivityCheck::addAnnotatedFUs(
                candidateFUs, n.move(),
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
            MachineConnectivityCheck::addAnnotatedFUs(
                allowedFUs, n.move(),
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST);
            MachineConnectivityCheck::addAnnotatedFUs(
                rejectedFUs, n.move(),
                TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_DST);
        }
    }
    
    // if the move has already the FU assigned, force it to be chosen
    // kludge implementation that use the candidate set code
    const TTAMachine::FunctionUnit& targetFU =
        node.move().destination().functionUnit();
    if (dynamic_cast<const UniversalFunctionUnit*>(&targetFU) == NULL &&
        dynamic_cast<const TTAMachine::ControlUnit*>(&targetFU) == NULL)  {
        candidateFUs.clear();
        candidateFUs.insert(node.move().destination().functionUnit().name());
        debugLogRM(
            TCEString("Added candidate FU duo pre-set fu") +
            node.move().destination().functionUnit().name());
    }

    // find units that support operation and are available
    while (resIter != resMap_.end()) {
        const FunctionUnit* unit =
            static_cast<const FunctionUnit*>((*resIter).first);

        assert(unit != NULL);
        // in case the unit is limited by a candidate set, skip FUs that are
        // not in it
        debugLogRM(TCEString("checking ") << unit->name());
        if (candidateFUs.size() > 0 &&
            !AssocTools::containsKey(candidateFUs, unit->name())) {
            debugLogRM(
                TCEString("skipped ") << unit->name() << " because it was not "
                " in the candidate set.");
            ++resIter;
            continue;
        }

        if (allowedFUs.size() > 0 &&
            !AssocTools::containsKey(allowedFUs, unit->name())) {
            debugLogRM(
                TCEString("skipped ") << unit->name() << " because it was not "
                " in the allowed set.");
            ++resIter;
            continue;
        }

        if (AssocTools::containsKey(rejectedFUs, unit->name())) {
            debugLogRM(
                TCEString("skipped ") << unit->name() << " because it was"
                " in the rejected set.");
            ++resIter;
            continue;
        }

        debugLogRM(TCEString("Found ") + unit->name());

        const SchedulingResource& res = *resourceOf(*unit);
        if (res.isInputFUResource()) {
            const InputFUResource& fuRes =
                static_cast<InputFUResource&>(*resourceOf(*unit));
            if (unit->hasOperationLowercase(opName)) {
                debugLogRM(TCEString("found FU with the op ") + opName);
                HWOperation* hwOp = unit->operationLowercase(opName);
                assert(hwOp != NULL);                
                Socket* soc = hwOp->port(opIndex)->inputSocket();

                FUPort* tempPort =
                    dynamic_cast<FUPort*>(hwOp->port(opIndex));
                if (tempPort == NULL){
                    throw InvalidData(
                        __FILE__, __LINE__, __func__, 
                        "Target is missing necessary FUPort!");                               
                }
                bool triggering = false;
                if (tempPort->isTriggering()) {
                    triggering = true;
                }

                InputPSocketResource* pSocket = NULL;
                try {
                    pSocket = &static_cast<InputPSocketResource&>(
                        resourceMapper().resourceOf(*soc));
                } catch (const KeyNotFound& e) {
                    std::string msg = "InputFUBroker: finding ";
                    msg += " resource for Socket ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }                                                            
                if (fuRes.canAssign(cycle, node, *pSocket, triggering)) {
                    resourceSet.insert(*(*resIter).second);
                } else {
                    debugLogRM("could not assign the fuRes to it.");
                }
            } else {
                debugLogRM(TCEString("does not have operation ") + opName);
            }
        }
        resIter++;
    }
    if (resourceSet.count() == 0) {
        debugLogRM("InputFUBroker returned an empty resource set.");
    }

    return resourceSet;
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
 * given node.
 */
int
InputFUBroker::earliestCycle(int, const MoveNode&,
                             const TTAMachine::Bus*,
                             const TTAMachine::FunctionUnit*,
                             const TTAMachine::FunctionUnit*, int,
                             const TTAMachine::ImmediateUnit*, int) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
InputFUBroker::latestCycle(int, const MoveNode&,
                           const TTAMachine::Bus*,
                           const TTAMachine::FunctionUnit*,
                           const TTAMachine::FunctionUnit*, int,
                           const TTAMachine::ImmediateUnit*, int) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle. Not used.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle) and recorded for node that it is using it (could be inUse
 * because some other operation is executed).
 */
bool
InputFUBroker::isAlreadyAssigned(
    int cycle, const MoveNode& node, const TTAMachine::Bus*) const {
    cycle = instructionIndex(cycle);
    Terminal& dst = const_cast<MoveNode&>(node).move().destination();
    if (dst.isFUPort()) {
        const FunctionUnit& fu = dst.functionUnit();
        if (hasResourceOf(fu)) {
            if (MapTools::containsKey(assignedResources_, &node)){
                return true;
            }
        }
    }
    return false;
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * @param node Node.
 * @return True if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 */
bool
InputFUBroker::isApplicable(
    const MoveNode& node, const TTAMachine::Bus*) const {
    Move& move = const_cast<MoveNode&>(node).move();
    return move.destination().isFUPort();
}

/**
 * Mark given resource as in use for the given node, and assign the
 * corresponding machine part (if applicable) to the node's move.
 *
 * If the node is already assigned to given resource, this method does
 * nothing.
 *
 * @param cycle Cycle.
 * @param node Node to assign.
 * @param res Resource representing Input of and FU
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
InputFUBroker::assign(
    int cycle, MoveNode& node, SchedulingResource& res, int, int) {
    //    cycle = instructionIndex(cycle);
    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw WrongSubclass(__FILE__, __LINE__, __func__, msg);
    }

    if (!hasResource(res)) {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    InputFUResource& fuRes = static_cast<InputFUResource&>(res);

    Move& move = const_cast<MoveNode&>(node).move();
    TerminalFUPort& dst = static_cast<TerminalFUPort&>(move.destination());

    if (dynamic_cast<const SpecialRegisterPort*>(&dst.port()) != NULL) {

        const ControlUnit* gcu =
            dynamic_cast<const ControlUnit*>(&machinePartOf(fuRes));

        if (gcu != NULL) {
            TerminalFUPort* newDst =
                new TerminalFUPort(*gcu->returnAddressPort());
            move.setDestination(newDst);
            fuRes.assign(cycle, node);
            assignedResources_.insert(
                std::pair<const MoveNode*, SchedulingResource*>(
                     &node,&fuRes));
            return;
        } else {
            abortWithError("GCU required but tried to assign something else!");
        }
    }

    int opIndex = dst.operationIndex();
    Operation& op = dst.hintOperation();

    const FunctionUnit& unit =
        static_cast<const FunctionUnit&>(machinePartOf(fuRes));
    HWOperation* hwOp = unit.operation(op.name());
    TerminalFUPort* newDst = new TerminalFUPort(*hwOp, opIndex);
    newDst->setProgramOperation(dst.programOperation());
    move.setDestination(newDst);
    fuRes.assign(cycle, node);
    assignedResources_.insert(
        std::pair<const MoveNode*, SchedulingResource*>(&node, &fuRes));
}

/**
 * Free the resource type managed by this broker and unassign it from
 * given node.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does nothing.
 *
 * @param node Node.
 */
void
InputFUBroker::unassign(MoveNode& node) {
    if (!isApplicable(node)) {
        return;
    }
    if (!MapTools::containsKey(assignedResources_, &node)) {
        return;
    } else {
        Move& move = const_cast<MoveNode&>(node).move();
        TerminalFUPort& dst =
            static_cast<TerminalFUPort&>(move.destination());
        InputFUResource& res =
            static_cast<InputFUResource&>(*resourceOf(dst.functionUnit()));
        res.unassign(node.cycle(), node);
        assignedResources_.erase(&node);
        return;
    }

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
InputFUBroker::buildResources(const TTAMachine::Machine& target) {
    
    Machine::FunctionUnitNavigator navi = target.functionUnitNavigator();
    for (int i = 0; i < navi.count(); i++) {
        FunctionUnit* fu = navi.item(i);
        InputFUResource* fuResource = new InputFUResource(
            fu->name(), fu->operationCount(), initiationInterval_);
        ResourceBroker::addResource(*fu, fuResource);
    }

    ControlUnit* gcu = target.controlUnit();
    InputFUResource* fuResource = new InputFUResource(
        gcu->name(), gcu->operationCount(), initiationInterval_);
    ResourceBroker::addResource(*gcu, fuResource);
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
InputFUBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const FunctionUnit* fu =
            dynamic_cast<const FunctionUnit*>((*resIter).first);
        if (fu == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Broker has other resource then FU registered!");                               
        }

        SchedulingResource* fuResource = (*resIter).second;

        for (int i = 0; i < fu->portCount(); i++) {
            Port* port = fu->port(i);
            if (port->inputSocket() != NULL) {
                SchedulingResource& depRes =
                    mapper.resourceOf(*port->inputSocket());
                fuResource->addToDependentGroup(0, depRes);
            }
        }

        int fuCount = mapper.resourceCount(*fu);
        for (int i = 0; i < fuCount; i++) {
            SchedulingResource* depRes = &mapper.resourceOf(*fu, i);
            if (depRes->isExecutionPipelineResource()){
                fuResource->addToDependentGroup(1, *depRes);
                break;
            }
        }
    }
}
