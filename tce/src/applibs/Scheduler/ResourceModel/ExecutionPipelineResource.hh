/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ExecutionPipelineResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the ExecutionPipelineResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
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
    // Records operation triggered in cycle. Necessary for opcodeSetting
    // and triggering ports separation
    // Each opcode setting port also triggers but non opcode setting ports
    // could trigger too. In which case the operation set by last
    // opcode setting move is started.
    std::map<int, std::string> opcodeSettingCycle_;
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
