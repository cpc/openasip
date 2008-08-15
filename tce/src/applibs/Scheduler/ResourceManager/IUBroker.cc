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
 * @file IUBroker.cc
 *
 * Implementation of IUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
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
IUBroker::IUBroker(std::string name, SimpleResourceManager* rm) : 
    ResourceBroker(name), 
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
IUBroker::isAnyResourceAvailable(int useCycle,const MoveNode& node) const {
    int resultCount = allAvailableResources(useCycle, node).count();
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
IUBroker::availableResource(int useCycle, const MoveNode& node)
    const throw (InstanceNotFound){
    try {
        return allAvailableResources(useCycle, node).resource(0);
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
IUBroker::allAvailableResources(int useCycle, const MoveNode& node) const {
     
    int defCycle = (useCycle > 0) ? useCycle : 1;
    
    SchedulingResourceSet results;
    std::vector<IUResource*> tmpResult;
    
    while (defCycle > 0 && (useCycle - defCycle) < MAX_LIMM_DISTANCE &&
           tmpResult.empty()) {
        defCycle--;
        ResourceMap::const_iterator resIter = resMap_.begin();
        while (resIter != resMap_.end()) {
            IUResource* iuRes = dynamic_cast<IUResource*>((*resIter).second);
            if (iuRes->canAssign(defCycle, useCycle, node)) {                   
                TerminalImmediate* tempImm = 
                    dynamic_cast<TerminalImmediate*>(
                        node.move().source().copy());               
                const ImmediateUnit& iu =                    
                    dynamic_cast<const ImmediateUnit&>(machinePartOf(*iuRes));
                RFPort* port = iu.port(0);         
                TerminalRegister* newSrc = new TerminalRegister(*port, 0);
                Immediate* imm = new Immediate(tempImm, newSrc);
                if (rm_->isTemplateAvailable(defCycle, imm)) {
                    tmpResult.push_back(iuRes);                    
                } 
                delete imm;                
            }
            resIter++;
        }
        sort(tmpResult.begin(), tmpResult.end(), less_width());
        std::vector<IUResource*>::iterator tmpItr = tmpResult.begin();
        while (tmpItr != tmpResult.end()) {
            results.insert(*(*tmpItr));
            tmpItr++;
        }
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
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
IUBroker::assign(int useCycle, MoveNode& node, SchedulingResource& res)
    throw (Exception) {

    int defCycle = (useCycle > 0) ? useCycle : 1;
    
    IUResource& iuRes = dynamic_cast<IUResource&>(res);
    Move& move = const_cast<MoveNode&>(node).move();

    if (hasResource(res)) {
        ImmediateUnit& iu =
            const_cast<ImmediateUnit&>(
                dynamic_cast<const ImmediateUnit&>(machinePartOf(res)));
        // IURes assign method will set index to the register
        // in IU that was assigned for immediate read
        int index = 0;
        while (defCycle > 0 && (useCycle - defCycle) < MAX_LIMM_DISTANCE) {
            defCycle--;
            if (iuRes.canAssign(defCycle, useCycle, node)) {
                TerminalImmediate* tempImm = 
                    dynamic_cast<TerminalImmediate*>(
                        node.move().source().copy());               
                const ImmediateUnit& iu =                    
                    dynamic_cast<const ImmediateUnit&>(machinePartOf(iuRes));
                RFPort* port = iu.port(0);         
                TerminalRegister* newSrc = new TerminalRegister(*port, 0);
                Immediate* imm = new Immediate(tempImm, newSrc);
                if (rm_->isTemplateAvailable(defCycle, imm)) {
                    delete imm;
                    break;                    
                } else {
                    delete imm;
                }
            }
        }
        iuRes.assign(defCycle, useCycle, node, index);

        // temporary, source() has to know some port of IU to
        // be able to identify which IU Move uses.
        // OutputPSocket broker will assign a free port later
        RFPort* port = iu.port(0);
        assignedResources_.insert(
            std::pair<const MoveNode*, SchedulingResource*>(&node, &iuRes));
        TerminalRegister* newSrc = new TerminalRegister(*port, index);
        move.setSource(newSrc);
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
        abort();
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
IUBroker::earliestCycle(int, const MoveNode&) const {
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
IUBroker::latestCycle(int, const MoveNode&) const {
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
IUBroker::isAlreadyAssigned(int, const MoveNode& node) const {
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
IUBroker::isApplicable(const MoveNode& node) const {     
    if (node.isSourceImmediateRegister()) {
        return true;        
    }
    // If node is annotated, it needs to be converted to LIMM,
    // detected in BB scheduler
    if (node.isSourceConstant() && node.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        return true;
    }    
    if (node.isSourceConstant() && !rm_->canTransportImmediate(node)) {
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
                iu->name(), iu->numberOfRegisters(), iu->width(),
                iu->latency(), extension);
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
TTAProgram::Terminal*
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
