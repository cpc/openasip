/**
 * @file InputPSocketBroker.hh
 *
 * Declaration of InputPSocketBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_PSOCKET_BROKER_HH
#define TTA_INPUT_PSOCKET_BROKER_HH

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Input p-socket broker takes care of assigning input socket and input
 * port (of FU or RF) at once.
 */
class InputPSocketBroker : public ResourceBroker {
public:
    InputPSocketBroker(std::string name);
    virtual ~InputPSocketBroker();

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

};

#endif
