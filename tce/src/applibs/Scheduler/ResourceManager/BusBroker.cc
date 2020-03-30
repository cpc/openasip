/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file BusBroker.cc
 *
 * Implementation of BusBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "BusBroker.hh"
#include "BusResource.hh"
#include "OutputPSocketResource.hh"
#include "ShortImmPSocketResource.hh"
#include "InputPSocketResource.hh"
#include "ResourceMapper.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "AssocTools.hh"
#include "MathTools.hh"
#include "MapTools.hh"
#include "MoveNode.hh"
#include "MachineConnectivityCheck.hh"
#include "SequenceTools.hh"
#include "TemplateSlot.hh"
#include "TerminalImmediate.hh"
#include "UniversalMachine.hh"

using std::string;
using std::set;
using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Constructor.
 *
 * @param name name for this broker.
 * @param ipsb reference to InputPSocketBroker of this RM.
 * @param opsb reference to OutputPSocketBroker of this RM.
 * @param initiationInterval initiationinterval when doing loop scheduling.
 */
BusBroker::BusBroker(
    std::string name, 
    ResourceBroker& ipBroker, 
    ResourceBroker& opBroker,
    const TTAMachine::Machine& mach,
    unsigned int initiationInterval) :
    ResourceBroker(name, initiationInterval), inputPSocketBroker_(ipBroker),
    outputPSocketBroker_(opBroker), hasLimm_(false), mach_(&mach) {
}

/**
 * Destructor.
 */
BusBroker::~BusBroker(){
    SequenceTools::deleteAllItems(shortImmPSocketResources_);
}

/**
 * Return true if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return True if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 */
bool
BusBroker::isAnyResourceAvailable(int cycle, const MoveNode& node,
                                  const TTAMachine::Bus* bus,
                                  const TTAMachine::FunctionUnit* srcFU,
                                  const TTAMachine::FunctionUnit* dstFU,
                                  int immWriteCycle,
                                  const TTAMachine::ImmediateUnit* immu,
                                  int immRegIndex) const {

    cycle = instructionIndex(cycle);
    SchedulingResourceSet allAvailableBuses =
        allAvailableResources(
	    cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
    return allAvailableBuses.count() > 0;
}

/**
 * Return one (any) resource managed by this broker that can be
 * assigned to the given node in the given cycle.
 *
 * If no change occurs to the state of the resources, the broker
 * should always return the same object. If a resource of the type
 * managed by this broker is already assigned to the node, it is
 * returned.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return One (any) resource managed by this broker that can be
 * assigned to the given node in the given cycle.
 * @exception InstanceNotFound If no available resource is found.
 */
SchedulingResource&
BusBroker::availableResource(int cycle, const MoveNode& node,
                             const TTAMachine::Bus* bus,
                             const TTAMachine::FunctionUnit* srcFU,
                             const TTAMachine::FunctionUnit* dstFU,
                             int immWriteCycle,
                             const TTAMachine::ImmediateUnit* immu,
                             int immRegIndex) const {

    cycle = instructionIndex(cycle);
    SchedulingResourceSet allAvailableBuses =
        allAvailableResources(cycle, node, bus, srcFU, dstFU, immWriteCycle,
			      immu, immRegIndex);
    if (allAvailableBuses.count() == 0) {
        string msg = "No available resource found.";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    } else {
        if (bus != NULL) {
            assert(allAvailableBuses.count() == 1);
        }
        return allAvailableBuses.resource(0);
    }
}

/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 */
SchedulingResourceSet
BusBroker::allAvailableResources(
    int cycle,
    const MoveNode& node,
    const TTAMachine::Bus* bus,
    const TTAMachine::FunctionUnit*,
    const TTAMachine::FunctionUnit*, int,
    const TTAMachine::ImmediateUnit*, int) const {

    cycle = instructionIndex(cycle);
    SchedulingResourceSet candidates;

    Move& move = const_cast<MoveNode&>(node).move();
    const Port* dstPort = &move.destination().port();
    Socket* inputSocket = dstPort->inputSocket();

    // In case bus was already assigned previously, pick only relevant resource.
    UniversalMachine& um = UniversalMachine::instance();
    BusResource* preassignedBus = NULL;
    if (&move.bus() != &um.universalBus()) {
        preassignedBus = dynamic_cast<BusResource*>(resourceOf(move.bus()));
    }
    if (bus != NULL) {
        preassignedBus = dynamic_cast<BusResource*>(resourceOf(*bus));
    }

    if (inputSocket == NULL) {
        string unit = dstPort->parentUnit()->name();
        string port = dstPort->name();
        string msg =
            "Tried to find bus for a move to '" + unit + "." + port +
            "' which has no connections to an input socket. "
            "Check operation bindings!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    InputPSocketResource* iPSocket = NULL;
    try {
        iPSocket = 
            static_cast<InputPSocketResource*>(
                inputPSocketBroker_.resourceOf(*inputSocket));
    } catch (const KeyNotFound& e) {
        std::string msg = "BusBroker: finding ";
        msg += " resource for Socket ";
        msg += " failed with error: ";
        msg += e.errorMessageStack();
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, msg);
    }                                                            

    OutputPSocketResource* oPSocket = NULL;

    if (move.source().isImmediate()) {

        // find a bus with appropriate immediate width that is connected to
        // destination
        ResourceMap::const_iterator resIter = resMap_.begin();
        while (resIter != resMap_.end()) {
            // look for bus resources with appropriate related shortimmsocket
            // resources
            BusResource* busRes =
                static_cast<BusResource*>((*resIter).second);
            if (busRes == NULL) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Bus broker has other then Bus Resource registered!");
            }

            ShortImmPSocketResource& immRes = findImmResource(*busRes);
            if (preassignedBus == NULL || busRes == preassignedBus) {
                if (canTransportImmediate(node, immRes) &&
                    busRes->canAssign(cycle, node, immRes, *iPSocket)) {
                    candidates.insert(*busRes);
                }
            }
            resIter++;
        }

    } else {

        const Port* srcPort = &move.source().port();
        Socket* outputSocket = srcPort->outputSocket();
        if (outputSocket == NULL) {
            string unit = srcPort->parentUnit()->name();
            string port = srcPort->name();
            string msg =
                "Tried to find bus for a move from '" + unit + "." + port +
                "' which has no connections to an output socket! Check "
                "operation bindings!";
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }

        oPSocket = NULL;
        try {
            oPSocket = 
            static_cast<OutputPSocketResource*>(
                outputPSocketBroker_.resourceOf(*outputSocket));
        } catch (const KeyNotFound& e) {
            std::string msg = "BusBroker: finding ";
            msg += " resource for Socket ";
            msg += " failed with error: ";
            msg += e.errorMessageStack();
            throw KeyNotFound(
                __FILE__, __LINE__, __func__, msg);
        }                                                            

        ResourceMap::const_iterator resIter = resMap_.begin();
        while (resIter != resMap_.end()) {
            BusResource* busRes =
                static_cast<BusResource*>((*resIter).second);
            if (preassignedBus == NULL ||
                busRes == preassignedBus) {
                if (busRes->canAssign(cycle, node, *oPSocket, *iPSocket)) {
                    candidates.insert(*busRes);
                }
            }
            resIter++;
        }
    }
    if (!move.isUnconditional()) {

        const Guard& guard = move.guard().guard();
        for (int i = 0; i < candidates.count(); i++) {
            SchedulingResource& busResource = candidates.resource(i);
            const Bus* aBus =
                static_cast<const Bus*>(&machinePartOf(busResource));
            if (aBus == NULL) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Bus Resource is missing bus in MOM!");
            }

            bool guardFound = false;
            for (int j = 0; j < aBus->guardCount(); j++) {
                Guard* busGuard = aBus->guard(j);
                if (busGuard->isEqual(guard)) {
                    guardFound = true;
                    break;
                }
            }

            if (!guardFound) {
                candidates.remove(busResource);
                i--;
            }
        }
    }
    return candidates;
}

/**
 * Tells whether the given resource is available for given node at
 * given cycle.
 */
bool
BusBroker::isAvailable(
    SchedulingResource& res, const MoveNode& node, int cycle,
    const TTAMachine::Bus* bus,
    const TTAMachine::FunctionUnit*,
    const TTAMachine::FunctionUnit*,
    int,
    const TTAMachine::ImmediateUnit*, int) const {

    if  (bus != NULL && resourceOf(*bus) != &res) {
        return false;
    }
    cycle = instructionIndex(cycle);

    Move& move = const_cast<MoveNode&>(node).move();
    const Port* dstPort = &move.destination().port();
    Socket* inputSocket = dstPort->inputSocket();

    if (inputSocket == NULL) {
        string unit = dstPort->parentUnit()->name();
        string port = dstPort->name();
        string msg =
            "Tried to find bus for a move to '" + unit + "." + port +
            "' which has no connections to an input socket. "
            "Check operation bindings!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    InputPSocketResource* iPSocket = NULL;
    try {
        iPSocket = 
            static_cast<InputPSocketResource*>(
                inputPSocketBroker_.resourceOf(*inputSocket));
    } catch (const KeyNotFound& e) {
        std::string msg = "BusBroker: finding ";
        msg += " resource for Socket ";
        msg += " failed with error: ";
        msg += e.errorMessageStack();
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, msg);
    }                                                            

    OutputPSocketResource* oPSocket = NULL;

    if (move.source().isImmediate()) {
        
        // look for bus resources with appropriate related shortimmsocket
        // resources
        BusResource* busRes =
            dynamic_cast<BusResource*>(&res);
        if (busRes == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Wrong type of resource for the broker given!");
        }
        
        ShortImmPSocketResource& immRes = findImmResource(*busRes);
        if (!canTransportImmediate(node, immRes) &&
            busRes->canAssign(cycle, node, immRes, *iPSocket)) {
            return false;
        }
    } else {
        const Port* srcPort = &move.source().port();
        Socket* outputSocket = srcPort->outputSocket();
        if (outputSocket == NULL) {
            string unit = srcPort->parentUnit()->name();
            string port = srcPort->name();
            string msg =
                "Tried to find bus for a move from '" + unit + "." + port +
                "' which has no connections to an output socket! Check "
                "operation bindings!";
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }

        oPSocket = NULL;
        try {
            oPSocket = 
            static_cast<OutputPSocketResource*>(
                outputPSocketBroker_.resourceOf(*outputSocket));
        } catch (const KeyNotFound& e) {
            std::string msg = "BusBroker: finding ";
            msg += " resource for Socket ";
            msg += " failed with error: ";
            msg += e.errorMessageStack();
            throw KeyNotFound(
                __FILE__, __LINE__, __func__, msg);
        }                                                            

        BusResource* busRes =
            dynamic_cast<BusResource*>(&res);
        if (busRes == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Wrong type of resource for the broker given!");
        }
        
        if (!busRes->canAssign(cycle, node, *oPSocket, *iPSocket)) {
            return false;
        }
    }
    if (!move.isUnconditional()) {

        const Guard& guard = move.guard().guard();
        const Bus* aBus =
            dynamic_cast<const Bus*>(&machinePartOf(res));
        if (aBus == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Bus Resource is missing bus in MOM!");
        }
        
        bool guardFound = false;
        for (int j = 0; j < aBus->guardCount(); j++) {
            Guard* busGuard = aBus->guard(j);
            if (busGuard->isEqual(guard)) {
                guardFound = true;
                break;
            }
        }
        
        if (!guardFound) {
            return false;
        }
    }
    return true;
}

/**
 * Mark given resource as in use for the given node, and assign the
 * corresponding machine part (if applicable) to the node's move.
 *
 * If the node is already assigned to given resource, this method does
 * nothing.
 *
 * @param cycle Cycle for which to assign bus
 * @param node Node which will be transfered using a bus
 * @param res ResourceObject referring to particular bus to assign
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
BusBroker::assign(
    int cycle, MoveNode& node, SchedulingResource& res, int, int) {

    cycle = instructionIndex(cycle);
    BusResource& busRes = static_cast<BusResource&>(res);
    Move& move = const_cast<MoveNode&>(node).move();
    if (hasResource(res)) {
        Bus& bus =
            const_cast<Bus&>(static_cast<const Bus&>(machinePartOf(res)));

        UniversalMachine& um = UniversalMachine::instance();
        if (&move.bus() != &um.universalBus()) {
            busPreassigned_[&node] = true;
            assert(&bus == &move.bus() &&
                   "preassigned bus which is different than selected bus?");
        } else {
            move.setBus(bus);
            busPreassigned_[&node] = false;
        }
        if (!move.isUnconditional()) {
            for (int j = 0; j < bus.guardCount(); j++) {
                Guard* busGuard = bus.guard(j);
                if (busGuard->isEqual(move.guard().guard())) {
                    move.setGuard(new MoveGuard(*busGuard));
                    break;
                }
            }
        }
        busRes.assign(cycle,node);
    } else {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    assignedResources_.insert(
        std::pair<const MoveNode*, SchedulingResource*>(&node, &busRes));
}

/**
 * Free the resource type managed by this broker and unassign it from
 * given node.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does
 * nothing.
 *
 * @param node Node to unassign.
 */
void
BusBroker::unassign(MoveNode& node) {

    if (MapTools::containsKey(assignedResources_, &node)) {
        Move& move = const_cast<MoveNode&>(node).move();
        const Bus& bus = move.bus();
        SchedulingResource* res = resourceOf(bus);
        BusResource& busRes = static_cast<BusResource&>(*res);
        busRes.unassign(node.cycle(),node);
        assignedResources_.erase(&node);
        if (!busPreassigned_[&node]) {
            UniversalMachine& um = UniversalMachine::instance();
            move.setBus(um.universalBus());
        }
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
BusBroker::earliestCycle(int, const MoveNode&, const TTAMachine::Bus*,
                         const TTAMachine::FunctionUnit*,
                         const TTAMachine::FunctionUnit*, int,
                         const TTAMachine::ImmediateUnit*,
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
BusBroker::latestCycle(int, const MoveNode&, const TTAMachine::Bus*,
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
BusBroker::isAlreadyAssigned(
    int cycle, const MoveNode& node, const TTAMachine::Bus*) const {
    if (!MapTools::containsKey(assignedResources_, &node)) {
        return false;
    }
    return isInUse(instructionIndex(cycle),node);
}

/**
 * Return true if given node already uses a resource of the type
 * managed by this broker and assignment appears to be valid.
 *
 * @param cycle Cycle to test
 * @param node Node to test
 * @return True if given node already uses a resource of the
 * type managed by the broker.
 */
bool
BusBroker::isInUse(int cycle, const MoveNode& node) const {
    cycle = instructionIndex(cycle);
    const Bus& bus = const_cast<MoveNode&>(node).move().bus();
    if (!hasResourceOf(bus)) {
        return false;
    }

    SchedulingResource* res = resourceOf(bus);
    BusResource& busRes = static_cast<BusResource&>(*res);
    return busRes.isInUse(cycle);
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
BusBroker::isApplicable(const MoveNode& mn, const TTAMachine::Bus*) const {
    return mn.isMove();
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
BusBroker::buildResources(const TTAMachine::Machine& target) {
    hasLimm_ = target.immediateUnitNavigator().count() > 0;
    Machine::BusNavigator navi = target.busNavigator();
    Machine::InstructionTemplateNavigator itn = 
        target.instructionTemplateNavigator();

    std::map<const Bus*,int> limmSlotCounts;
    
    // calculate count of limm slots associated with each bus.
    // used to priorize busses which do not get into way of limm writes.
    for (int i = 0; i < itn.count(); i++) {
        InstructionTemplate* it = itn.item(i);
        for (int j = 0; j < it->slotCount(); j++) {
            TemplateSlot* itSlot = it->slot(j);
            if (itSlot->destination() != NULL) {
                limmSlotCounts[itSlot->bus()]++; 
            }
        }
    }

    for (int i = 0; i < navi.count(); i++) {
        Bus* bus = navi.item(i);
        assert(bus->segmentCount() == 1);
        int socketCount = bus->segment(0)->connectionCount();
        BusResource* busResource = new BusResource(
            bus->name(), bus->width(),limmSlotCounts[bus], bus->guardCount(),
            bus->immediateWidth(), socketCount, initiationInterval_);
        ResourceBroker::addResource(*bus, busResource);
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
BusBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const Bus* bus = dynamic_cast<const Bus*>((*resIter).first);
        if (bus == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Bus broker has other then Bus Resource registered!");
        }        

        SchedulingResource* busResource = (*resIter).second;

        int immWidth = bus->immediateWidth();

        ShortImmPSocketResource* immSocketResource =
            new ShortImmPSocketResource(
                bus->name() + "Imm", immWidth, bus->signExtends(),
                initiationInterval_);

        shortImmPSocketResources_.push_back(immSocketResource);
        busResource->addToRelatedGroup(0, *immSocketResource);

        for (int i = 0; i < bus->segmentCount(); i++) {
            Segment* seg = bus->segment(i);
            for (int j = 0; j < seg->connectionCount(); j++) {
                Socket* socket = seg->connection(j);
                SchedulingResource* relRes = 
                    inputPSocketBroker_.resourceOf(*socket);
                if (relRes == NULL) {
                    relRes = outputPSocketBroker_.resourceOf(*socket);
                }
                if (relRes != NULL) {
                    busResource->addToRelatedGroup(0, *relRes);
                } else {
                    std::string msg = "BusBroker: finding ";
                    msg += " resource for Socket ";
                    msg += " failed. ";
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }
            }
        }
    }
}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
BusBroker::isBusBroker() const {
    return true;
}

/**
 * Return true if immediate in given node can be transported by any bus
 * in broker, with the guard which the move contains.
 *
 * @param node Node that contains immediate read.
 * @return True if immediate in given node can be transported by any bus
 * in broker.
 */
bool
BusBroker::canTransportImmediate(
    const MoveNode& node, const TTAMachine::Bus* preassignedBus) const {
    ResourceMap::const_iterator resIter = resMap_.begin();
    while (resIter != resMap_.end()) {
        BusResource* busRes = static_cast<BusResource*>((*resIter).second);
        ShortImmPSocketResource* immRes = &findImmResource(*busRes);
        const Bus* aBus = static_cast<const Bus*>(resIter->first);
        if (preassignedBus != NULL && aBus != preassignedBus) {
            resIter++;
            continue;
        }
        if (canTransportImmediate(node, *immRes)) {
            bool guardOK = false;
            if (node.move().isUnconditional()) {
                guardOK = true;
            } else {
                // We need to check that the bus contains the guard
                // which the move has. Only return true if the
                // guard is found from the bus.
                const Guard& guard = node.move().guard().guard();
                for (int j = 0; j < aBus->guardCount(); j++) {
                    Guard* busGuard = aBus->guard(j);
                    if (busGuard->isEqual(guard)) {
                        guardOK = true;
                        break;
                    }
                }
            }
            if (guardOK) {
                if (!hasLimm_) {
                    return true;
                } else {
                    if (MachineConnectivityCheck::busConnectedToDestination(
                            *aBus, node)) {
                        return true;
                    }
                }
            }
        }
        resIter++;
    }
    return false;
}

/**
 * Return true if immediate in given node can be transported by bus
 * that is represented by given p-socket resource.
 *
 * @param node Node that contains immediate read.
 * @param immRes P-socket resource representing write if immediate to related
 * bus.
 * @return True if immediate in given node can be transported by bus
 * that is represented by given p-socket resource.
 * @note Minimum number of bits for Control Flow operations is 10!
 */
bool
BusBroker::canTransportImmediate(
    const MoveNode& node,
    ShortImmPSocketResource& immRes) const {

    Move& move = const_cast<MoveNode&>(node).move();
    int bits = MachineConnectivityCheck::requiredImmediateWidth(
        immRes.signExtends(),
        static_cast<const TTAProgram::TerminalImmediate&>(move.source()),
        *mach_);
    if (bits <= immRes.immediateWidth()) {
        return true;
    } else {
        return false;
    }
}

/**
 * Return the short immediate p-socket resource related to given bus.
 *
 * @param busRes Bus resource.
 * @return The short immediate p-socket resource related to given bus.
 */
ShortImmPSocketResource&
BusBroker::findImmResource(BusResource& busRes) const {
    int i = 0;
    SchedulingResource* socketRes = NULL;
    ShortImmPSocketResource* immRes = NULL;
    while (i < busRes.relatedResourceCount(0)) {
        socketRes = &busRes.relatedResource(0, i);
        if (socketRes->isShortImmPSocketResource()) {
            immRes =
                dynamic_cast<ShortImmPSocketResource*>(socketRes);
            if (immRes == NULL) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "BusBroker has other then ShortImm resource registered!");
            }                
            break;
        }
        i++;
    }
    return *immRes;
}

/**
 * Tests if any of a buses known to BusBroker supports a guard
 * required by MoveNode.
 *
 * @param node MoveNode to test.
 * @return true if some of the buses supports guard of node.
 */
bool
BusBroker::hasGuard(const MoveNode& node) const {
    Move& move = const_cast<MoveNode&>(node).move();
    const Guard& guard = move.guard().guard();
    ResourceMap::const_iterator resIter = resMap_.begin();
    while (resIter != resMap_.end()) {
        SchedulingResource& busResource = *(*resIter).second;
        const Bus* aBus =
            static_cast<const Bus*>(&machinePartOf(busResource));

        for (int j = 0; j < aBus->guardCount(); j++) {
            Guard* busGuard = aBus->guard(j);
            if (busGuard->isEqual(guard)) {
                return true;
            }
        }
        resIter++;
    }
    return false;
}

void
BusBroker::clear() {
    ResourceBroker::clear();
    for (std::list<SchedulingResource*>::iterator i = 
             shortImmPSocketResources_.begin(); 
         i != shortImmPSocketResources_.end(); i++) {
        (*i)->clear();
    }
    busPreassigned_.clear();
}
         
