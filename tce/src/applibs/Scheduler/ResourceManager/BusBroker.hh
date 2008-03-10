/**
 * @file BusBroker.hh
 *
 * Declaration of BusBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_BROKER_HH
#define TTA_BUS_BROKER_HH

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

namespace TTAProgram {
    class TerminalImmediate;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;
class BusResource;
class ShortImmPSocketResource;

/**
 * Bus broker.
 */
class BusBroker : public ResourceBroker {
public:
    BusBroker(std::string name);
    virtual ~BusBroker();

    virtual bool isAnyResourceAvailable(int cycle, const MoveNode& node)
        const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node) const throw (InstanceNotFound);
    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isBusBroker() const;

    virtual bool canTransportImmediate(const MoveNode& node) const;
    virtual bool isInUse(int cycle, const MoveNode& node) const;
    virtual bool hasGuard(const MoveNode& node) const;

private:
    virtual bool canTransportImmediate(
        const MoveNode& node,
        ShortImmPSocketResource& immRes) const;
    virtual ShortImmPSocketResource& findImmResource(
        BusResource& busRes) const;
};

#endif
