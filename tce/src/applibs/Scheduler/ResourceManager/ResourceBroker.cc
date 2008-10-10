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
 * @file ResourceBroker.cc
 *
 * Implementation of ResourceBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "ResourceBroker.hh"
#include "SchedulingResource.hh"
#include "MapTools.hh"
#include "MachinePart.hh"

#include "ShortImmPSocketResource.hh"

using std::pair;
using std::string;
using namespace TTAMachine;

/**
 * Constructor.
 */
ResourceBroker::ResourceBroker(std::string name): 
    resourceMapper_(NULL),
    brokerName_(name){
}

/**
 * Destructor.
 */
ResourceBroker::~ResourceBroker() {
    MapTools::deleteAllValues(resMap_);
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
ResourceBroker::isAnyResourceAvailable(int cycle, const MoveNode& node)
    const {

    for (ResourceMap::const_iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const SchedulingResource* res = (*resIter).second;
        if (res->isAvailable(cycle) && res->canAssign(cycle, node)) {
            return true;
        }
    }

    return false;
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
 * @return One (any) resource managed by this broker that can be
 * assigned to the given node in the given cycle.
 * @exception InstanceNotFound If no available resource is found.
 */
SchedulingResource&
ResourceBroker::availableResource(int cycle, const MoveNode& node) const
    throw (InstanceNotFound) {

    for (ResourceMap::const_iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        SchedulingResource* res = (*resIter).second;
        if (res->isAvailable(cycle) && res->canAssign(cycle, node)) {
            return *res;
        }
    }

    string msg = "No available resource found.";
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 * @param cycle Cycle.
 * @param node Node.
 * @note This default implementation always returns an empty set.
 */
SchedulingResourceSet
ResourceBroker::allAvailableResources(int, const MoveNode&) const {
    SchedulingResourceSet resourceSet;
    return resourceSet;
}

/**
 * Return the resource instance that models the given machine part.
 *
 * @param mp Machine part.
 * @return The resource instance that models the given machine part.
 * @exception KeyNotFound If no corresponding resource is found or mp
 * is not one of the machine parts modelled by the primary resources
 * of this broker.
 */
SchedulingResource&
ResourceBroker::resourceOf(const TTAMachine::MachinePart& mp) const
    throw (WrongSubclass, KeyNotFound) {
    SchedulingResource* resource = NULL;
    if (!MapTools::containsKey(resMap_, &mp)) {
        abortWithError("ResourceOf(machine part) failed");
    }
    try {
        resource = MapTools::valueForKey<SchedulingResource*>(resMap_, &mp);
    } catch (const Exception& e) {
        const Component& component = dynamic_cast<const Component&>(mp);
        string msg = "Resource for component " + component.name() +
            " not found.";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return *resource;
}

/**
 * Return the machine part that models the given resource.
 *
 * @param r Scheduling resource.
 * @return The machine part that models the given resource.
 * @exception KeyNotFound If no corresponding machine part is found or
 * r is not one of the primary resources of this broker.
 */
const TTAMachine::MachinePart&
ResourceBroker::machinePartOf(const SchedulingResource& r) const
    throw (WrongSubclass, KeyNotFound) {
    const TTAMachine::MachinePart* machinePart = NULL;
    try {
        machinePart = MapTools::keyForValue<const MachinePart*>(resMap_, &r);
    } catch (const Exception& e) {
        string msg = "Machine part of for resource ";
        msg += r.name();
        msg += " not found!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return *machinePart;
}

/**
 * Return true if this broker holds a resource instance that models
 * given machine part.
 *
 * @param mp Machine part.
 * @return True if this broker holds a resource instance that models
 * given machine part.
 */
bool
ResourceBroker::hasResourceOf(const TTAMachine::MachinePart& mp) const {
    return MapTools::containsKey(resMap_, &mp);
}

/**
 * Return true if this broker holds given resource instance.
 *
 * @param r Scheduling resource.
 * @return True if this broker holds given resource instance.
 */
bool
ResourceBroker::hasResource(const SchedulingResource& r) const {
    return MapTools::containsValue(resMap_, &r);
}

/**
 * Set the resource mapper for this broker.
 *
 * @param mapper Resource mapper.
 */
void
ResourceBroker::setResourceMapper(const ResourceMapper& mapper) {
    resourceMapper_ = &mapper;
}

/**
 * Return the resource mapper set for this broker.
 *
 * @return The resource mapper set for this broker.
 */
const ResourceMapper&
ResourceBroker::resourceMapper() const {
    return *resourceMapper_;
}

/**
 * Add resource - machine part pair to the broker.
 *
 * @param mp Machine part.
 * @param res Corresponding resource.
 */
void
ResourceBroker::addResource(
    const TTAMachine::MachinePart& mp,
    SchedulingResource* res) {
    
    resMap_.insert(
        pair<const TTAMachine::MachinePart*, SchedulingResource*>(&mp, res));
}

int
ResourceBroker::resourceCount() const {
    return resMap_.size();
}

bool
ResourceBroker::isBusBroker() const {
    return false;
}

bool
ResourceBroker::isITemplateBroker() const {
    return false;
}

bool
ResourceBroker::isIUBroker() const {
    return false;
}

bool
ResourceBroker::isExecutionPipelineBroker() const {
    return false;
}

/**
 * Tests if is each resource has correct related and dependent groups.
 * 
 * @throw ModuleRunTimeError in case resource does not have correct related
 *      or dependent resources.
 */
void
ResourceBroker::validateResources() const {
    ResourceMap::const_iterator itr = resMap_.begin();
    while (itr != resMap_.end()) {
        if (!(*itr).second->validateDependentGroups()) {
            std::string msg = (boost::format(
                "Broker '%s' failed to validate DependentGroup for '%s'!") % 
                brokerName() % (*itr).second->name()).str();                 
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }
        if (!(*itr).second->validateRelatedGroups()){
            std::string msg = (boost::format(
                "Broker '%s' failed to validate RelatedGroup for '%s'!") % 
                brokerName() % (*itr).second->name()).str();                 
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);            
        }
        ++itr;
    }
    return;
}

/**
 * Returns the name of particular broker.
 * Used for debugging mainly.
 * 
 * @return Name of a broker.
 */
std::string
ResourceBroker::brokerName() const {
    return brokerName_;
}
