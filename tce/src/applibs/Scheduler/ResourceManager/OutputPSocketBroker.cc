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
 * @file OutputPSocketBroker.cc
 *
 * Implementation of OutputPSocketBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "OutputPSocketBroker.hh"
#include "OutputPSocketResource.hh"
#include "ResourceMapper.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "Terminal.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "TerminalRegister.hh"
#include "MoveNode.hh"

using std::string;

using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param name name for this broker.
 * @param fub reference to OutputFUBroker of this resource manager.
 * @param initiationInterval initiationinterval when doing loop scheduling.
 */
OutputPSocketBroker::OutputPSocketBroker(
    std::string name,
    ResourceBroker& fub,
    SimpleResourceManager* rm,
    unsigned int initiationInterval) : 
    ResourceBroker(name, initiationInterval), 
    outputFUBroker_(fub), busBroker_(NULL), rm_(rm) {
}

/**
 * Destructor.
 */
OutputPSocketBroker::~OutputPSocketBroker(){
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
OutputPSocketBroker::allAvailableResources(
    int cycle,
    const MoveNode& node,
    const TTAMachine::Bus* preassignedBus,
    const TTAMachine::FunctionUnit*,
    const TTAMachine::FunctionUnit*,
    int,
    const TTAMachine::ImmediateUnit*, int) const {

    cycle = instructionIndex(cycle);
    if (!isApplicable(node, preassignedBus)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    TTAProgram::Move& move = const_cast<MoveNode&>(node).move();

    SchedulingResourceSet resourceSet;

    if (move.source().isFUPort()) {
        // psocket is implicit by fubroker choice of FU
        Socket& outputSocket = *move.source().port().outputSocket();
        SchedulingResource* res = resourceOf(outputSocket);
        if (res->canAssign(cycle,node)) {
            resourceSet.insert(*res);
        }
        return resourceSet;
    }
    if (move.source().isGPR()) {
        // assign psocket for reading rf
        const RegisterFile& rf = move.source().registerFile();
        for (int i = 0; i < rf.portCount(); i++) {
            Port& port = *rf.port(i);
            Socket* outputSocket = port.outputSocket();
            
            if (outputSocket != NULL) {
                SchedulingResource* res = resourceOf(*outputSocket);
                if (res->canAssign(cycle, node)) {
                    resourceSet.insert(*res);
                }
            }
        }
        return resourceSet;
    }
    if (move.source().isImmediateRegister()) {
        // assign psocket for reading IU
        const ImmediateUnit& iu = move.source().immediateUnit();
        for (int i = 0; i < iu.portCount(); i++) {
            Port& port = *iu.port(i);
            Socket* outputSocket = port.outputSocket();
            if (outputSocket != NULL) {
                SchedulingResource* res = resourceOf(*outputSocket);
                if (res->canAssign(cycle, node)) {
                    resourceSet.insert(*res);
                }
            }
        }
    }
    return resourceSet;
}

/**
 * Return true if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 */
bool
OutputPSocketBroker::isAnyResourceAvailable(
    int cycle, const MoveNode& node,
    const TTAMachine::Bus* preassignedBus,
    const TTAMachine::FunctionUnit*,
    const TTAMachine::FunctionUnit*, int,
    const TTAMachine::ImmediateUnit*, int) const {

    cycle = instructionIndex(cycle);
    if (!isApplicable(node, preassignedBus)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    TTAProgram::Move& move = const_cast<MoveNode&>(node).move();

    SchedulingResourceSet resourceSet;

    if (move.source().isFUPort()) {
        // psocket is implicit by fubroker choice of FU
        Socket& outputSocket = *move.source().port().outputSocket();
        SchedulingResource* res = resourceOf(outputSocket);
        if (res->canAssign(cycle,node)) {
            return true;
        } else {
            return false;
        }
    }
    if (move.source().isGPR()) {
        // assign psocket for reading rf
        const RegisterFile& rf = move.source().registerFile();
        for (int i = 0; i < rf.portCount(); i++) {
            Port& port = *rf.port(i);
            Socket* outputSocket = port.outputSocket();
            
            if (outputSocket != NULL) {
                SchedulingResource* res = resourceOf(*outputSocket);
                if (res->canAssign(cycle, node)) {
                    return true;
                }
            }
        }
        return false;
    }
    if (move.source().isImmediateRegister()) {
        // assign psocket for reading IU
        const ImmediateUnit& iu = move.source().immediateUnit();
        for (int i = 0; i < iu.portCount(); i++) {
            Port& port = *iu.port(i);
            Socket* outputSocket = port.outputSocket();
            if (outputSocket != NULL) {
                SchedulingResource* res = resourceOf(*outputSocket);
                if (res->canAssign(cycle, node)) {
                    return true;
                }
            }
        }
        return false;
    }
    return false;
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
 * @param res Resource representing Output PSocket
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
OutputPSocketBroker::assign(
    int cycle,
    MoveNode& node,
    SchedulingResource& res,
    int, int) {

    cycle = instructionIndex(cycle);
    if (!hasResource(res)) {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    TTAProgram::Move& move = node.move();

    if (move.source().isGPR() || move.source().isImmediateRegister()) {

        Port* port = NULL;
        const Socket& socket =
            static_cast<const Socket&>(machinePartOf(res));
        for (int i = 0; i < socket.portCount(); i++) {
            if (socket.port(i)->outputSocket() == &socket) {
                if ((move.source().isGPR() &&
                     socket.port(i)->parentUnit() !=
                     &move.source().registerFile()) ||
                    (move.source().isImmediateRegister() &&
                     socket.port(i)->parentUnit() !=
                     &move.source().immediateUnit()))
                    continue;
                port = socket.port(i);
                break;
            }
        }
        if (port == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Broker can not find necesary Port!");
        }
        TTAProgram::TerminalRegister* newSrc = new TerminalRegister(
            *port, move.source().index());
        move.setSource(newSrc);
    }
    res.assign(cycle, node);
    assignedResources_.insert(
        std::pair<const MoveNode*, SchedulingResource*>(&node, &res));
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
OutputPSocketBroker::unassign(MoveNode& node) {
    if (MapTools::containsKey(assignedResources_, &node)) {
        SchedulingResource* res = 
            MapTools::valueForKey<SchedulingResource*>(
                    assignedResources_,&node);
        res->unassign(node.cycle(), node);
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
OutputPSocketBroker::earliestCycle(int, const MoveNode&,
                                   const TTAMachine::Bus*,
                                   const TTAMachine::FunctionUnit*,
                                   const TTAMachine::FunctionUnit*,
                                   int, const TTAMachine::ImmediateUnit*,
                                   int) const {
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
OutputPSocketBroker::latestCycle(int, const MoveNode&,
                                 const TTAMachine::Bus*,
                                 const TTAMachine::FunctionUnit*,
                                 const TTAMachine::FunctionUnit*, int,
                                 const TTAMachine::ImmediateUnit*,
                                 int) const {
    abortWithError("Not implemented.");
    return -1;
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
OutputPSocketBroker::isAlreadyAssigned(
    int cycle,
    const MoveNode& node, const TTAMachine::Bus* preassignedBus) const {
    cycle = instructionIndex(cycle);
    if (node.isSourceConstant() && 
        node.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        return true;
    }    
    if (node.isSourceConstant() && rm_ &&
        !rm_->canTransportImmediate(node, preassignedBus)) {
        return true;
    }
    Terminal& src = const_cast<MoveNode&>(node).move().source();
    if (src.isFUPort() || src.isGPR() || src.isImmediateRegister()) {
        const Port& port = src.port();
        if (port.outputSocket() == NULL)
            return false;
        SchedulingResource* res = resourceOf(*port.outputSocket());
        if (res != NULL && res->isInUse(cycle) &&
            MapTools::containsKey(assignedResources_, &node)) {
            return true;
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
OutputPSocketBroker::isApplicable(
    const MoveNode& node, const TTAMachine::Bus* preassignedBus) const {
    if (!node.isMove()) {
        return false;
    }
    Move& move = const_cast<MoveNode&>(node).move();
    // If node is annotated, it will be converted to LIMM
    // and so we will need to assign output PScoket
    if (node.isSourceConstant() && node.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        return true;
    }    
    if (node.isSourceConstant() &&
        rm_ && !rm_->canTransportImmediate(node, preassignedBus)) {
        return true;
    }
    return (move.source().isFUPort() ||
        move.source().isGPR() ||
        move.source().isImmediateRegister());
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
OutputPSocketBroker::buildResources(const TTAMachine::Machine& target) {

    Machine::SocketNavigator navi = target.socketNavigator();

    for (int i = 0; i < navi.count(); i++) {
        Socket* socket = navi.item(i);
        if (socket->direction() == Socket::OUTPUT) {
            OutputPSocketResource* opsResource =
                new OutputPSocketResource(socket->name(), initiationInterval_);
            ResourceBroker::addResource(*socket, opsResource);
        }
    }
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
OutputPSocketBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const Socket* socket =
            static_cast<const Socket*>((*resIter).first);

        SchedulingResource* socketResource = (*resIter).second;

        for (int i = 0; i < socket->portCount(); i++) {
            Port* port = socket->port(i);
            Unit* unit = port->parentUnit();
            if (dynamic_cast<FunctionUnit*>(unit) != NULL) {

                SchedulingResource& relRes = 
                    *outputFUBroker_.resourceOf(*unit);
                socketResource->addToRelatedGroup(0, relRes);
            } else if (dynamic_cast<ImmediateUnit*>(unit) != NULL) {
                try {
                    SchedulingResource& relRes = mapper.resourceOf(*unit);
                    socketResource->addToRelatedGroup(1, relRes);
                } catch (const KeyNotFound& e) {
                    std::string msg = "OutputPSocketBroker: finding ";
                    msg += " resource for IU ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }                                                            
            }
        }

        for (int i = 0; i < socket->segmentCount(); i++) {
            try {
                Segment* segment = socket->segment(i);
                Bus* bus = segment->parentBus();
                SchedulingResource& relRes = 
                    *busBroker_->resourceOf(*bus);
                socketResource->addToRelatedGroup(2, relRes);
            } catch (const KeyNotFound& e) {
                std::string msg = "OutputPSocketBroker: finding ";
                msg += " resource for Segment ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }                                                                        
        }
    }
}

/**
 * Gives reference to segmentbroker to this broker.
 *
 * Cannot be given in constructor because SegmentBroker is created later.
 */
void OutputPSocketBroker::setBusBroker(ResourceBroker& sb) {
    busBroker_ = &sb;
}
