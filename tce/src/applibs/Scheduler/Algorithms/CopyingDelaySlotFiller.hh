/**
 * @file CopyingDelaySlotFiller.hh
 *
 * Definition of CopyingDelaySlotFiller class.
 *
 * @author Heikki Kultala 2007 (hkultala@cs.tut.fi)
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
