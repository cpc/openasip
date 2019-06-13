/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file BypassingBUBasicBlockScheduler.cc
 *
 * Declaration of BypassingBUBasicBlockScheduler class.
 *
 * This scheduler first schedules result reads of an operation, then
 * it tries to bypass the operands, and recursively schedule the 
 * operations which produce the result, bypassing the operands while
 * bypassing the results of the other operation. If it cannot schedule the
 * operation producing the value of the operand, it reverts the bypass,
 * and schedules operands from registers.
 *
 * @author Heikki Kultala 2011 (hkultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BYPASSING_BU_BB_SCHEDULER_HH
#define TTA_BYPASSING_BU_BB_SCHEDULER_HH

#include "BUMoveNodeSelector.hh"
#include "DDGPass.hh"
#include "BasicBlockPass.hh"
#include "BasicBlockScheduler.hh"
#include "MachinePart.hh"

class BasicBlockNode;
class SimpleResourceManager;
class SoftwareBypasser;
class CopyingDelaySlotFiller;
class DataDependenceGraphBuilder;
class DataDependenceGraph;
class RegisterRenamer;
class MoveNode;
class MoveNodeGroup;
class LLVMTCECmdLineOptions;

namespace TTAMachine {
    class Unit;
    class Port;
    class RegisterFile;
}

/**
 * A class that implements the functionality of a bottom up basic block 
 * scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class BypassingBUBasicBlockScheduler :
    public BasicBlockScheduler {
public:

    enum TempRegCopyLocation {
        TempRegNotAllowed,
        TempRegBefore,
        TempRegAfter };

    BypassingBUBasicBlockScheduler(
        InterPassData& data, SoftwareBypasser* bypasser=NULL, 
        CopyingDelaySlotFiller* delaySlotFiller=NULL,
        RegisterRenamer* registerRenamer = NULL);
    virtual ~BypassingBUBasicBlockScheduler();

    virtual void handleDDG(
        DataDependenceGraph& ddg, SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

    virtual MoveNodeSelector* createSelector( 
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) {
        return new BUMoveNodeSelector(bb, machine);
    }

    using BasicBlockPass::ddgBuilder;

    void scheduleOperation(MoveNodeGroup& mng, MoveNodeSelector& selector);

private:

    bool renameSourceIfNotConnected(
        MoveNode& moveNode, int latestCycle);

    void finalizeOperation(MoveNodeSelector& selector);

    bool scheduleOperation(
        ProgramOperation& po, int& latestCycle, bool allowTempRegCopies);
                           

    bool scheduleResults(
        ProgramOperation& po, int latestCycle, bool allowTempRegCopies);



    bool scheduleMoveUB(MoveNode& mn, int earlistCycle, int latestCycle);

    bool scheduleMoveBU(
        MoveNode& mn, int earlistCycle, int latestCycle, 
        TempRegCopyLocation t);

    int bypassNode(MoveNode& node, int maxHopCount);

    std::pair<MoveNode*, int> findBypassSource(
        MoveNode& node, int maxHopCount);

    bool bypassAndScheduleNode(
        MoveNode& node, MoveNode* trigger, int latestCycle, 
        bool allowRegCopies);

    bool bypassAndScheduleOperands(
        ProgramOperation& po, MoveNode* trigger, int latestCycle,
        bool allowRegCopies);

    bool scheduleOperandOrTrigger(
        MoveNode& operand, MoveNode* trigger, int latestCycle,
        bool allowRegCopies);

    void unscheduleResults(ProgramOperation& po);
    
    void unscheduleOperands(ProgramOperation& po);

    void unschedule(MoveNode& mn);

    void undoBypass(MoveNode& mn);

    void undoBypassAndUnschedule(MoveNode& mn);

    void unscheduleOperation(ProgramOperation& po);

    std::pair<int,int> operandCycleLimits(MoveNode& mn, MoveNode* trigger);

    int lastOperandCycle(const ProgramOperation& po);

    MoveNode* createTempRegCopy(MoveNode& mn, bool after);

    void createAntidepsFromUnscheduledRegCopies(
        MoveNode& copyNode, MoveNode& mn, 
        TTAProgram::Terminal& terminalRegister);

    std::set<TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator> 
    possibleTempRegRFs(const MoveNode& mn, bool tempRegAfter);

    void clearCaches();

    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> bypassSources_;
    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> removedBypassSources_;

    std::set<MoveNode*, MoveNode::Comparator> removedNodes_;
    std::set<MoveNode*, MoveNode::Comparator> pendingBypassSources_;
    std::set<MoveNode*, MoveNode::Comparator> scheduledMoves_;

    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> regCopiesBefore_;
    std::map<MoveNode*, MoveNode*, MoveNode::Comparator> regCopiesAfter_;


    bool killDeadResults_;
    bool renameRegisters_;
    int endCycle_;
};

#endif
