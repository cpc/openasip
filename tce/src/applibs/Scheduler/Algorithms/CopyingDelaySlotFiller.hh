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
 * @file CopyingDelaySlotFiller.hh
 *
 * Definition of CopyingDelaySlotFiller class.
 *
 * @author Heikki Kultala 2007-2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef COPYING_DELAY_SLOT_FILLER_HH
#define COPYING_DELAY_SLOT_FILLER_HH

#include <map>
#include <vector>
#include <list>
#include "Exception.hh"
#include "ProgramOperation.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "ControlFlowEdge.hh"

class BasicBlockNode;
class ControlFlowGraph;
class ControlFlowEdge;
class SimpleResourceManager;
class ResourceManager;
class UniversalMachine;
class InterPassData;
class MoveNode;

namespace TTAMachine {
    class Machine;
    class RegisterFile;
}

namespace TTAProgram {
    class InstructionReferenceManager;
    class Move;
    class Immediate;
    class MoveGuard;
    class TerminalInstructionAddress;
    class CodeGenerator;
    class BasicBlock;
    class TerminalImmediate;
}


class CopyingDelaySlotFiller {
public:
    CopyingDelaySlotFiller();
    ~CopyingDelaySlotFiller();

    void initialize(
        ControlFlowGraph& cfg, DataDependenceGraph& ddg,
        const TTAMachine::Machine& machine);

    void fillDelaySlots(
        ControlFlowGraph& cfg, DataDependenceGraph& ddg,
        const TTAMachine::Machine& machine);
    void addResourceManager(TTAProgram::BasicBlock& bbn, SimpleResourceManager& rm);

    void bbnScheduled(BasicBlockNode& bbn);
    void finalizeProcedure();

    static std::pair<int, TTAProgram::Move*> findJump(
        TTAProgram::BasicBlock& bb,
        ControlFlowEdge::CFGEdgePredicate* pred = nullptr);

    static std::pair<TTAProgram::Move*, TTAProgram::Immediate*>
    findJumpImmediate(
        int jumpIndex, TTAProgram::Move& jumpMove,
        TTAProgram::InstructionReferenceManager& irm);

protected:
    bool fillDelaySlots(
        BasicBlockNode& jumpingBB, int delaySlots, bool fillFallThru);

private:
    typedef std::vector <std::list<MoveNode*> > MoveNodeListVector;
    typedef std::map<TCEString,TTAProgram::TerminalImmediate*> 
    PendingImmediateMap;


    bool areAllJumpPredsScheduled(BasicBlockNode& bbn) const;

    bool areAllJumpPredsFilled(BasicBlockNode& bbn) const;

    void bbFilled(BasicBlockNode& bbn);

    bool mightFillIncomingTo(BasicBlockNode& bbn);

    bool writesRegister(
        TTAProgram::Move& move, TTAMachine::RegisterFile* rf,
        unsigned int registerIndex);

    bool tryToFillSlots(
        BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBNode,
        bool fallThru, TTAProgram::Move* jumpMove, int slotsToFill,
        int removeGuards, int grIndex, TTAMachine::RegisterFile* grFile,
        TTAProgram::Move*& skippedJump, int delaySlots);

    bool updateJumpsAndCfg(
        BasicBlockNode& jumpBBN, 
        BasicBlockNode& fillingBBN,
        ControlFlowEdge& fillEdge,
        TTAProgram::Move* jumpAddressMove, 
        TTAProgram::Immediate* jumpAddressImmediate,
        TTAProgram::Move* jumpMove,
        int slotsFilled,
        TTAProgram::Move* skippedJump);

    bool updateFTBBAndCfg(
        BasicBlockNode& jumpingBB, BasicBlockNode& nextBBN,
        ControlFlowEdge& edge, int slotsFilled);

    void loseCopies(DataDependenceGraph::NodeSet* keptTempNodes);

    bool collectMoves(
        BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBN, 
        MoveNodeListVector& moves, int slotsToFill, bool fallThru, 
        int removeGuards, TTAProgram::Move* jumpMove, int grIndex, 
        TTAMachine::RegisterFile* grFile, TTAProgram::Move*& skippedJump,
        int delaySlots);

    bool checkImmediatesAfter(TTAProgram::BasicBlock& nextBB, int slotsToFill);

    bool checkIncomingDeps(
        MoveNode& mnOld, BasicBlockNode& blockToFillNode, int cycleDiff);

    bool tryToAssignNodes(
        MoveNodeListVector& moves, int slotsToFill, int firstCycleToFill, 
        ResourceManager& rm, int nextBBStart,
        DataDependenceGraph::NodeSet& tempAssigns);

    bool tryToAssignOtherMovesOfDestOps(
        MoveNode& mn, int firstCycleToFill, ResourceManager& rm, 
        int lastCycle, int nextBBStart, DataDependenceGraph::NodeSet& tempAssigns);

    bool tryToAssignOtherMovesOfOp(
        ProgramOperation& po, int firstCycleToFill, ResourceManager& rm, 
        int lastCycle, int nextBBStart, DataDependenceGraph::NodeSet& tempAssigns);

    void unassignTempAssigns(
        DataDependenceGraph::NodeSet& tempAssigns, SimpleResourceManager& rm);

    MoveNode& getMoveNode(
        MoveNode& old, BasicBlockNode& bbn, bool fillOverBackEdge);
    ProgramOperationPtr getProgramOperationPtr(
        ProgramOperationPtr old, BasicBlockNode& bbn, bool fillOverBackEdge);
    std::shared_ptr<TTAProgram::Move> getMove(TTAProgram::Move& old);

    bool poMoved(
        ProgramOperationPtr po,  MoveNodeListVector& movesToCopy, 
        DataDependenceGraph::NodeSet& tempAssigns);
    
    void finishBB(BasicBlockNode& bbn, bool force = false);

    DataDependenceGraph* ddg_;
    ControlFlowGraph* cfg_;

    std::map<TTAProgram::BasicBlock*,SimpleResourceManager*> resourceManagers_;
    UniversalMachine* um_;

    // indexed by the original PO's
    std::map<ProgramOperation*, ProgramOperationPtr, ProgramOperation::Comparator>
    programOperations_;
    std::map<ProgramOperation*, ProgramOperationPtr, ProgramOperation::Comparator>
    oldProgramOperations_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> moveNodes_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> oldMoveNodes_;
    std::map<TTAProgram::Move*,std::shared_ptr<TTAProgram::Move> > moves_;

    std::map<BasicBlockNode*, DataDependenceGraph::NodeSet> tempResultNodes_;
    std::map<BasicBlockNode*, std::set<
        ProgramOperationPtr, ProgramOperation::Comparator> > tempPOs_;

    // garbage collection would be SOOOO nice!
    std::map<MoveNode*,bool,MoveNode::Comparator> mnOwned_;


    enum BBNStates { BBN_UNKNOWN = 0, BBN_SCHEDULED = 1, BBN_JUMP_FILLED = 3,
                     BBN_FALLTHRU_FILLED = 5, BBN_BOTH_FILLED = 7,
                     BBN_TEMPS_CLEANED = 15,
                     BBN_ALL_DONE = 31 };

    // can go from uninitialized to UNKNOWN
    mutable std::map <BasicBlockNode*, BBNStates> bbnStatus_;
//    mutable std::map <BasicBlockNode*, BBNStates> bbnIncomingStatus_;
    int delaySlots_;

    ControlFlowGraph::NodeSet killedBBs_;
};

#endif
