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
ResourceBroker::ResourceBroker(std::string name,
        unsigned int initiationInterval): 
    initiationInterval_(initiationInterval),
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
ResourceBroker::isAnyResourceAvailable(int cycle, const MoveNode& node,
                                       const TTAMachine::Bus*,
                                       const TTAMachine::FunctionUnit*,
                                       const TTAMachine::FunctionUnit*, int,
                                       const TTAMachine::ImmediateUnit*,
                                       int) const {

    for (ResourceMap::const_iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        // IUBroker overrides this so no need for immu check
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
ResourceBroker::availableResource(int cycle, const MoveNode& node,
                                  const TTAMachine::Bus*,
                                  const TTAMachine::FunctionUnit*,
                                  const TTAMachine::FunctionUnit*, int,
                                  const TTAMachine::ImmediateUnit*, int)
    const {

    for (ResourceMap::const_iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        SchedulingResource* res = (*resIter).second;
        if (res->isAvailable(cycle)
	    && res->canAssign(cycle, node)) {
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
ResourceBroker::allAvailableResources(int, const MoveNode&,
                                      const TTAMachine::Bus*,
                                      const TTAMachine::FunctionUnit*,
                                      const TTAMachine::FunctionUnit*,
                                      int,
                                      const TTAMachine::ImmediateUnit*,
                                      int) const {
    SchedulingResourceSet resourceSet;
    return resourceSet;
}

/**
 * Tells whether the given resource is available for given node at
 * given cycle.
 */
bool ResourceBroker::isAvailable(
    SchedulingResource& res, const MoveNode& node, int cycle,
    const TTAMachine::Bus* bus,
    const TTAMachine::FunctionUnit* srcFU,
    const TTAMachine::FunctionUnit* dstFU,
    int immWriteCycle,
    const TTAMachine::ImmediateUnit* immu,
    int immRegIndex) const {
    SchedulingResourceSet resources =
      allAvailableResources(cycle, node, bus, srcFU, dstFU,
			    immWriteCycle, immu, immRegIndex);
    return resources.hasResource(res);
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
 * Return the instruction index corresponding to cycle.
 *
 * If modulo scheduling is not used (ie. initiation interval is 0), then
 * index is equal to cycle.
 *
 * @param cycle Cycle to get instruction index.
 * @return Return the instruction index for cycle.
 */
unsigned int
ResourceBroker::instructionIndex(unsigned int cycle) const {

    if (initiationInterval_ != 0) {
        return cycle % initiationInterval_;
    } else {
        return cycle;
    }
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

/**
 * Get resources
 *
 * @param contents Set to which resources are copied to.
 */
void
ResourceBroker::resources(std::set<SchedulingResource*>& contents) {
    for (ResourceMap::iterator i = resMap_.begin(); i != resMap_.end(); ++i ) {
        contents.insert(i->second);
    }
}


/**
 * Set initiation interval, if ii = 0 then initiation interval is not used.
 *
 * @param ii initiation interval
 */
void
ResourceBroker::setInitiationInterval(unsigned int ii)
{
    initiationInterval_ = ii;
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

/**
 * Clears all resources managed by the broker so that the broker
 * can be reused.
 */
void
ResourceBroker::clear() {
    // Call clear for all resources.
    for (ResourceMap::iterator i = resMap_.begin(); i != resMap_.end(); i++) {
        i->second->clear();
    }
    assignedResources_.clear();
}
