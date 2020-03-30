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
 * @file CycleLookBackSoftwareBypasser.hh
 *
 * Declaration of CycleLookBackSoftwareBypasser interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH
#define TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH

#include <map>
#include <set>

#include "SoftwareBypasser.hh"
#include "DataDependenceGraph.hh"

namespace TTAMachine {
    class Bus;
}
class MoveNodeSelector;
class MoveNode;
/**
 * A simple implementation of software bypassing that reschedules operand
 * writes as bypassed moves in case the result has been produced in n
 * previous cycles.
 */
class CycleLookBackSoftwareBypasser : public SoftwareBypasser {
public:
    CycleLookBackSoftwareBypasser();
    virtual ~CycleLookBackSoftwareBypasser();

    virtual int bypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm, bool bypassTrigger);

    virtual void removeBypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    virtual void removeBypass(
        MoveNode& moveNode,
        DataDependenceGraph& ddg,
        ResourceManager& rm, bool restoreSource=true);

    virtual int removeDeadResults(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    void setSelector(MoveNodeSelector* selector);
    
    virtual void clearCaches(DataDependenceGraph& ddg, bool removeDeadResults);

    static void printStats();
private:
    /// count of cycles before the operand write to look for the producer
    /// of the read value
    int cyclesToLookBack_;

    /// count of cycles before the operand write to look for the producer
    /// of the read value when cannot kill result
    int cyclesToLookBackNoDRE_;
    
    // whether dead results should be killed.
    bool killDeadResults_;

    // whether to bypass from register-to-register moves
    bool bypassFromRegs_;

    // whether to bypass to register-to-register moves
    bool bypassToRegs_;

    int bypassNode(
        MoveNode& nodeToBypass,
        int& lastOperandCycle,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    /// Stores sources and bypassed moves in case they
    /// have to be unassigned (case when operands are scheduled
    /// and bypassed but result can not be scheduled with such operands
    // First is bypassed node, second is original source
    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> storedSources_;
    
    // cycles and buses of source nodes so that they can be reassigned.
    std::map<MoveNode*, int> sourceCycles_;
    std::map<MoveNode*, const TTAMachine::Bus*> sourceBuses_;

    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> removedStoredSources_;

    // these are alreayd removed from the dubgraph. remove also from the
    // big ddg.
    DataDependenceGraph::NodeSet removedNodes_;

    MoveNodeSelector* selector_;

    static int bypassCount_;
    static int deadResultCount_;
    static int triggerAbortCount_;
};

#endif
