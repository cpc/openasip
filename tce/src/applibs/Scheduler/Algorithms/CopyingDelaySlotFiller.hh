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
 * @file CopyingDelaySlotFiller.hh
 *
 * Definition of CopyingDelaySlotFiller class.
 *
 * @author Heikki Kultala 2007-2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef COPYING_DELAY_SLOT_FILLER_HH
#define COPYING_DELAY_SLOT_FILLER_HH

#include <map>
#include <vector>
#include <list>
#include "Exception.hh"
#include "ProgramOperation.hh"
#include "MoveNode.hh"

class BasicBlockNode;
class ControlFlowGraph;
class ControlFlowEdge;
class DataDependenceGraph;
class SimpleResourceManager;
class UniversalMachine;
class InterPassData;
class ResourceManager;

namespace TTAMachine {
    class Machine;
    class RegisterFile;
}

namespace TTAProgram {
    class InstructionReferenceManager;
    class Move;
    class Immediate;
    class MoveGuard;
    class BasicBlock;
}


class CopyingDelaySlotFiller {
public:
    CopyingDelaySlotFiller();
    ~CopyingDelaySlotFiller();
    void fillDelaySlots(
        ControlFlowGraph& cfg, DataDependenceGraph& ddg, 
        const TTAMachine::Machine& machine, bool deleteRMs = true) 
        throw (Exception);
    void addResourceManager(
        TTAProgram::BasicBlock& bbn, SimpleResourceManager& rm);
protected:
        void fillDelaySlots(
            BasicBlockNode& jumpingBB, int delaySlots, bool fillFallThru)
    throw (Exception);
private:
    typedef std::vector <std::list<MoveNode*> > MoveNodeListVector;

    TTAProgram::MoveGuard* createInverseGuard(TTAProgram::MoveGuard &mg);
    TTAProgram::Immediate* findJumpImmediate(
        int jumpIndex, TTAProgram::Move& jumpMove)
        throw (Exception);

    bool writesRegister(
        TTAProgram::Move& move, TTAMachine::RegisterFile* rf, 
        unsigned int registerIndex);

    bool 
    tryToFillSlots(
        BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBNode, 
        bool fallThru, TTAProgram::Move& jumpMove, int slotsToFill,
        int grIndex, TTAMachine::RegisterFile* grFile) throw (Exception);

    void updateJumps(
        BasicBlockNode& fillingBBN,
        TTAProgram::Immediate* jumpAddressImmediate,
        TTAProgram::Move* jumpMove,
        int slotsFilled);

    void loseCopies();

    bool collectMoves(
        BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBN, 
        MoveNodeListVector& moves, int slotsToFill, bool fallThru, 
        TTAProgram::Move& jumpMove, int grIndex, 
        TTAMachine::RegisterFile* grFile);

    bool checkIncomingDeps(
        MoveNode& mnOld, BasicBlockNode& blockToFillNode, int cycleDiff);

    bool tryToAssignNodes(
        MoveNodeListVector& moves, int slotsToFill, int firstCycleToFill, 
        ResourceManager& rm, int nextBBStart);

    void unassignTempAssigns(
	const std::list<MoveNode*>& tempAssigns, ResourceManager& rm);

    MoveNode& getMoveNode(MoveNode& old);
    ProgramOperationPtr getProgramOperationPtr(ProgramOperationPtr old);
    TTAProgram::Move& getMove(TTAProgram::Move& old);

    bool poMoved(ProgramOperation& po,  MoveNodeListVector& movesToCopy);
    
    DataDependenceGraph* ddg_;
    ControlFlowGraph* cfg_;
    std::map<TTAProgram::BasicBlock*, SimpleResourceManager*> resourceManagers_;
    UniversalMachine* um_;

    // indexed by the original PO's
    std::map<ProgramOperation*, ProgramOperationPtr, ProgramOperation::Comparator>
    programOperations_;
    std::map<ProgramOperation*, ProgramOperationPtr, ProgramOperation::Comparator>
    oldProgramOperations_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> moveNodes_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> oldMoveNodes_;
    std::map<TTAProgram::Move*,TTAProgram::Move*> moves_;
    
    // garbage collection would be SOOOO nice!
    std::map<MoveNode*,bool,MoveNode::Comparator> mnOwned_;
};

#endif
