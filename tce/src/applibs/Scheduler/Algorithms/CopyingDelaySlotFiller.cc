/**
 * @file CopyingDelaySlotFiller.cc
 *
 * Implementation of of CopyingDelaySlotFiller class.
 *
 * @author Heikki Kultala 2007 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <vector>
#include <list>

#include "Machine.hh"
#include "UniversalMachine.hh"
#include "ControlUnit.hh"
#include "Immediate.hh"
#include "ImmediateUnit.hh"
#include "Guard.hh"
#include "UniversalFunctionUnit.hh"

#include "TerminalInstructionAddress.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "TerminalRegister.hh"

#include "MoveNode.hh"
#include "ProgramOperation.hh"
#include "ResourceManager.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"

#include "POMDisassembler.hh"
#include "CopyingDelaySlotFiller.hh"

#include "InterPassDatum.hh"
#include "InterPassData.hh"

//using std::set;
using std::list;
using std::vector;
using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Fill delay slots of given BB.
 * 
 * @param jumpingBB BB to fill delay slots.
 * @param delaySlots number of delay slots in the machine.
 * @param fillFallThru fill from Fall-thru of jump BB?
 */
void CopyingDelaySlotFiller::fillDelaySlots(
    BasicBlockNode &jumpingBB, int delaySlots, bool fillFallThru) 
    throw (Exception) {

    
    ControlFlowGraph::EdgeSet outEdges = cfg_->outEdges(jumpingBB);

    InstructionReferenceManager& irm = cfg_->program()->
        instructionReferenceManager();

    // should be the same
    int jumpIndex = -1;
    Move* jumpMove = NULL;

    BasicBlock& thisBB = jumpingBB.basicBlock();
    for (int i = thisBB.instructionCount()-1; i >= 0; i--) {
        Instruction& ins = thisBB.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++ ) {
            Move& move = ins.move(j);
            if (move.isJump()) {
                jumpIndex = i;
                jumpMove = &move;
                break;
            }
        }
        // found?
        if (jumpMove != NULL) {
            break;
        }
        if (ins.hasCall()) {
            return; // not yet support calls
        }
    }
    // not found?
    if ( jumpMove == NULL ) {
        return;
    }

    Immediate* jumpImm = NULL;

    // need for imm source only if filling jump, not imm.
    if (!fillFallThru) {
        if ( !jumpMove->source().isInstructionAddress()) {
            // address comes from LIMM, search the write to limm reg.
            jumpImm = findJumpImmediate(jumpIndex, *jumpMove);
            if (jumpImm == NULL) {
//                cout << "Imm source not found, aborting" << std::endl;
                return;
            }
        }
    }

    RegisterFile* grFile = NULL;
    unsigned int grIndex = -1;

    // lets be aggressive, fill more than just slots?
    int maxFillCount = std::min(delaySlots+3, thisBB.instructionCount());

    // if we have references to instructions in target BB, cannot put anything
    // before them, so limit how many slots fill at maximum.
    for (int i = 1 ; i < thisBB.instructionCount(); i++) {
        if (irm.hasReference(thisBB.instructionAtIndex(i))) {
            maxFillCount = std::min(maxFillCount, thisBB.instructionCount()-i);
        }
    }

    if (!jumpMove->isUnconditional()) {
        Guard& g = jumpMove->guard().guard();
        RegisterGuard* rg = dynamic_cast<RegisterGuard*>(&g);
        if (rg != NULL) {
            grFile = rg->registerFile();
            grIndex = rg->registerIndex();
        } else { 
            return; // port guards not yet supported
        }
        // if conditional, fill only slots+jump ins
        maxFillCount = std::min(maxFillCount,delaySlots+1);
    }

    // 1 or two items, big loop. These may come in either order, grr.
    for (ControlFlowGraph::EdgeSet::iterator iter = outEdges.begin();
         iter != outEdges.end(); iter++) {
        int slotsFilled = 0;

        ControlFlowEdge& edge = **iter;
        if(isFallThruEdge(edge,*jumpMove) != fillFallThru) {
            continue;
        }

        BasicBlockNode& nextBBN = cfg_->headNode(edge);

        if (nextBBN.isNormalBB()) {

            // cannot remove ins if more than one input path to BB
            if (fillFallThru && cfg_->inDegree(nextBBN) != 1) {
                continue;
            }

            // fix antideps between ndoes in these BBs.
            ddg_->fixInterBBAntiEdges(jumpingBB, nextBBN);
            
            // also try to fill into jump instruction.
            // fillSize = delaySlots still adpcm-3-full-fails, should be +1
            for (int fillSize = maxFillCount; fillSize > 0; fillSize--) {
                bool ok = tryToFillSlots(
                    jumpingBB, nextBBN, fillFallThru, *jumpMove, fillSize,
                    grIndex, grFile);
                if (ok) {
                    slotsFilled = fillSize;
                    break;
                }
            }
        }

        // filled some slots?
        if (slotsFilled != 0) {
            // filled from jump dest or fal-thru? 
            if (!fillFallThru) {
                // update jump destination

                InstructionReferenceManager& irm = cfg_->program()->
                    instructionReferenceManager();
                // TODO: only the correct jump one, nto both
                assert(slotsFilled <= nextBBN.basicBlock().instructionCount());
                if ( slotsFilled == nextBBN.basicBlock().instructionCount()) {
                    ControlFlowGraph::NodeSet nextBBs = 
                        cfg_->successors(nextBBN);
                    if (nextBBs.size() != 1) {
                        std::string msg = "no succeessors but no jump";
                        throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
                    }
                    assert(
                        (*nextBBs.begin())->basicBlock().instructionCount()
                        != 0);
                    
                    InstructionReference& ir = irm.createReference(
                        (*nextBBs.begin())->basicBlock().instructionAtIndex(
                            0));
                    if (jumpImm == NULL) {
                        jumpMove->source().setInstructionReference(ir);
                    } else {
                        jumpImm->setValue(new TerminalInstructionAddress(ir));
                    }
                } else {
                    InstructionReference& ir = irm.createReference(
                        nextBBN.basicBlock().instructionAtIndex(slotsFilled));
                    
                    if (jumpImm == NULL) {
                        jumpMove->source().setInstructionReference(ir);
                    } else {
                        jumpImm->setValue(new TerminalInstructionAddress(ir));
                    }
                }
            } else { // fall-thru, skip first instructions of next BB.
                nextBBN.basicBlock().skipFirstInstructions(slotsFilled);
            }
        }
    }
}

/**
 * Constructor.
 */
CopyingDelaySlotFiller::CopyingDelaySlotFiller() : slotsFilled_(0) {
}

/**
 *
 * Fills all delay slots for all BB's in the CFG.
 * 
 * @param cfg ControlFlowGraph where to fill dedaly slots.
 * @param ddg DataDependenceGraph containing data dependencies.
 */
void 
CopyingDelaySlotFiller::fillDelaySlots(
    ControlFlowGraph& cfg, DataDependenceGraph& ddg,
    const TTAMachine::Machine& machine, UniversalMachine& um, 
    bool deleteRMs) throw (Exception) {

    um_ = &um;
    int delaySlots = machine.controlUnit()->delaySlots();

    cfg_ = &cfg;
    ddg_ = &ddg;
    // first fill only jumps
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            fillDelaySlots(bbn, delaySlots, false);
        }
    }
    // then fill only fall-thru's.
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            fillDelaySlots(bbn, delaySlots, true);
        }
    }

    if (deleteRMs) {
        AssocTools::deleteAllValues(resourceManagers_);
    }
}

/*
 * Adds a resource manager to the filler. 
 *
 * @param bb Basic block which the resource manager handles.
 * @param rm Resource manager or the bb.
 */
void 
CopyingDelaySlotFiller::addResourceManager(
    BasicBlock& bb,ResourceManager& rm) {
    resourceManagers_[&bb] = &rm;
}

/**
 * Checks if an edge is a fall-thru or a jump edge
 * 
 * @param e edge being checked
 * @param jumpMove move containing the jump
 * @return true if edge is fall-thru, not jump
 */
bool 
CopyingDelaySlotFiller::isFallThruEdge(ControlFlowEdge& e, Move& jumpMove) {

    return (e.isTrueEdge() && jumpMove.guard().guard().isInverted())
        || (e.isFalseEdge() && !jumpMove.guard().guard().isInverted());
}

/**
 * Finds the immediate which contains the jump address.
 *
 * @param jumpIndex index of the instruction containin the jump in the BB. 
 * @param jumpMove the move containing the jump.
 * @return Immediate containing jump address or NULL if not found.
 */
Immediate*
CopyingDelaySlotFiller::findJumpImmediate(
    int jumpIndex, Move& jumpMove) throw (Exception) {
    CodeSnippet& bb = jumpMove.parent().parent();

    Move* irMove = &jumpMove;
    int irIndex = jumpIndex;

    // Register copy or some unknown source.
    // should handle unlimited number of reg. copies
    while (irMove->source().isGPR()) {
        const RegisterFile* rf = &irMove->source().registerFile();
        int regIndex = static_cast<int>(irMove->source().index());
        int found = false;
        for( int i = irIndex -1 ; i >= 0 && !found; i-- ) {
            Instruction &ins = bb.instructionAtIndex(i);
            for (int j = 0; j < ins.moveCount(); j++ ) {
                Move& move = ins.move(j);
                if (move.destination().isGPR()) {
                    if (&move.destination().registerFile() == rf &&
                        move.destination().index() == regIndex) {
                        irMove = &move;
                        irIndex = i;
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    
    // then read the actual immediate
    if (irMove->source().isImmediateRegister()) {
        const ImmediateUnit& immu = irMove->source().immediateUnit();
        int index = static_cast<int>(irMove->source().index());
        for( int i = irIndex -1 ; i >= 0; i-- ) {
            Instruction &ins = bb.instructionAtIndex(i);
            for ( int j = 0; j < ins.immediateCount(); j++ ) {
                Immediate& imm = ins.immediate(j);
                if (imm.destination().index() == index &&
                    &imm.destination().immediateUnit() == &immu) {
                    return &imm;
                }
            }
        }
    }
    return NULL;
}
/**
 * Checks whether given move writes to given register
 * 
 * @param move to check
 * @param rf RegisterFile containing the register
 * @param registerIndex index of the register.
 */
bool CopyingDelaySlotFiller::writesRegister(
    Move& move, RegisterFile* rf, unsigned int registerIndex) {
    Terminal& term = move.destination();
    if (term.isGPR()) {
        TerminalRegister& rd = dynamic_cast<TerminalRegister&>(term);
        if ( &rd.registerFile() == rf && rd.index() == 
             static_cast<int>(registerIndex)) {
            return true;
        }
    }
    return false;
}

/** 
 * Tries to fill n slots from other BB. 
 *
 * Aborts cannot fill all of the slots
 *
 * @param blockToFill BB containing the delay slots.
 * @param nextBB block where from to copy the instructions.
 * @param slotsToFill how many slots tries to fill
 */
bool 
CopyingDelaySlotFiller::tryToFillSlots(
    BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBNode, 
    bool fallThru, Move& jumpMove, int slotsToFill,
    int grIndex, RegisterFile* grFile) throw (Exception) {

    ResourceManager& rm = *resourceManagers_[&blockToFillNode.basicBlock()];
    BasicBlock& blockToFill = blockToFillNode.basicBlock();
    BasicBlock& nextBB = nextBBNode.basicBlock();

    int firstToFill = blockToFill.instructionCount() - slotsToFill;

    if (fallThru) {
        // test that we can create an inverse guard
        MoveGuard* invG = createInverseGuard(jumpMove.guard());
        if (invG == NULL) {
            return false; 
        } else {
            delete invG;
        }
        // don't leave empty BBs until ProcedurePass::copyCfgBackToProcedure 
        // can handle those
        if (slotsToFill == nextBB.instructionCount()) {
            return false;
        }
    }

    // should be > instead of >=
    if (slotsToFill > nextBB.instructionCount()) {
        return false;
    }
    
    bool failed = false;

    MoveNodeListVector moves(slotsToFill);

    // Find all moves to copy and check their dependencies.
    // Loop thru instructions first..
    for (int i = 0; i < slotsToFill; i++) {
        Instruction& filler = nextBB.instructionAtIndex(i);
            
        // do not fill with instructions that have LIMMs
        // until we a) can assign LIMM write without read
        if (filler.immediateCount() > 0) {
            failed = true;
            break;
        }

        if (filler.hasJump() || filler.hasCall()) {
            failed = true;
            break; // goto cleanup
        }

        // loop thru all moves in the instr
        for (int j = 0; j < filler.moveCount(); j++) {
            Move& oldMove = filler.move(j);

            // check if it overwrites the guard of the jump
            if (writesRegister(oldMove, grFile, grIndex)) {
                // overwriting the guard reg as last thing does not matter,
                // only allow it in last imported instruction.
                if ( i != slotsToFill-1) {
                    failed = true;
                    break;
                } 
            }

            // Do not fill with guarded moves, if jump is guarded,
            // might need 2 guards.
            if (!oldMove.isUnconditional() && grFile != NULL) {
                failed = true;
                break;
            }

            // check all deps
            MoveNode& mnOld = ddg_->nodeOfMove(oldMove);

            DataDependenceGraph::EdgeSet inEdges = ddg_->inEdges(mnOld);
            for (DataDependenceGraph::EdgeSet::iterator ieIter = 
                     inEdges.begin(); ieIter != inEdges.end(); ieIter++) {
                // slow
                DataDependenceEdge& ddEdge = **ieIter;
                MoveNode& pred = ddg_->tailNode(ddEdge);
                if (pred.isMove()) {
                    BasicBlockNode& predBlock = ddg_->getBasicBlockNode(pred);

                    if (&predBlock == &blockToFillNode) {
                        int nodeCycle = ddEdge.dependenceType() == 
                            DataDependenceEdge::DEP_WAR ?
                            pred.cycle() : pred.cycle()+1;
                        if (nodeCycle > firstToFill+i) {
                            failed = true;
                            break;
                        }
                    }
                }
            }
            if (failed) {
                break;
            }
            MoveNode& mn = getMoveNode(mnOld); // also copies move
            Move& newMove = mn.move();

            if (!jumpMove.isUnconditional()) {
                if (fallThru) {
                    newMove.setGuard(createInverseGuard(jumpMove.guard()));
                } else {
                    newMove.setGuard(jumpMove.guard().copy());
                }
            }

            moves.at(i).push_back(&mn);
            for (int j = 0; j < filler.immediateCount(); j++) {
// TODO: immediate support. now disabled to be safe.                
//                immeds.insert(filler.immediate(j).copy());
                failed = true;
                break;
            }
        }
        if (failed) {
            break; // also break out from outer loop
        }
    }
    if (failed) {
        loseCopies();
        return false;
    }

    // then try to assign the nodes
    for (int i = 0; i < slotsToFill; i++) {
        list<MoveNode*>& movesInThisCycle = moves.at(i);
        int currentCycle = firstToFill + i;

        for (std::list<MoveNode*>::iterator iter = movesInThisCycle.begin();
             iter != movesInThisCycle.end(); iter++) {
            MoveNode& mn = **iter;
            if (rm.canAssign(currentCycle, mn)) {
                rm.assign(currentCycle, mn);
                assert(mn.isScheduled());
                assert(mn.cycle() == currentCycle);

                // check that result scheduling is possible
                if (mn.isDestinationOperation()) {
                    ProgramOperation& po = mn.destinationOperation();
                    if (po.areInputsAssigned()) {
                        for (int j = 0; j < po.outputMoveCount(); j++) {
                            MoveNode& mn = po.outputMove(j);
                            int mnCycle = 
                                firstToFill + oldMoveNodes_[&mn]->cycle();
                            if (!rm.canAssign(mnCycle, mn)) {
                                failed = true;
                            }
                        }
                        if (failed) {
                            break;
                        }
                    }
                }
            } else {
                failed = true;
                break;
            }
        }
        if (failed) { 
            break;
        }
    }
    if (failed) {
        // cleanup part 2 : delete movenodes
        for (int i = 0; i < slotsToFill; i++) {
            list<MoveNode*>& movesInThisCycle = moves.at(i);
            for (std::list<MoveNode*>::iterator iter = 
                     movesInThisCycle.begin();
                 iter != movesInThisCycle.end();iter++) {
                MoveNode& mn = **iter;
                if (mn.isScheduled()) {
                    rm.unassign(mn);
                }
            }
        }
        loseCopies();
        return false;
    }

    // all ok, add filled nodes and PO's to DDG
    // first check which PO's to copy
    for (std::map<ProgramOperation*,bool>::iterator iter = poOwned_.begin();
         iter != poOwned_.end(); iter++) {
        ProgramOperation* po = iter->first;
        if (wholePOMoved(*po, moves)) {
            iter->second = false;
            ddg_->addProgramOperation(po);
        }
    }

    for (int i = 0; i < slotsToFill; i++) {
        list<MoveNode*> movesInThisCycle = moves.at(i);
        for (std::list<MoveNode*>::iterator iter = movesInThisCycle.begin();
             iter != movesInThisCycle.end(); iter++) {
            MoveNode& mn = **iter;
            ddg_->addNode(mn, blockToFillNode);
            mnOwned_[&mn] = false;
            ddg_->copyDependencies(*oldMoveNodes_[&mn],mn);
            // adjust program operations to original in other BB is not
            // whole BB copied.
            if (mn.isSourceOperation()) {
                ProgramOperation& po = mn.sourceOperation();
                // whole PO not copied, set to old PO
                if (poOwned_[&po] == true) {
                    ProgramOperation* oldPO = oldProgramOperations_[&po];
                    mn.setSourceOperation(*oldPO);
                    oldPO->addOutputNode(mn);
                }
            }
            if (mn.isDestinationOperation()) {
                ProgramOperation& po = mn.destinationOperation();
                // whole PO not copied, set to old PO
                if (poOwned_[&po] == true) {
                    ProgramOperation* oldPO = oldProgramOperations_[&po];
                    mn.setDestinationOperation(*oldPO);
                    oldPO->addInputNode(mn);
                }
            }
        }
    }

    loseCopies();
    return true;
}

/**
 * 
 * Gets a corresponding MoveNode a given move in the next BB.
 *
 * If no corresponding MoveNode created, creates one
 *
 * @param old ProgramOperation in jump target BB.
 * @return new MoveNode for this BB.
 */

MoveNode&
CopyingDelaySlotFiller::getMoveNode(MoveNode& old) {
    if (AssocTools::containsKey(moveNodes_,&old)) {
        return *moveNodes_[&old];
    } else {
        Move& move = getMove(old.move());
        MoveNode *newMN = new MoveNode(move);
        moveOwned_[&move] = false;
        moveNodes_[&old] = newMN;
        oldMoveNodes_[newMN] = &old;
        mnOwned_[newMN] = true;
        if (old.isSourceOperation()) {
            newMN->setSourceOperation(
                getProgramOperation(old.sourceOperation()));
        }
        if (old.isDestinationOperation()) {
            newMN->setDestinationOperation(
                getProgramOperation(old.destinationOperation()));
        }
        return *newMN;
    }    
}

/**
 * Gets a corresponding ProgramOperation to a given move in the next BB.
 *
 * If no corresponding ProgramOperation created, creates one
 *
 * @param old ProgramOperation in jump target BB.
 * @return new ProgramOperation for this BB.
 */
ProgramOperation& 
CopyingDelaySlotFiller::getProgramOperation(ProgramOperation& old) {
    if (AssocTools::containsKey(programOperations_,&old)) {
        return *programOperations_[&old];
    } else {
        ProgramOperation* po = new ProgramOperation(old.operation());
        poOwned_[po] = true;
        programOperations_[&old] = po;
        oldProgramOperations_[po] = &old;
        for (int i = 0; i < old.inputMoveCount();i++) {
            MoveNode& mn = old.inputMove(i);
            po->addInputNode(getMoveNode(mn));
        }
        for (int j = 0; j < old.outputMoveCount();j++) {
            MoveNode& mn = old.outputMove(j);
            po->addOutputNode(getMoveNode(mn));
        }
        return *po;
    }
}

/**
 * Gets a corresponding move to a given move in the next BB.
 *
 * If no corresponding move created, creates one
 * 
 * @param old Move in jump target BB.
 * @return new Move for this BB.
 */
Move&
CopyingDelaySlotFiller::getMove(Move& old) {
    if (AssocTools::containsKey(moves_,&old)) {
        return *moves_[&old];
    } else {
        MoveNode& oldMN = ddg_->nodeOfMove(old);

        Move* newMove = old.copy();
        newMove->setBus(um_->universalBus());

        if (oldMN.isSourceOperation()) {
            Terminal& source = newMove->source();
            assert(source.isFUPort());
            std::string fuName = source.functionUnit().name();
            TTAProgram::ProgramAnnotation srcUnit(
                TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_SRC, 
                fuName);
            newMove->setAnnotation(srcUnit);

            Operation &srcOp = oldMN.sourceOperation().operation();
            HWOperation& hwop = *um_->universalFunctionUnit().operation(
                srcOp.name());
            newMove->setSource(new TerminalFUPort(
                                   hwop, old.source().operationIndex()));
        }
        if (oldMN.isDestinationOperation()) {
            Terminal& dest = newMove->destination();
            assert(dest.isFUPort());

            std::string fuName = dest.functionUnit().name();
            TTAProgram::ProgramAnnotation dstUnit(
                TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_DST, 
                fuName);
            newMove->setAnnotation(dstUnit);

            Operation &dstOp = oldMN.destinationOperation().operation();
            HWOperation& hwop = *um_->universalFunctionUnit().operation(
                dstOp.name());
            newMove->setDestination(new TerminalFUPort(
                                   hwop, old.destination().operationIndex()));
        }            

        moves_[&old] = newMove;
        // todo: guard?
        moveOwned_[newMove] = true;
        return *newMove;
    }
}

/**
 * Deletes MoveNodes and programOperations not put into final graph.
 *
 * Loses all bookkeeping of corresponging stuff between BB's
 */
void CopyingDelaySlotFiller::loseCopies() {
    
    for (std::map<MoveNode*,MoveNode*>::iterator mnIter =
             moveNodes_.begin(); mnIter != moveNodes_.end();
         mnIter++ ) {
        if (mnOwned_[mnIter->second] == true) {
            delete mnIter->second;
        }
    }
    moveNodes_.clear();
    mnOwned_.clear();
    oldMoveNodes_.clear();
    
    for (std::map<Move*,Move*>::iterator mIter = moves_.begin(); 
         mIter != moves_.end();
         mIter++ ) {
        if (moveOwned_[mIter->second] == true) {
            delete mIter->second;
        }
    }
    moves_.clear();
    moveOwned_.clear();
    
    for (std::map<ProgramOperation*,ProgramOperation*>::iterator poIter =
             programOperations_.begin(); poIter != programOperations_.end();
         poIter++ ) {
        if (poOwned_[poIter->second] == true) {
            delete poIter->second;
        }
    }
    programOperations_.clear();
    oldProgramOperations_.clear();
    poOwned_.clear();    
}

/**
 * Destructor.
 */
CopyingDelaySlotFiller::~CopyingDelaySlotFiller() {
    assert(programOperations_.size() == 0);
    assert(poOwned_.size() == 0);
    assert(moveNodes_.size() == 0);
    assert(mnOwned_.size() == 0);
    assert(moves_.size() == 0);
    assert(moveOwned_.size() == 0);
    
    //should not be needed but lets be sure
    loseCopies();
}

/**
 * Creates a guard with same guard register etc but inverted.
 *
 * TODO: support for port guards
 *
 * @param mg guard to inverse
 * @return new MoveGuard that is given guard inevrted.
 */
TTAProgram::MoveGuard* 
CopyingDelaySlotFiller::createInverseGuard(TTAProgram::MoveGuard &mg) {
    Guard& g = mg.guard();
    bool inv = g.isInverted();
    RegisterGuard* rg = dynamic_cast<RegisterGuard*>(&g);
    if (rg != NULL) {
        RegisterFile* rf = rg->registerFile();
        int regIndex = rg->registerIndex();
        Bus* parentBus = rg->parentBus();
        
        // find guard
        for (int i = 0 ; i<parentBus->guardCount(); i++) {
            Guard *g2 = parentBus->guard(i);
            RegisterGuard* rg2 = 
                dynamic_cast<RegisterGuard*>(g2);
            if (rg2) {
                if( rg2->registerFile() == rf &&
                    rg2->registerIndex() == regIndex &&
                    rg2->isInverted() == !inv ) {
                    return new MoveGuard(*rg2);
                }
            }
        }
    }
    return NULL;
}

/**
 *
 * This method may get slow with a big machine
 *
 * Checks whether all the moves in the given programOperation are copied
 * to the given BB. If they are, also the programOperation is copied to the
 * new BB. otherwise the moves will be set to point to the original PO.
 * 
 * @param po ProgramOperation to check.
 * @movesToCopy all the moves that are copies from one BB to another
 * @return whether all Moves referenced by po are in movesToCopy.
 */
bool 
    CopyingDelaySlotFiller::wholePOMoved(
        ProgramOperation& po,  MoveNodeListVector& movesToCopy) {
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        bool found = false;
        for (unsigned int j = 0; j < movesToCopy.size(); j++) {
            std::list<MoveNode*>& moveList = movesToCopy.at(j);
            for (std::list<MoveNode*>::iterator iter = moveList.begin();
                 iter != moveList.end(); iter++) {
                if (&mn == *iter) {
                    found = true;
                    break; // avoid arguement with pekka, goto better here
                }
            }
            if (found = true) { // as with goto this if not needed
                break;
            }
        }
        if (found = false) {
            return false;
        }
    }
    return true;
}
