/**
 * @file ResourceBroker.hh
 *
 * Declaration of ResourceBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_BROKER_HH
#define TTA_RESOURCE_BROKER_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "SchedulingResource.hh"
#include "MachinePart.hh"

#define TEST_BROKERS 0

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

class MoveNode;
class ResourceMapper;

/**
 * Base interface for resource brokers.
 */
class ResourceBroker {
public:
    ResourceBroker(std::string);
    virtual ~ResourceBroker();

    virtual bool isAnyResourceAvailable(int cycle, const MoveNode& node)
        const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node) const throw (InstanceNotFound);
    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node) const;
    virtual int earliestCycle(int cycle, const MoveNode& node) const = 0;
//     virtual ResourceCyclePairList earliestCyclesOfAllResources(
//         int cycle, const MoveNode& node) const = 0;
    virtual int latestCycle(int cycle, const MoveNode& node) const = 0;
//     virtual ResourceCyclePairList latestCyclesOfAllResources(
//         int cycle, const MoveNode& node) const = 0;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node)
        const = 0;
    virtual bool isApplicable(const MoveNode& node) const = 0;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception) = 0;
    virtual void unassign(MoveNode& node) = 0;

    virtual void buildResources(const TTAMachine::Machine& target) = 0;
    virtual void setupResourceLinks(const ResourceMapper& mapper) = 0;
    virtual SchedulingResource& resourceOf(const TTAMachine::MachinePart& mp)
        const throw (WrongSubclass, KeyNotFound);
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
protected:
    typedef std::map<
        const TTAMachine::MachinePart*, SchedulingResource*> ResourceMap;
    // A container to store MoveNode - Scheduling Resource pairs
    typedef std::map<const MoveNode*, SchedulingResource*> MoveResMap;
    void setResourceMapper(const ResourceMapper& mapper);
    const ResourceMapper& resourceMapper() const;

    void addResource(
        const TTAMachine::MachinePart& mp,
        SchedulingResource* res);

    ResourceMap resMap_;
    const ResourceMapper* resourceMapper_;
    MoveResMap assignedResources_;
    std::string brokerName_;
};

#endif
