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
 * @file ExecutionPipelineResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the ExecutionPipelineResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTIONPIPELINERESOURCE_HH
#define TTA_EXECUTIONPIPELINERESOURCE_HH

#include<string>
#include<vector>
#include<map>

#include "SchedulingResource.hh"
#include "ProgramOperation.hh"

class PSocketResource;
/**
 * ExecutionPipelineResource keeps book of pipeline resource reservation
 * status. It uses rather simple resource reservation table approach.
 *
 */
class ExecutionPipelineResource : public SchedulingResource {
public:
    ExecutionPipelineResource(
        const std::string& name,
        const TTAMachine::FunctionUnit& fu,                                                   
        const unsigned int maxLatency,
        const unsigned int resNum);
    virtual ~ExecutionPipelineResource();

    virtual bool isInUse(const int cycle) const
        throw (Exception);
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const PSocketResource& pSocket,
        const bool triggering = false) const
        throw (Exception);
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void assign(const int cycle, MoveNode& node, bool source)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node, bool source)
        throw (Exception);
    virtual bool isExecutionPipelineResource() const;
    void setResourceUse(
        const std::string& opName,
        const int cycle,
        const int resIndex);
    void setLatency(
        const std::string& opName,
        const int output,
        const int latency);
    int highestKnownCycle() const;
    int nextResultCycle(int cycle, const MoveNode& node) const;
    
    void clear();
protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();
    int size() const;

private:
    // Type for resource vector
    typedef std::vector<bool> ResourceVector ;
    // Type for resource reservation table
    typedef std::vector<ResourceVector> ResourceTable;
    //Copying forbidden
    ExecutionPipelineResource(const ExecutionPipelineResource&);
    // Assignment forbidden
    ExecutionPipelineResource& operator=(const ExecutionPipelineResource&);

    // Find first and last cycles already scheduled for same PO
    void findRange(
        const int cycle,
        const MoveNode& node,
        int& first,
        int& last,
        int& triggering) const;

    //
    const TTAMachine::FunctionUnit& fu_;     
    // Resource and ports vector width, depends on particular FU
    int numberOfResources_;
    // Maximal latency of operation in FU
    int maximalLatency_;
    // Stores one resource vector per cycle of scope
    ResourceTable fuExecutionPipeline_;
    // Operations supported, name - index to operation pipeline vector
    std::map<std::string, int> operationSupported_;
    // Pipelines for operations
    std::vector<ResourceTable> operationPipelines_;
    // Stores for each result ready in a cycle in which it was ready
    // counts number of results that are ready in that cycle
    std::vector<std::pair<ProgramOperation*, int> > resultWriten_;
    // Stores for each operand write in a cycle in which it was written
    std::vector<ProgramOperation*> operandsWriten_;
    // Stores for each result read a cycle in which it was read
    // counts number of reads in given cycle
    std::vector<std::pair<ProgramOperation*, int> > resultRead_;
    // stores cycle in which the result is written to output register of
    // FU
    std::map<MoveNode*, int, MoveNode::Comparator> storedResultCycles_;
    // stores the set of assigned source nodes
    std::set<MoveNode*, MoveNode::Comparator> assignedSourceNodes_;
    // stores the set of assigned destination nodes
    std::set<MoveNode*, MoveNode::Comparator> assignedDestinationNodes_;
    // latencies of all operations supported by this FU.
    std::vector<std::map<int,int> > operationLatencies_;

    mutable int cachedSize_;
   
};

#endif
