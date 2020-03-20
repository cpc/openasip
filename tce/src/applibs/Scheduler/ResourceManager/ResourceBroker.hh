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
 * @file ResourceBroker.hh
 *
 * Declaration of ResourceBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_BROKER_HH
#define TTA_RESOURCE_BROKER_HH

#include <map>
#include <set>
#include <string>

#include "Exception.hh"
#include "SchedulingResource.hh"
#include "MachinePart.hh"
#include "MoveNode.hh"

#define TEST_BROKERS 0

namespace TTAMachine {
    class Machine;
    class MachinePart;
    class Bus;
    class FunctionUnit;
    class ImmediateUnit;
}


class ResourceMapper;

/**
 * Base interface for resource brokers.
 */
class ResourceBroker {
public:
    ResourceBroker(std::string, unsigned int initiationInterval = 0);
    virtual ~ResourceBroker();

    typedef std::set<SchedulingResource*> ResourceSet;
    virtual bool isAnyResourceAvailable(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex)
      const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
	const TTAMachine::ImmediateUnit* immu,
	int immRegIndex) const;
    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
	const TTAMachine::ImmediateUnit* immu,
	int immRegIndex) const;
    virtual bool isAvailable(
        SchedulingResource& des, const MoveNode& node, int cycle,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
	const TTAMachine::ImmediateUnit* immu,
	int immRegIndex) const;
    virtual int earliestCycle(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const = 0;
    virtual int latestCycle(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const = 0;
    virtual bool isAlreadyAssigned(
        int cycle, const MoveNode& node,const TTAMachine::Bus* preassignedBus)
        const = 0;
    virtual bool isApplicable(
        const MoveNode& node, const TTAMachine::Bus* preassignedBus) const = 0;
    virtual void assign(
        int cycle, MoveNode& node, SchedulingResource& res, int immWriteCycle,
	int immRegIndex) = 0;

    virtual void unassign(MoveNode& node) = 0;

    virtual void buildResources(const TTAMachine::Machine& target) = 0;
    virtual void setupResourceLinks(const ResourceMapper& mapper) = 0;
    inline SchedulingResource* resourceOf(
        const TTAMachine::MachinePart& mp) const;
    virtual const TTAMachine::MachinePart& machinePartOf(
        const SchedulingResource& r)
        const throw (WrongSubclass, KeyNotFound);
    bool hasResourceOf(const TTAMachine::MachinePart& mp) const;
    bool hasResource(const SchedulingResource& r) const;
    int resourceCount() const;

    virtual bool isBusBroker() const;
    virtual bool isITemplateBroker() const;
    virtual bool isIUBroker() const;
    virtual bool isExecutionPipelineBroker() const;
    void validateResources() const;
    virtual std::string brokerName() const;
    void resources(ResourceSet& contents);
    virtual void setInitiationInterval(unsigned int cycles);
    virtual void clear();
    virtual void setMaxCycle(unsigned int) {};
protected:
    typedef std::map<
    const TTAMachine::MachinePart*, SchedulingResource*,
    TTAMachine::MachinePart::Comparator> ResourceMap;
    // A container to store MoveNode - Scheduling Resource pairs
    typedef std::map<const MoveNode*, SchedulingResource*, 
                     MoveNode::Comparator> MoveResMap;
    void setResourceMapper(const ResourceMapper& mapper);
    const ResourceMapper& resourceMapper() const;

    unsigned int initiationInterval_;
    unsigned int instructionIndex(unsigned int) const;
    
    void addResource(
        const TTAMachine::MachinePart& mp,
        SchedulingResource* res);

    ResourceMap resMap_;
    const ResourceMapper* resourceMapper_;
    MoveResMap assignedResources_;
    std::string brokerName_;
};

#include "ResourceBroker.icc"

#endif
