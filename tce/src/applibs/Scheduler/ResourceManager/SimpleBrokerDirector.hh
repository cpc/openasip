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
 * @file SimpleBrokerDirector.hh
 *
 * Declaration of SimpleBrokerDirector class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
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
#include "MoveNode.hh"

namespace TTAProgram {
    class Instruction;
}

class ResourceBroker;
class IUBroker;
class ITemplateBroker;
class BusBroker;
class ExecutionPipelineBroker;
class DataDependenceGraph;

/**
 * A simple broker director.
 */
class SimpleBrokerDirector : public BrokerDirector {
public:
    SimpleBrokerDirector(
        const TTAMachine::Machine& machine,
        AssignmentPlan& plan,
        unsigned int initiationInterval_);

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
    virtual void loseInstructionOwnership(int cycle);
    virtual TTAProgram::Terminal* immediateValue(const MoveNode&);
    virtual int immediateWriteCycle(const MoveNode&) const;
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
    void clearOldResources();
    void clear();
    void setDDG(const DataDependenceGraph* ddg);

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

    std::map<const MoveNode*, OriginalResources*, MoveNode::Comparator> 
    origResMap_;
    int knownMaxCycle_;
    unsigned int initiationInterval_;

    /// the number of instructions to look back in the schedule
    /// for a free slot
    int schedulingWindow_;

    // how many moves in each cycle, for quick bus check fail.
    std::map<int,int> moveCounts_;
    int busCount_;

    unsigned int instructionIndex(unsigned int cycle) const;
};

#endif
