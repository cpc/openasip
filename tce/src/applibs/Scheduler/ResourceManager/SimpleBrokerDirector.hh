/**
 * @file SimpleBrokerDirector.hh
 *
 * Declaration of SimpleBrokerDirector class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_BROKER_DIRECTOR_HH
#define TTA_SIMPLE_BROKER_DIRECTOR_HH

#include <vector>
#include <map>
#include "BrokerDirector.hh"
#include "Exception.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "Bus.hh"
#include "Immediate.hh"

namespace TTAProgram {
    class Instruction;
}

class ResourceBroker;
class IUBroker;
class ITemplateBroker;
class BusBroker;
class ExecutionPipelineBroker;

/**
 * A simple broker director.
 */
class SimpleBrokerDirector : public BrokerDirector {
public:
    SimpleBrokerDirector(
        const TTAMachine::Machine& machine,
        AssignmentPlan& plan);
    virtual ~SimpleBrokerDirector();

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
    struct OriginalResources {
        OriginalResources(
            TTAProgram::Terminal*,
            TTAProgram::Terminal*,
            TTAMachine::Bus*,
            TTAProgram::MoveGuard*,
            bool);
        ~OriginalResources();

        TTAProgram::Terminal* src_;
        TTAProgram::Terminal* dst_;
        TTAMachine::Bus*    bus_;
        TTAProgram::MoveGuard* guard_;
        bool isGuarded_;
    };
    IUBroker& immediateUnitBroker() const
        throw (InstanceNotFound);
    ITemplateBroker& instructionTemplateBroker() const
        throw (InstanceNotFound);
    BusBroker& busBroker() const
        throw (InstanceNotFound);
    ExecutionPipelineBroker& executionPipelineBroker() const
        throw (InstanceNotFound);

    std::map<const MoveNode*, OriginalResources*> origResMap_;
    int knownMaxCycle_;
};

#endif
