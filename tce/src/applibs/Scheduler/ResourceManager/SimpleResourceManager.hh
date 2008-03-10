/**
 * @file SimpleResourceManager.hh
 *
 * Declaration of SimpleResourceManager class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_RESOURCE_MANAGER_HH
#define TTA_SIMPLE_RESOURCE_MANAGER_HH

#include "ResourceManager.hh"
#include "SimpleBrokerDirector.hh"
#include "AssignmentPlan.hh"
#include "ResourceBuildDirector.hh"

/**
 * A simple resource manager.
 */
class SimpleResourceManager : public ResourceManager {
public:
    SimpleResourceManager(const TTAMachine::Machine& machine);
    virtual ~SimpleResourceManager();

    virtual bool canAssign(int cycle, MoveNode& node) const;
    virtual bool canTransportImmediate(const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(MoveNode& node)
        throw (Exception);
    virtual int earliestCycle(MoveNode& node) const
        throw (Exception);
    virtual int earliestCycle(int cycle, MoveNode& node) const
        throw (Exception);
    virtual int latestCycle(MoveNode& node) const;
    virtual int latestCycle(int cycle, MoveNode& node) const;

    virtual bool hasConnection(MoveNodeSet& nodes);
    virtual bool hasGuard(const MoveNode& node) const;
    virtual TTAProgram::Instruction* instruction(int cycle) const;
    virtual bool supportsExternalAssignments() const;
    virtual int largestCycle() const;
    virtual void loseInstructionOwnership();
    virtual TTAProgram::Terminal* immediateValue(const MoveNode&);
    virtual int immediateWriteCycle(const MoveNode&) const;
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
private:
    /// Resource manager's broker director.
    SimpleBrokerDirector* director_;
    /// Resource assignment plan.
    AssignmentPlan plan_;
    /// Resource build director.
    ResourceBuildDirector buildDirector_;
};

#endif
