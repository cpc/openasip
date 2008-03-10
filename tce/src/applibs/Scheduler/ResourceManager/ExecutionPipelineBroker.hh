/**
 * @file ExecutionPipelineBroker.hh
 *
 * Declaration of ExecutionPipelineBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTION_PIPELINE_BROKER_HH
#define TTA_EXECUTION_PIPELINE_BROKER_HH

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
    class FunctionUnit;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Execution pipeline broker.
 */
class ExecutionPipelineBroker : public ResourceBroker {
public:
    ExecutionPipelineBroker(std::string);
    virtual ~ExecutionPipelineBroker();

    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);
    virtual int highestKnownCycle() const;
    virtual bool isExecutionPipelineBroker() const;

private:
    /// Map to link FU's and pipelines.
    typedef std::map<SchedulingResource*, const TTAMachine::FunctionUnit*>
        FUPipelineMap;
    int latestFromSource(int, const MoveNode&) const;
    int latestFromDestination(int, const MoveNode&) const;
    int earliestFromSource(int, const MoveNode&) const;
    int earliestFromDestination(int, const MoveNode&) const;

    /// FU's and their corresponding pipeline resources.
    FUPipelineMap fuPipelineMap_;
};

#endif
