/**
 * @file IUBroker.hh
 *
 * Declaration of IUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IU_BROKER_HH
#define TTA_IU_BROKER_HH

#include "ResourceBroker.hh"
#include "SimpleResourceManager.hh"
#include "IUResource.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

namespace TTAProgram {
    class Terminal;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Immediate unit broker.
 */
class IUBroker : public ResourceBroker {
public:
    IUBroker(std::string);
    IUBroker(std::string, SimpleResourceManager*);
    virtual ~IUBroker();

    bool isAnyResourceAvailable(int useCycle, const MoveNode& node) const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node) const
        throw (InstanceNotFound);
    virtual SchedulingResourceSet allAvailableResources(
        int,
        const MoveNode&) const;
    virtual void assign(int, MoveNode&, SchedulingResource&)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isIUBroker() const;
    virtual TTAProgram::Terminal* immediateValue(const MoveNode& node) const;
    virtual int immediateWriteCycle(const MoveNode& node) const;
    

private:
    /// Target machine.
    const TTAMachine::Machine* target_;
    /// Functor for sorting result of allAvailable by register width.
    struct less_width {
        bool operator()(IUResource* x, IUResource* y) {
            return x->width() < y->width();
        }
    };
    // Pointer to resource manager, needed to get information about
    // immediate from IUBroker
    SimpleResourceManager* rm_;    
};

#endif
