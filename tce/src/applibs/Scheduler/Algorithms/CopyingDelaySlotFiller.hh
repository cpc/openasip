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

class BasicBlockNode;
class BasicBlock;
class ControlFlowGraph;
class ControlFlowEdge;
class DataDependenceGraph;
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
}


class CopyingDelaySlotFiller {
public:
    CopyingDelaySlotFiller();
    ~CopyingDelaySlotFiller();
    void fillDelaySlots(
        ControlFlowGraph& cfg, DataDependenceGraph& ddg, 
        const TTAMachine::Machine& machine, UniversalMachine& um,
        bool releteRMs = true) throw (Exception);
    void addResourceManager(BasicBlock& bbn,ResourceManager& rm);
protected:
        void fillDelaySlots(
            BasicBlockNode& jumpingBB, int delaySlots, bool fillFallThru)
    throw (Exception);
private:
    typedef std::vector <std::list<MoveNode*> > MoveNodeListVector;

    TTAProgram::MoveGuard* createInverseGuard(TTAProgram::MoveGuard &mg);
    bool isFallThruEdge(ControlFlowEdge& e, TTAProgram::Move& jumpMove);
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

    void loseCopies();

    MoveNode& getMoveNode(MoveNode& old);
    ProgramOperation& getProgramOperation(ProgramOperation& old);
    TTAProgram::Move& getMove(TTAProgram::Move& old);

    bool wholePOMoved(ProgramOperation& po,  MoveNodeListVector& movesToCopy);
    
    int slotsFilled_;
    DataDependenceGraph* ddg_;
    ControlFlowGraph* cfg_;
    std::map<BasicBlock*,ResourceManager*> resourceManagers_;
    UniversalMachine* um_;

    // indexed by the original PO's
    std::map<ProgramOperation*,ProgramOperation*,ProgramOperation::Comparator>
    programOperations_;
    std::map<ProgramOperation*,ProgramOperation*,ProgramOperation::Comparator>
    oldProgramOperations_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> moveNodes_;
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> oldMoveNodes_;
    std::map<TTAProgram::Move*,TTAProgram::Move*> moves_;
    
    // garbage collection would be SOOOO nice!
    std::map<ProgramOperation*,bool,ProgramOperation::Comparator> poOwned_;
    std::map<MoveNode*,bool,MoveNode::Comparator> mnOwned_;
    std::map<TTAProgram::Move*,bool> moveOwned_;

    // do I have to free also resource managers?
};

#endif
