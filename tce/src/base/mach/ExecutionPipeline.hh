/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ExecutionPipeline.hh
 *
 * Declaration of class ExecutionPipeline.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTION_PIPELINE_HH
#define TTA_EXECUTION_PIPELINE_HH

#include <vector>
#include <map>
#include <set>

#include "MachinePart.hh"
#include "Exception.hh"
#include "PipelineElement.hh"

class Operand;
class ObjectState;

namespace TTAMachine {

class HWOperation;
class FUPort;

/**
 * Represents an execution pipeline in function unit.
 */
class ExecutionPipeline : public SubComponent {
public:
    /// Set for operand indexes.
    typedef std::set<int> OperandSet;
    /// Set for pipeline elements.
    typedef std::set<PipelineElement*,PipelineElement::Comparator> ResourceSet;

    ExecutionPipeline(HWOperation& parentOperation);
    virtual ~ExecutionPipeline();

    const HWOperation* parentOperation() const;

    void addResourceUse(const std::string& name, int start, int duration);
    void addPortRead(int operand, int start, int duration);
    void addPortWrite(int operand, int start, int duration);
    void removeResourceUse(const std::string& name);
    void removeResourceUse(const std::string& name, int cycle);
    void removeAllResourceUses();
    void removeOperandUse(int operand, int cycle);
    bool isResourceUsed(const std::string& name, int cycle) const;

    ResourceSet resourceUsages(int cycle) const;

    bool isPortUsed(const FUPort& port, int cycle) const;
    bool isPortRead(const FUPort& port, int cycle) const;
    bool isPortWritten(const FUPort& port, int cycle) const;

    OperandSet readOperands(int cycle) const;
    OperandSet writtenOperands(int cycle) const;
    OperandSet readOperands() const;
    OperandSet writtenOperands() const;

    int latency() const;
    int latency(int output) const;
    int slack(int input) const;

    ObjectState* saveState() const;
    void loadState(const ObjectState* state);

    bool isArchitectureEqual(const ExecutionPipeline* pipeline) const;

    /// ObjectState name for ExecutionPipeline.
    static const std::string OSNAME_PIPELINE;
    /// ObjectState name for pipeline resource usage.
    static const std::string OSNAME_RESOURCE_USAGE;
    /// ObjectState name for operand read.
    static const std::string OSNAME_OPERAND_READ;
    /// ObjectState name for operand write.
    static const std::string OSNAME_OPERAND_WRITE;
    /// ObjectState attribute key for name of resource.
    static const std::string OSKEY_RESOURCE_NAME;
    /// ObjectState attribute key for operand number.
    static const std::string OSKEY_OPERAND;
    /// ObjectState attribute key for start cycle of a resource usage.
    static const std::string OSKEY_START_CYCLE;
    /// ObjectState attribute key for duration of a resource usage.
    static const std::string OSKEY_CYCLES;

private:
    /// Vector for ObjectState pointers.
    typedef std::vector<const ObjectState*> ObjectStateTable;

    /// Vector for resource sets.
    typedef std::vector<ResourceSet> ResourceUsage;
    /// Vector for operand sets.
    typedef std::vector<OperandSet> IOUsage;

    void checkStartCycle(int startCycle) const;
    static void checkDuration(int duration);
    static void checkResourceName(const std::string& name);
    void checkInputOperand(int operand) const;
    void checkOutputOperand(int operand) const;
    void checkResourceAvailability(
        const std::string& resource, int start, int duration) const;
    void checkOperandAvailability(int operand, int start, int duration) const;

    void internalAddResourceUse(
        const std::string& name,
        int start,
        int duration);
    void internalAddPortUse(
        int operand,
        int start,
        int duration,
        IOUsage& toModify);
    void internalRemoveResourceUse(
        const std::string& name,
        int start,
        int duration);
    void internalRemoveOperandUse(int operand, int start, int duration);
    void adjustLatency(int newLatency);
    bool isOperandBound(
        const FUPort& port,
        const OperandSet& operands) const;
    bool isOperandWritten(int operand) const;
    bool isOperandRead(int operand) const;
    int firstCycle() const;
    int firstCycleWithoutResource(const std::string& resource) const;
    int firstCycleWithoutResource(
        const std::string& resource,
        int cycle) const;
    int firstCycleWithoutOperandUse(int operand, int cycle) const;
    ResourceSet usedResources() const;
    PipelineElement* pipelineElement(const std::string& name) const;
    PipelineElement* addPipelineElement(const std::string& name) const;
    static void saveResourceUse(
        const PipelineElement* element,
        int cycleToSave,
        ObjectState* pipelineState);
    static void saveOperandUse(
        int operand,
        int cycleToSave,
        ObjectState* pipelineState,
        const std::string& osName);
    ObjectStateTable sortResourceUsages(const ObjectState* pipelineState) const;
    void addResourceUsage(
        ObjectStateTable& usages, const ObjectState* usageState) const;

    /// Resource usage.
    ResourceUsage resourceUsage_;
    /// Operand reads.
    IOUsage opReads_;
    /// Operand writes.
    IOUsage opWrites_;
    /// The parent operation.
    HWOperation* parent_;

};
}

#endif
