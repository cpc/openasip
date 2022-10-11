/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ExecutionPipelineBroker.hh
 *
 * Declaration of ExecutionPipelineBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
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
class DataDependenceGraph;

/**
 * Execution pipeline broker.
 */
class ExecutionPipelineBroker : public ResourceBroker {
public:
    ExecutionPipelineBroker(std::string, unsigned int initiationInterval = 0);
    virtual ~ExecutionPipelineBroker();

    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res,
                        int immWriteCycle,
			int immRegIndex) override;

    virtual void unassign(MoveNode& node) override;

    virtual int earliestCycle(int cycle, const MoveNode& node,
                              const TTAMachine::Bus* bus,
                              const TTAMachine::FunctionUnit* srcFU,
                              const TTAMachine::FunctionUnit* dstFU,
                              int immWriteCycle,
                              const TTAMachine::ImmediateUnit* immu,
                              int immRegIndex) const override;
    virtual int latestCycle(int cycle, const MoveNode& node,
                            const TTAMachine::Bus* bus,
                            const TTAMachine::FunctionUnit* srcFU,
                            const TTAMachine::FunctionUnit* dstFU,
                            int immWriteCycle,
                            const TTAMachine::ImmediateUnit* immu,
                            int immRegIndex) const override;
    virtual bool isAlreadyAssigned(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* preassignedBus) const override;
    virtual bool isApplicable(
        const MoveNode& node, const TTAMachine::Bus*) const override;
    virtual void buildResources(const TTAMachine::Machine& target) override;
    virtual void setupResourceLinks(const ResourceMapper& mapper) override;
    virtual int highestKnownCycle() const;
    virtual bool isExecutionPipelineBroker() const override;
    virtual void setInitiationInterval(unsigned int cycles) override;
    virtual int longestLatency() const { return longestLatency_; }
    virtual void setMaxCycle(unsigned int maxCycle) override;
    void setDDG(const DataDependenceGraph* ddg);
private:
    bool isMoveTrigger(const MoveNode& node) const;
    bool isLoopBypass(const MoveNode& node) const;

    /// Map to link FU's and pipelines.
    typedef std::map<SchedulingResource*, const TTAMachine::FunctionUnit*>
        FUPipelineMap;
    int latestFromSource(
        int, const MoveNode&, const TTAMachine::FunctionUnit* srcFU) const;
    int latestFromDestination(
        int, const MoveNode&, const TTAMachine::FunctionUnit* dstFU) const;
    int earliestFromSource(
        int, const MoveNode&, const TTAMachine::FunctionUnit* srcFU) const;
    int earliestFromDestination(
        int, const MoveNode&, const TTAMachine::FunctionUnit* dstFU) const;

    /// FU's and their corresponding pipeline resources.
    FUPipelineMap fuPipelineMap_;
    /// Longest latency of all operations of all the FUs
    int longestLatency_;
    const DataDependenceGraph* ddg_;
};

#endif
