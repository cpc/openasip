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
 * @author Heikki Kultala 2009 (heikki.kultala-no-spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTIONPIPELINERESOURCE_HH
#define TTA_EXECUTIONPIPELINERESOURCE_HH

#include <climits>
#include <string>
#include <vector>
#include <map>

#include "SchedulingResource.hh"
#include "MoveNode.hh"
#include "SparseVectorMap.hh"

class DataDependenceGraph;
class ExecutionPipelineResourceTable;

class PSocketResource;
class ProgramOperation;

namespace TTAMachine {
    class FunctionUnit;
    class Port;
}

/**
 * ExecutionPipelineResource keeps book of pipeline resource reservation
 * status. It uses rather simple resource reservation table approach.
 *
 */
class ExecutionPipelineResource : public SchedulingResource {
public:
    ExecutionPipelineResource(
        const TTAMachine::FunctionUnit& fu,
        const unsigned int ii = 0);
    virtual ~ExecutionPipelineResource();

    virtual bool isInUse(const int cycle) const
        throw (Exception);
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssignSource(
        const int cycle,
        const MoveNode& node,
        const TTAMachine::Port& resultPort) const;
    virtual bool canAssignDestination(
        const int cycle,
        const MoveNode& node,
        const bool triggering = false) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void assignSource(
        const int cycle, MoveNode& node);
    virtual void assignDestination(
        const int cycle, MoveNode& node);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassignSource(
        const int cycle, MoveNode& node);
    virtual void unassignDestination(const int cycle, MoveNode& node);
    virtual bool isExecutionPipelineResource() const;
    int highestKnownCycle() const;
    int nextResultCycle(
        const TTAMachine::Port& port,
        int cycle, const MoveNode& node, const MoveNode* trigger = NULL,
        int triggerCycle=INT_MAX) const;

    bool resultNotOverWritten(
        int resultReadCycle, int resultReadyCycle,
        const MoveNode& node, const TTAMachine::Port& port,
        const MoveNode* trigger, int triggerCycle) const;

    void clear();
    void setDDG(const DataDependenceGraph* ddg);
protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();
    unsigned int size() const;
private:
    struct ResultHelper {
        unsigned int realCycle;
        const ProgramOperation* po;
        int count;
        ResultHelper() : realCycle(0), po(NULL), count(0) {};
        ResultHelper(int rc, const ProgramOperation* p, int c) : 
            realCycle(rc), po(p), count(c) {}
    };

    typedef std::pair<ResultHelper,ResultHelper> ResultHelperPair;

    typedef std::pair<const MoveNode*,const MoveNode*> ResourceReservation;


    /// Type for resource vector, represents one cycle of use.
    /// Includes the ownerships of the reservation.
    typedef std::vector<ResourceReservation> 
    ResourceReservationVector;

    /// Used for both result read and result written.
    typedef SparseVector<ResultHelperPair> ResultVector;

    typedef std::map<const TTAMachine::Port*, ResultVector> ResultMap;

    /// Type for resource reservation table, resource vector x latency.
    /// Includes the ownerships of the reservation.
    typedef SparseVector<ResourceReservationVector> ResourceReservationTable;
    

    //Copying forbidden
    ExecutionPipelineResource(const ExecutionPipelineResource&);
    // Assignment forbidden
    ExecutionPipelineResource& operator=(const ExecutionPipelineResource&);

    /// Find first and last cycles already scheduled for same PO
    void findRange(
        const int cycle,
        const MoveNode& node,
        int& first,
        int& last,
        int& triggering) const;

    int resultReadyCycle(
        const ProgramOperation& po, const TTAMachine::Port& resultPort) const;

    void setResultWriten(
        const TTAMachine::Port& port, 
        unsigned int realCycle, 
        const ProgramOperation& po);

    void setResultWriten(
        const ProgramOperation& po, 
        unsigned int triggerCycle);

    void unsetResultWriten(
        const TTAMachine::Port& port, unsigned int realCycle, 
        const ProgramOperation& po);

    void unsetResultWriten(
        const ProgramOperation& po, 
        unsigned int triggerCycle);

    bool testTriggerResult(const MoveNode& trigger, int cycle) const;
    bool resultAllowedAtCycle(
        int resultCycle, const ProgramOperation& po, 
        const TTAMachine::Port& resultPort,
        const MoveNode& trigger, int triggerCycle) const;

    const TTAMachine::Port& resultPort(const MoveNode& mn) const;

    bool exclusiveMoves(
        const MoveNode* mn1, const MoveNode* mn2, int cycle=INT_MAX) const;

    const ExecutionPipelineResourceTable* resources;

    /// Stores one resource vector per cycle of scope for whole FU
    mutable ResourceReservationTable fuExecutionPipeline_;

    // Stores PO for each "result ready" a cycle in which it was produced,
#if 0
    // Test if other program operation writes into same register
    // in given cycle
    bool sameRegisterWrite(
	const MoveNode& node,
	ProgramOperation* operation)const;
#endif
    // counts number of results that are ready in that cycle
    ResultMap resultWriten_;
    // Stores PO for each "result read" move a cycle in which it was read,
    // counts number of reads in given cycle
    ResultMap resultRead_;

    // Stores for every cycle the PO whose operand writes are in that cycle
    // these are modcycles, not real cycles
    SparseVector<std::pair<ProgramOperation*,ProgramOperation*> > 
        operandsWriten_;
    // Stores for each move a cycle in which the result is written to output
    // register of FU, this information is not available elsewhere
    // these are real cycles.
    std::map<MoveNode*, int, MoveNode::Comparator> storedResultCycles_;
    // stores the set of assigned source nodes
    std::multimap<int, MoveNode*> assignedSourceNodes_;
    // stores the set of assigned destination nodes
    std::multimap<int, MoveNode*> assignedDestinationNodes_;

    mutable int cachedSize_;

    const DataDependenceGraph* ddg_;
    const TTAMachine::FunctionUnit& fu_;
};

#endif
