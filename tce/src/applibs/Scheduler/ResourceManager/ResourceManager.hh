/**
 * @file ResourceManager.hh
 *
 * Declaration of ResourceManager class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_MANAGER_HH
#define TTA_RESOURCE_MANAGER_HH

namespace TTAMachine {
    class Machine;
}

class MoveNode;
class MoveNodeSet;

/**
 * Base interface for resource managers.
 */
class ResourceManager {
public:
    ResourceManager(const TTAMachine::Machine& machine);
    virtual ~ResourceManager();

    virtual bool canAssign(int cycle, MoveNode& node) const = 0;
    virtual void assign(int cycle, MoveNode& node) = 0;
    virtual void unassign(MoveNode& node) = 0;
    virtual int earliestCycle(MoveNode& node) const = 0;
    virtual int earliestCycle(int cycle, MoveNode& node) const = 0;
    virtual int latestCycle(MoveNode& node) const = 0;
    virtual int latestCycle(int cycle, MoveNode& node) const = 0;
    virtual bool hasConnection(MoveNodeSet& nodes) = 0;
    virtual bool supportsExternalAssignments() const = 0;
    virtual int largestCycle() const = 0;
    const TTAMachine::Machine& machine() const;

private:
    /// Target machine.
    const TTAMachine::Machine* machine_;
};

#endif
