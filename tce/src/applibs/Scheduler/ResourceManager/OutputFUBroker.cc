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
 * @file OutputFUBroker.cc
 *
 * Implementation of OutputFUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "OutputFUBroker.hh"
#include "OutputFUResource.hh"
#include "ResourceMapper.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "Operation.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "ProgramOperation.hh"
#include "MapTools.hh"
#include "AssocTools.hh"
#include "ProgramAnnotation.hh"
#include "TCEString.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 */
OutputFUBroker::OutputFUBroker(std::string name) : ResourceBroker(name) {
}

/**
 * Destructor.
 */
OutputFUBroker::~OutputFUBroker(){
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
OutputFUBroker::allAvailableResources(
    int cycle,
    const MoveNode& node) const {

    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    Move& move = const_cast<MoveNode&>(node).move();
    TerminalFUPort& src = dynamic_cast<TerminalFUPort&>(move.source());

    SchedulingResourceSet resourceSet;
    ResourceMap::const_iterator resIter = resMap_.begin();

    if (dynamic_cast<const SpecialRegisterPort*>(&src.port()) != NULL) {
        // only gcu applies
        while (resIter != resMap_.end()) {
            const ControlUnit* gcu =
                dynamic_cast<const ControlUnit*>((*resIter).first);
            if (gcu != NULL) {
                if ((*resIter).second->isAvailable(cycle)) {
                    resourceSet.insert(*(*resIter).second);
                }
                return resourceSet;
            }
            resIter++;
        }
        abortWithError("No GCU found!");
    }

    Operation& op = src.hintOperation();
    int opIndex = src.operationIndex();
    // check if a unit has already been assigned to some node
    // of the same operation and use it.
    if (node.isSourceOperation()) {
        ProgramOperation& PO = node.sourceOperation();
        if (PO.isComplete()) {
            for (int i = 1; PO.hasInputNode(i); i++) {
                MoveNodeSet nodeSet = PO.inputNode(i);
                for (int j = 0; j < nodeSet.count(); j++) {
                    Move& move = nodeSet.at(j).move();
                    Terminal& dst = move.destination();
                    if (!dst.isFUPort()) {
                    throw InvalidData(__FILE__, __LINE__, __func__, 
                        "Operand move does not write FU!");
                    }                    
                    const FunctionUnit& fu = dst.functionUnit();
                    if (hasResourceOf(fu)) {
                        OutputFUResource& fuRes =
                            dynamic_cast<OutputFUResource&>(resourceOf(fu));
                    // Find what is the port on a new FU for given
                    // operation index. Find a socket for testing.
                        HWOperation* hwOp = fu.operation(op.name());
                        Socket* soc = hwOp->port(opIndex)->outputSocket();
                        OutputPSocketResource* pSocket = NULL;
                        try {
                            pSocket = &dynamic_cast<OutputPSocketResource&>(
                                resourceMapper().resourceOf(*soc));
                        } catch (const KeyNotFound& e) {
                            std::string msg = "OutputFUBroker: finding ";
                            msg += " resource for Socket ";
                            msg += " failed with error: ";
                            msg += e.errorMessageStack();
                            throw KeyNotFound(
                                __FILE__, __LINE__, __func__, msg);
                        }
                        if (fuRes.canAssign(cycle, node,*pSocket)) {
                            resourceSet.insert(fuRes);
                        }
                        return resourceSet;
                    }
                }
            }
            for (int i = 1; PO.hasOutputNode(i); i++) {
                MoveNodeSet nodeSet = PO.outputNode(i);
                for (int j = 0; j < nodeSet.count(); j++) {
                    Move& move = nodeSet.at(j).move();
                    Terminal& src = move.source();
                    if (!src.isFUPort()) {
                        throw InvalidData(__FILE__, __LINE__, __func__, 
                            "Result move does not read FU!");
                    }                                        
                    
                    const FunctionUnit& fu = src.functionUnit();
                    if (hasResourceOf(fu)) {
                        OutputFUResource& fuRes =
                            dynamic_cast<OutputFUResource&>(resourceOf(fu));
                    // Find what is the port on a new FU for given
                    // operation index. Find a socket for testing.
                        HWOperation* hwOp = fu.operation(op.name());
                        Socket* soc = hwOp->port(opIndex)->outputSocket();
                        OutputPSocketResource* pSocket = NULL;
                        try {
                            pSocket = &dynamic_cast<OutputPSocketResource&>(
                                resourceMapper().resourceOf(*soc));
                        } catch (const KeyNotFound& e) {
                            std::string msg = "OutputFUBroker: finding ";
                            msg += " resource for Socket ";
                            msg += " failed with error: ";
                            msg += e.errorMessageStack();
                            throw KeyNotFound(
                                __FILE__, __LINE__, __func__, msg);
                        }
                        
                        if (fuRes.canAssign(cycle, node,*pSocket)) {
                            resourceSet.insert(fuRes);
                        }
                        return resourceSet;
                    }
                }
            }

        }
    }

    // check if the move has a candidate FU set which limits the
    // choice of FU for the node
    std::set<std::string> candidateFUs;
    if (node.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_SRC)) {

        const int annotationCount =
            node.move().annotationCount(
                TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_SRC);
        for (int i = 0; i < annotationCount; ++i) {
            std::string candidateFU =
                node.move().annotation(
                    i, TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_SRC).
                stringValue();

            candidateFUs.insert(candidateFU);
        }
    }

    // find units that support operation and are available at given cycle
    while (resIter != resMap_.end()) {
        const FunctionUnit* unit =
            dynamic_cast<const FunctionUnit*>((*resIter).first);
        // in case the unit is limited by a candidate set, skip FUs that are
        // not in it
        if (candidateFUs.size() > 0 &&
            !AssocTools::containsKey(candidateFUs, unit->name())) {
            ++resIter;
            continue;
        }
        if (unit->hasOperation(op.name())) {
            OutputFUResource& fuRes =
                dynamic_cast<OutputFUResource&>(resourceOf(*unit));
                // Find what is the port on a new FU for given
                // operation index. Find a socket for testing.
            HWOperation* hwOp = unit->operation(op.name());
            Socket* soc = hwOp->port(opIndex)->outputSocket();
            OutputPSocketResource* pSocket = NULL;
            try {
                pSocket = &dynamic_cast<OutputPSocketResource&>(
                    resourceMapper().resourceOf(*soc));
            } catch (const KeyNotFound& e) {
                std::string msg = "OutputFUBroker: finding ";
                msg += " resource for Socket ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }
            
            if (fuRes.canAssign(cycle, node,*pSocket)) {                
                resourceSet.insert(*(*resIter).second);
            }
        }
        resIter++;
    }
    return resourceSet;
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
 * @param res Resource representing Output of FU
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
OutputFUBroker::assign(int cycle, MoveNode& node, SchedulingResource& res)
    throw (Exception) {

    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw WrongSubclass(__FILE__, __LINE__, __func__, msg);
    }

    if (!hasResource(res)) {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    OutputFUResource& fuRes = dynamic_cast<OutputFUResource&>(res);
    Move& move = const_cast<MoveNode&>(node).move();
    TerminalFUPort& src = dynamic_cast<TerminalFUPort&>(move.source());

    if (dynamic_cast<const SpecialRegisterPort*>(&src.port()) != NULL) {
        const ControlUnit* gcu =
            dynamic_cast<const ControlUnit*>(&machinePartOf(res));
        if (gcu != NULL) {
            // output of GCU is ra read
            TerminalFUPort* newSrc =
                new TerminalFUPort(*gcu->returnAddressPort());
            move.setSource(newSrc);
            Socket* outputSocket = newSrc->port().outputSocket();
            
            if (outputSocket == NULL) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__, "GCU is missing RA port - "
                    "socket connection!");
            }
            assignedResources_.insert(
                std::pair<const MoveNode*, SchedulingResource*>(
                &node, &fuRes));
            return;
        } else {
            abortWithError(
                "GCU required but tried to assign something else!");
        }
    }

    int opIndex = src.operationIndex();
    Operation& op = src.hintOperation();
    const FunctionUnit& unit =
        dynamic_cast<const FunctionUnit&>(machinePartOf(res));
    HWOperation* hwOp = unit.operation(op.name());
    TerminalFUPort* newSrc = new TerminalFUPort(*hwOp, opIndex);
    move.setSource(newSrc);
    fuRes.assign(cycle, node);
    assignedResources_.insert(
        std::pair<const MoveNode*, SchedulingResource*>(
        &node, &fuRes));
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
OutputFUBroker::unassign(MoveNode& node) {
    if (!isApplicable(node)) {
        return;
    }
    if (MapTools::containsKey(assignedResources_, &node)) {
        Move& move = const_cast<MoveNode&>(node).move();
        TerminalFUPort& src = dynamic_cast<TerminalFUPort&>(move.source());
        SchedulingResource& res = resourceOf(src.functionUnit());
        const ControlUnit* gcu =
            dynamic_cast<const ControlUnit*>(&machinePartOf(res));

        // not ra read? unassign from fu
        if (gcu == NULL) {
            OutputFUResource& fuRes = dynamic_cast<OutputFUResource&>(res);
            fuRes.unassign(node.cycle(), node);
        } 
        // removed from assigned resources also if ra read
        assignedResources_.erase(&node);
    }
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
OutputFUBroker::earliestCycle(int, const MoveNode&) const {
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
OutputFUBroker::latestCycle(int, const MoveNode&) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle. not used.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 */
bool
OutputFUBroker::isAlreadyAssigned(int cycle, const MoveNode& node) const {
    Terminal& src = const_cast<MoveNode&>(node).move().source();
    if (src.isFUPort()) {
        const FunctionUnit& fu = src.functionUnit();
        if (hasResourceOf(fu)) {
            if (MapTools::containsKey(assignedResources_, &node)) {
                return true;
            }
        }
    }
    cycle = cycle; // avoid unused variable
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
OutputFUBroker::isApplicable(const MoveNode& node) const {
    Move& move = const_cast<MoveNode&>(node).move();
    return move.source().isFUPort();
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
OutputFUBroker::buildResources(const TTAMachine::Machine& target) {
    Machine::FunctionUnitNavigator navi = target.functionUnitNavigator();
    for (int i = 0; i < navi.count(); i++) {
        FunctionUnit* fu = navi.item(i);
        OutputFUResource* fuResource = new OutputFUResource(fu->name());
        ResourceBroker::addResource(*fu, fuResource);
    }
    ControlUnit* gcu = target.controlUnit();
    OutputFUResource* fuResource = new OutputFUResource(gcu->name());
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
OutputFUBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const FunctionUnit* fu =
            dynamic_cast<const FunctionUnit*>((*resIter).first);

        SchedulingResource* fuResource = (*resIter).second;
        for (int i = 0; i < fu->portCount(); i++) {
            Port* port = fu->port(i);
            if (port->outputSocket() != NULL) {
                try {
                    SchedulingResource& depRes =
                        mapper.resourceOf(*port->outputSocket());
                    fuResource->addToDependentGroup(0, depRes);
                } catch (const KeyNotFound& e) {
                    std::string msg = "OutputFUBroker: finding ";
                    msg += " resource for Socket ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }                                
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
    // todo: dependent execution pipeline to support trigger-on-result
}
