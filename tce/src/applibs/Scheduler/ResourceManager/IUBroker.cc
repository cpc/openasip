/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file IUBroker.cc
 *
 * Implementation of IUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <algorithm>

#include "IUBroker.hh"
#include "IUResource.hh"
#include "Machine.hh"
#include "Move.hh"
#include "MapTools.hh"
#include "ResourceMapper.hh"
#include "OutputPSocketResource.hh"
#include "MoveNode.hh"
#include "TCEString.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "Immediate.hh"
#include "SimpleResourceManager.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

// limit distance between LIMM write and use.
// this limit makes scheduling much faster, and cases
// where there is some advantage in limm read and write being
// far away are extremely rare
static const int MAX_LIMM_DISTANCE = 25;

/**
 * Constructor.
 */
IUBroker::IUBroker(std::string name):
    ResourceBroker(name),
    target_(NULL),
    rm_(NULL) {
}

/**
 * Constructor.
 */
IUBroker::IUBroker(std::string name, SimpleResourceManager* rm,
        unsigned int initiationInterval) :
    ResourceBroker(name, initiationInterval),
    target_(NULL), rm_(rm) {
}

/**
 * Destructor.
 */
IUBroker::~IUBroker(){
}

/**
 * Return true if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 *
 * @param defCycle Cycle in which the write to long immediate register is
 * @param useCycle Cycle in which the long immediate register is read by node
 * @param node Node.
 * @return True if one of the resources managed by this broker is
 * suitable for the request contained in the node and can be assigned
 * to it in given cycle.
 */
bool
IUBroker::isAnyResourceAvailable(int useCycle,const MoveNode& node,
                                 const TTAMachine::Bus* bus,
                                 const TTAMachine::FunctionUnit* srcFU,
                                 const TTAMachine::FunctionUnit* dstFU,
                                 int immWriteCycle,
                                 const TTAMachine::ImmediateUnit* immu,
                                 int immRegIndex) const {
    int resultCount = allAvailableResources(
        useCycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex).
	count();
    return (resultCount > 0);
}

/**
 * Return one (any) resource managed by this broker that has a
 * register available for assignment between given definition
 * and use cycles.
 *
 * @param useCycle Use cycle.
 * @param node Node.
 * @return One (any) resource managed by this broker that has a
 * register available for assignment between given definition and
 * use cycles.
 */
SchedulingResource&
IUBroker::availableResource(int useCycle, const MoveNode& node,
                            const TTAMachine::Bus* bus,
                            const TTAMachine::FunctionUnit* srcFU,
                            const TTAMachine::FunctionUnit* dstFU,
                            int immWriteCycle,
                            const TTAMachine::ImmediateUnit* immu,
                            int immRegIndex)
    const {
    try {
        return allAvailableResources(
            useCycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex).
	    resource(0);
    } catch (const KeyNotFound& e) {
        std::string message = "No immediate register resource available.";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, message);
    }
}
/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @param useCycle Read from immediate register by Node
 * @param node Node.
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 */
SchedulingResourceSet
IUBroker::allAvailableResources(int useCycle, const MoveNode& node,
                                const TTAMachine::Bus*,
                                const TTAMachine::FunctionUnit*,
                                const TTAMachine::FunctionUnit*,
                                int immWriteCycle,
                                const TTAMachine::ImmediateUnit* immu,
                                int immRegIndex) const {

    int defCycle = useCycle;
    if (immWriteCycle != -1) {
        defCycle = immWriteCycle;
    }

    SchedulingResourceSet results;

    // pre-split LIMM
//    assert (!node.move().source().isImmediateRegister());

    std::vector<IUResource*> tmpResult;

    // RM breaks if LIMM write and use are in same instruction.
    int maxLimmDistance = MAX_LIMM_DISTANCE;
    if (initiationInterval_) {
        maxLimmDistance = std::min(maxLimmDistance, (int)initiationInterval_);
    }
    while (defCycle >= 0 && (useCycle - defCycle) < maxLimmDistance &&
           tmpResult.empty()) {
        ResourceMap::const_iterator resIter = resMap_.begin();
        while (resIter != resMap_.end()) {
            // TODO: why is this dynamic, not static cast?
            IUResource* iuRes = dynamic_cast<IUResource*>((*resIter).second);
            if (immu == NULL || resourceOf(*immu) == (*resIter).second) {
                if (iuRes->canAssign(defCycle, useCycle, node, immRegIndex)) {
                    TerminalImmediate* tempImm =
                        dynamic_cast<TerminalImmediate*>(
                            node.move().source().copy());
                    const ImmediateUnit& iu =
                        dynamic_cast<const ImmediateUnit&>(
                            machinePartOf(*iuRes));
                    RFPort* port = iu.port(0);
                    TerminalRegister* newSrc = new TerminalRegister(*port, 0);
                    auto imm = std::make_shared<Immediate>(tempImm, newSrc);
                    if (rm_->isTemplateAvailable(defCycle, imm)) {
                        tmpResult.push_back(iuRes);
                    }
                }
            }
            resIter++;
        }
        sort(tmpResult.begin(), tmpResult.end(), less_width());
        std::vector<IUResource*>::iterator tmpItr = tmpResult.begin();
        while (tmpItr != tmpResult.end()) {
            results.insert(*(*tmpItr));
            tmpItr++;
        }
        if (immWriteCycle != -1) {
            break;
        }
        defCycle--;
    }
    return results;
}
/**
 * Mark given resource as in use for the given node, and assign the
 * corresponding machine part (if applicable) to the node's move.
 *
 * @param useCycle Cycle in which immediate register is read by Node
 * @param node MoveNode that reads a register
 * @param res Long immediate register file resource
 * @param immWriteCycle forced def cycle for the immediate. Not forced if -1
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
IUBroker::assign(
    int useCycle, MoveNode& node, SchedulingResource& res, int immWriteCycle,
    int immRegIndex) {

    int defCycle = useCycle;
    if (immWriteCycle != -1) {
        defCycle = immWriteCycle;
    }

    IUResource& iuRes = dynamic_cast<IUResource&>(res);
    Move& move = const_cast<MoveNode&>(node).move();

    if (hasResource(res)) {
        ImmediateUnit& iu =
            const_cast<ImmediateUnit&>(
                dynamic_cast<const ImmediateUnit&>(machinePartOf(res)));

        // pre-split LIMM
        if (node.move().source().isImmediateRegister()) {
            const ImmediateUnit& iuSrc =
                node.move().source().immediateUnit();
            assert(&iu == &iuSrc);
            IUResource* iuRes = static_cast<IUResource*>(resourceOf(iu));
            iuRes->assign(useCycle, node);
            assignedResources_.insert(
                std::pair<const MoveNode*, SchedulingResource*>(&node, iuRes));

        } else {
            int index = -1;
            // IURes assign method will set index to the register
            // in IU that was assigned for immediate read

            while (defCycle >= 0 && (useCycle - defCycle) < MAX_LIMM_DISTANCE) {
                if (iuRes.canAssign(defCycle, useCycle, node, immRegIndex)) {
                    TerminalImmediate* tempImm =
                        dynamic_cast<TerminalImmediate*>(
                            node.move().source().copy());
                    const ImmediateUnit& iu =
                        dynamic_cast<const ImmediateUnit&>(machinePartOf(iuRes));
                    RFPort* port = iu.port(0);
                    TerminalRegister* newSrc = new TerminalRegister(*port, 0);
                    auto imm = std::make_shared<Immediate>(tempImm, newSrc);
                    if (rm_->isTemplateAvailable(defCycle, imm)) {
                        break;
                    }
                }
                if (immWriteCycle != -1) {
                    std::cerr << "Use cycle: " << useCycle << " imm cycle: "
                              << immWriteCycle << " node: " << node.toString()
                              << std::endl;
                    assert(NULL && "Can't assign forced imm write at cycle");
                }
                defCycle--;
            }
	    // is index forced?
	    if (immRegIndex != -1)
		index = immRegIndex;
            iuRes.assign(defCycle, useCycle, node, index);


            // temporary, source() has to know some port of IU to
            // be able to identify which IU Move uses.
            // OutputPSocket broker will assign a free port later
            RFPort* port = iu.port(0);
            assignedResources_.insert(
                std::pair<const MoveNode*, SchedulingResource*>(&node, &iuRes));
            TerminalRegister* newSrc = new TerminalRegister(*port, index);
            move.setSource(newSrc);
        }
    } else {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Free the Immediate register and set MoveNode source back to
 * TerminalImmediate constant.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does
 * nothing.
 *
 * @param node Node.
 * @note The ImmediateResource keeps a copy of original source terminal and
 * will restore it during unassignment.
 */
void
IUBroker::unassign(MoveNode& node) {
    if (!MapTools::containsKey(assignedResources_, &node)) {
        TCEString msg = "Trying to unassign Long Immediate register ";
        msg += "that was not assigned previously: " + node.toString();
        abortWithError(msg);
        return;
    }
    IUResource* iuRes = dynamic_cast<IUResource*>(
        MapTools::valueForKey<SchedulingResource*>(
            assignedResources_, &node));
    iuRes->unassign(node.cycle(), node);
    assignedResources_.erase(&node);
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
IUBroker::earliestCycle(
    int, const MoveNode&,
    const TTAMachine::Bus*,
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
IUBroker::latestCycle(int, const MoveNode&,
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
IUBroker::isAlreadyAssigned(
    int, const MoveNode& node, const TTAMachine::Bus*) const {
    if (!MapTools::containsKey(assignedResources_, &node)) {
        return false;
    }
    try {
        // Somehow nasty solution, but in correct case should
        // never be throwing exception
        IUResource* iu = dynamic_cast<IUResource*>(
            MapTools::valueForKey<SchedulingResource*>(
            assignedResources_, &node));
        iu->immediateValue(node);
    } catch (const KeyNotFound&) {
        return false;
    }
    return true;
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * @param node Node.
 * @return True if the given node is Immediate read.
 * @note this method is called to determine if node needs IU resource
 * as well as by resetAssignments to remove the IU assignment
 */
bool
IUBroker::isApplicable(
    const MoveNode& node, const TTAMachine::Bus* preassignedBus) const {
    if (!node.isMove()) {
        return false;
    }
    if (node.isSourceImmediateRegister()) {
        return true;
    }
    // If node is annotated, it needs to be converted to LIMM,
    // detected in BB scheduler
    if (node.isSourceConstant() && node.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        return true;
    }
    if (node.isSourceConstant() &&
        !rm_->canTransportImmediate(node, preassignedBus)) {
        return true;
    }
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
IUBroker::buildResources(const TTAMachine::Machine& target) {

    target_ = &target;
    Machine::ImmediateUnitNavigator navi = target.immediateUnitNavigator();

    for (int i = 0; i < navi.count(); i++) {
        ImmediateUnit* iu = navi.item(i);
        bool extension = false;
        if (iu->extensionMode() == Machine::ZERO) {
            extension = false;
        } else {
            extension = true;
        }
        IUResource* iuResource =
            new IUResource(
                target,
                iu->name(), iu->numberOfRegisters(), iu->width(),
                iu->latency(), extension, initiationInterval_);
        ResourceBroker::addResource(*iu, iuResource);
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
IUBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const ImmediateUnit* iu =
            dynamic_cast<const ImmediateUnit*>((*resIter).first);

        SchedulingResource* iuResource = (*resIter).second;
        for (int i = 0; i < iu->portCount(); i++) {
            Port* port = iu->port(i);
            if (port->outputSocket() != NULL) {
                try {
                    SchedulingResource& relatedRes =
                        mapper.resourceOf(*port->outputSocket());
                    iuResource->addToRelatedGroup(0, relatedRes);
                } catch (const KeyNotFound& e) {
                    std::string msg = "IUBroker: finding ";
                    msg += " resource for Socket ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }
            }
        }

        Machine::InstructionTemplateNavigator navi =
            target_->instructionTemplateNavigator();

        for (int i = 0; i < navi.count(); i++) {
            InstructionTemplate* itemplate = navi.item(i);
            if (itemplate->isOneOfDestinations(*iu)) {
                try {
                    SchedulingResource& relatedRes =
                        mapper.resourceOf(*itemplate);
                    iuResource->addToRelatedGroup(1, relatedRes);
                } catch (const KeyNotFound& e) {
                    std::string msg = "IUBroker: finding ";
                    msg += " resource for Template ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }
            }
        }
    }
    // todo: dependent register resources?
}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
IUBroker::isIUBroker() const {
    return true;
}

/**
 * Returns a original Immediate value for a node
 *
 * @param node Node that was changed to immediate register
 * read
 * @return Terminal representing original source of Move
 */
std::shared_ptr<TTAProgram::TerminalImmediate>
IUBroker::immediateValue(const MoveNode& node) const {
    if (!node.isSourceImmediateRegister()) {
        return NULL;
    }
    if (MapTools::containsKey(assignedResources_, &node)) {
        IUResource* iuRes = dynamic_cast<IUResource*>(
            MapTools::valueForKey<SchedulingResource*>(
                assignedResources_, &node));
        return iuRes->immediateValue(node);
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__,
        "MoveNode was not assigned Immediate resource.");
}

/**
 * Returns a cycle in which the original Immediate value is written
 * into immediate register.
 *
 * @param node Node that was changed to immediate register
 * read
 * @return Cycle in which immediate was written into register.
 */
int
IUBroker::immediateWriteCycle(const MoveNode& node) const {
    if (!node.isSourceImmediateRegister()) {
        return -1;
    }
    if (MapTools::containsKey(assignedResources_, &node)) {
        IUResource* iuRes = dynamic_cast<IUResource*>(
            MapTools::valueForKey<SchedulingResource*>(
                assignedResources_, &node));
        return iuRes->immediateWriteCycle(node);
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__,
        "MoveNode was not assigned Immediate resource.");
}

/**
 * Clears bookkeeping which is needed for unassigning previously assigned
 * moves. After this call these cannot be unassigned, but new moves which
 * are assigned after this call can still be unassigned.
 */
void 
IUBroker::clearOldResources() {
    for (ResourceMap::iterator i = resMap_.begin(); i != resMap_.end(); i++) {
        IUResource* iuRes = static_cast<IUResource*>(i->second);
        iuRes->clearOldResources();
    }
}
