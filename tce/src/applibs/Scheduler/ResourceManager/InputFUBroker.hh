/**
 * @file InputFUBroker.hh
 *
 * Declaration of InputFUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_FU_BROKER_HH
#define TTA_INPUT_FU_BROKER_HH

#include <map>

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

namespace TTAProgram {
    class Move;
    class TerminalFUPort;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Input function unit broker.
 */
class InputFUBroker : public ResourceBroker {
public:
    InputFUBroker(std::string);
    virtual ~InputFUBroker();

    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node) const;

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);
private:
    /// A container type for storing the old terminals for easy restoration
    /// in unassign().
    typedef std::map<
        const TTAProgram::Move*,
        TTAProgram::TerminalFUPort*> RestoreMap;
    /// A map for old terminals for easy unassign.
    RestoreMap oldTerminalMap_;    
};

#endif
