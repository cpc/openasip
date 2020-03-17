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
 * @file AssignmentPlan.hh
 *
 * Declaration of AssignmentPlan class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ASSIGNMENT_PLAN_HH
#define TTA_ASSIGNMENT_PLAN_HH

#include <vector>
#include "Exception.hh"

class ResourceBroker;
class PendingAssignment;
class MoveNode;
class SchedulingResource;

namespace TTAMachine {
    class Bus;
    class FunctionUnit;
    class ImmediateUnit;
}

/**
 * Keeps the order in which single resource-specific brokers are
 * invoked to perform their bit of the allocation or assignment. Also
 * records the current state of tentative and potential assignments
 * for each type of resource.
 */
class AssignmentPlan {
public:
    AssignmentPlan();
    virtual ~AssignmentPlan();

    void insertBroker(ResourceBroker& broker);
    void setRequest(
        int cycle, MoveNode& node, const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex);
    ResourceBroker& firstBroker();
    ResourceBroker& nextBroker(ResourceBroker& pos);
    ResourceBroker& lastBroker();
    ResourceBroker& currentBroker();
    void advance();
    void backtrack();
    void tryNextAssignment();
    bool isTestedAssignmentPossible();
    void resetAssignments();
    void resetAssignments(MoveNode& node);

    int brokerCount() const;
    ResourceBroker& broker(int index) const;
    void clear();

    bool tryCachedAssignment(MoveNode& node, int cycle,
                             const TTAMachine::Bus* bus,
                             const TTAMachine::FunctionUnit* srcFU,
                             const TTAMachine::FunctionUnit* dstFU,
                             int immWriteCycle,
                             const TTAMachine::ImmediateUnit* immu,
                             int immRegIndex);
    void clearCache();
private:

    const TTAMachine::Bus* bus_;
    const TTAMachine::FunctionUnit* srcFU_;
    const TTAMachine::FunctionUnit* dstFU_;
    int immWriteCycle_;
    const TTAMachine::ImmediateUnit* immu_;
    int immRegIndex_;

    /// Sequence of pending assignments.
    std::vector<PendingAssignment*> assignments_;
    /// Sequence of applicable pending assignments.
    std::vector<PendingAssignment*> applicableAssignments_;
    /// Sequence of resource brokers.
    std::vector<ResourceBroker*> brokers_;
    /// Move of current resource assignment request.
    MoveNode* node_;
    /// Cycle in which current node should be placed.
    int cycle_;
    /// Current broker.
    int currentBroker_;
    /// True if a valid resource of current broker has been assigned.
    bool resourceFound_;

    ///  cache.
    std::vector<std::pair<ResourceBroker*, SchedulingResource*> > 
    lastTestedWorkingAssignment_;
    MoveNode* lastTriedNode_;
    int lastTriedCycle_;
};

#endif
