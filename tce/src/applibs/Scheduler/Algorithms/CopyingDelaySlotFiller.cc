/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file CopyingDelaySlotFiller.cc
 *
 * Implementation of of CopyingDelaySlotFiller class.
 *
 * @author Heikki Kultala 2007-2009 (hkultala-no.spam-cs.tut.fi)
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

#include "Move.hh"
#include "TerminalInstructionReference.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "TerminalRegister.hh"
#include "SpecialRegisterPort.hh"
#include "MoveGuard.hh"
#include "Instruction.hh"
#include "MoveNode.hh"
#include "ProgramOperation.hh"
#include "SimpleResourceManager.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "BasicBlock.hh"

#include "POMDisassembler.hh"
#include "CopyingDelaySlotFiller.hh"
#include "Program.hh"
#include "InterPassDatum.hh"
#include "InterPassData.hh"
#include "TCEString.hh"
#include "CodeGenerator.hh"
#include "TerminalFUPort.hh"
#include "Operation.hh"

//using std::set;
using std::list;
using std::vector;
using namespace TTAProgram;
using namespace TTAMachine;

IGNORE_COMPILER_WARNING("-Wstrict-overflow")

/**
 * Fill delay slots of given BB.
 *
 * @param jumpingBB BB to fill delay slots.
 * @param delaySlots number of delay slots in the machine.
 * @param fillFallThru fill from Fall-thru of jump BB?
 */
bool
CopyingDelaySlotFiller::fillDelaySlots(
    BasicBlockNode& jumpingBB, int delaySlots, bool fillFallThru) {
    // do not try to fill loop scheduled, also skip epilog and prolog.
    if (jumpingBB.isLoopScheduled()) {
        bbnStatus_[&jumpingBB] = BBN_BOTH_FILLED;
        return false;
    }

    bool cfgChanged = false;

    if (fillFallThru) {
        switch (bbnStatus_[&jumpingBB]) {
        case BBN_SCHEDULED:
            bbnStatus_[&jumpingBB] = BBN_FALLTHRU_FILLED;
            break;
        case BBN_JUMP_FILLED:
            bbnStatus_[&jumpingBB] = BBN_BOTH_FILLED;
            break;
        default:
            assert(false);
        }
    } else {
        switch (bbnStatus_[&jumpingBB]) {
        case BBN_SCHEDULED:
            bbnStatus_[&jumpingBB] = BBN_JUMP_FILLED;
            break;
        case BBN_FALLTHRU_FILLED:
            bbnStatus_[&jumpingBB] = BBN_BOTH_FILLED;
            break;
        default:
            assert(false);
        }
    }
    
    ControlFlowGraph::EdgeSet outEdges = cfg_->outEdges(jumpingBB);

    InstructionReferenceManager& irm =
        cfg_->program()->instructionReferenceManager();

    TTAProgram::BasicBlock& thisBB = jumpingBB.basicBlock();
    std::pair<int, TTAProgram::Move*> jumpData = findJump(thisBB);
    std::pair<Move*, Immediate*> jumpAddressData;

    // should be the same
    int jumpIndex = jumpData.first;
    Move* jumpMove = jumpData.second;

    // need for imm source only if filling jump, not imm.
    if (!fillFallThru) {
        // jump needed only when filling jump, not fall-thru.
        // not found?
        if (jumpMove == NULL) {
            return false;
        }

        if (!jumpMove->source().isInstructionAddress()) {
            // address comes from LIMM, search the write to limm reg.
            jumpAddressData = findJumpImmediate(jumpIndex, *jumpMove, irm);
            if (jumpAddressData.first == NULL && 
                jumpAddressData.second == NULL) {
                //Imm source not found, aborting
                return false;
            }
        } else {
            jumpAddressData.first = jumpMove;
        }
    }

    RegisterFile* grFile = NULL;
    unsigned int grIndex = 0;

    // lets be aggressive, fill more than just branch slots?
    int maxFillCount = std::min(
        delaySlots + 12,
        thisBB.instructionCount() - thisBB.skippedFirstInstructions());

    int maxGuardedFillCount = INT_MAX;

    // if we have references to instructions in target BB, cannot put anything
    // before them, so limit how many slots to fill at maximum.
    for (int i = 1 ; i < thisBB.instructionCount(); i++) {
        if (irm.hasReference(thisBB.instructionAtIndex(i))) {
            maxFillCount = std::min(maxFillCount, thisBB.instructionCount()-i);
        }
    }

    // if we have conditional jump, find the predicate register
    if (jumpMove != NULL && !jumpMove->isUnconditional()) {
        const Guard& g = jumpMove->guard().guard();
        const RegisterGuard* rg = dynamic_cast<const RegisterGuard*>(&g);
        if (rg != NULL) {
            grFile = rg->registerFile();
            grIndex = rg->registerIndex();
        } else {
            return false; // port guards not yet supported
        }

        // find when the predicate reg is written to and use that
        // to limit how many to bypass.
        MoveNode& jumpNode = ddg_->nodeOfMove(*jumpMove);
        DataDependenceGraph::NodeSet guardDefs =
            ddg_->guardDefMoves(jumpNode);
        if (guardDefs.size() != 1) {
            // many defs, don't know which is the critical one.
            // fill only slots+jump ins
            maxGuardedFillCount = delaySlots+1;
        } else {
            MoveNode& guardDefMove = **guardDefs.begin();
            if (&ddg_->getBasicBlockNode(guardDefMove) == &jumpingBB) {
                SimpleResourceManager& rm = 
                    *resourceManagers_[&jumpingBB.basicBlock()];
                
                int earliestToFill = guardDefMove.cycle() -
                    rm.smallestCycle() +
                    jumpNode.guardLatency();
                
                maxFillCount = 
                    std::min(maxFillCount, 
                             thisBB.instructionCount() - earliestToFill);
            } else {
                // guard written in previous BB?
                // make sure not filled to first insrt if
                // guard latency 2
                maxFillCount = 
                    std::min(maxFillCount,
                             thisBB.instructionCount() -
                             std::max(0,jumpNode.guardLatency()-1));
            }
        }
    } // jump conditional

    // 1 or two items, big loop. These may come in either order, grr.
    for (ControlFlowGraph::EdgeSet::iterator iter = outEdges.begin();
         iter != outEdges.end(); iter++) {
        int slotsFilled = 0;

        ControlFlowEdge& edge = **iter;
        if (edge.isFallThroughEdge() != fillFallThru) {
            continue;
        }
        if (edge.isCallPassEdge()) {
            continue;
        }

        BasicBlockNode& nextBBN = cfg_->headNode(edge);

        // cannot fill if the next is not normal BB.
        if (!nextBBN.isNormalBB() || nextBBN.isLoopScheduled()) {
            continue;
        }

        int nextInsCount = nextBBN.basicBlock().instructionCount();
        if (&nextBBN == &jumpingBB) {
            // jump to itself. may not fill from instructions 
            // which are itself being filled.
            maxFillCount = std::min(maxFillCount, nextInsCount/2);
        } else {
            // otherwise may fill all other target bb
            maxFillCount = std::min(maxFillCount,nextInsCount);
        }

        if (fillFallThru) {
            if (jumpMove != NULL) {
                // test that we can create an inverse guard
                MoveGuard* invG = CodeGenerator::createInverseGuard(
                    jumpMove->guard());
                if (invG == NULL) {
                    continue; // don't fill
                } else {
                    delete invG;
                }
            }

            // cannot remove ins if it has refs.
            // -> cannot fill fall instrs which have refs
            for (int i = 0; i < maxFillCount; i++) {
                if (irm.hasReference(
                        nextBBN.basicBlock().instructionAtIndex(i))) {
                    maxFillCount = i;
                }
            }
        }

        if (maxFillCount == 0) {
            continue;
        }

        // register copy added leaves some inter-bb edges..
        // TODO: remove when registercopyadder handles
        // inter-BB-antideps..

        // also delay slot filler itself does not create
        // all edges, at least if filling fall-thru jumps.
        // so always run the routine until fixed,
        // if-fall-thru jumps enabled.

        // temporaty solution: fix only if temp reg adder effective
//        if (!fullyConnected) {
        ddg_->fixInterBBAntiEdges(jumpingBB, nextBBN, edge.isBackEdge());
//        }

        Move* skippedJump;
        // also try to fill into jump instruction.
        // fillSize = delaySlots still adpcm-3-full-fails, should be +1
        for (int fillSize = maxFillCount; fillSize > 0; fillSize--) {
            int removeGuards = (fillSize > maxGuardedFillCount) ?
                fillSize - maxGuardedFillCount : 0;
            // then try to do the filling.
            bool ok = tryToFillSlots(
                jumpingBB, nextBBN, fillFallThru, jumpMove, fillSize,
                removeGuards, grIndex, grFile, skippedJump, delaySlots);
            if (ok) {
                slotsFilled = fillSize;
                break;
            }
        }

        // filled some slots?
        if (slotsFilled != 0) {

            // filled from jump dest or fal-thru?
            if (!fillFallThru) {
                // have to update jump destination
                cfgChanged |= updateJumpsAndCfg(
                    jumpingBB, nextBBN, edge, jumpAddressData.first,
                    jumpAddressData.second, jumpMove, slotsFilled, 
                    skippedJump);
            } else { // fall-thru, skip first instructions.
                cfgChanged |= updateFTBBAndCfg(
                    jumpingBB, nextBBN, edge, slotsFilled);
            }
        }
    }
    return cfgChanged;
}

bool CopyingDelaySlotFiller::updateFTBBAndCfg(
    BasicBlockNode& jumpingBB, BasicBlockNode& nextBBN,
    ControlFlowEdge& edge, int slotsFilled) {

    InstructionReferenceManager& irm =
        cfg_->program()->instructionReferenceManager();

    for (int i = 0; i < slotsFilled; i++) {
        assert(!irm.hasReference(
                   nextBBN.basicBlock().instructionAtIndex(i)));
    }

    if (slotsFilled == nextBBN.basicBlock().instructionCount() &&
        cfg_->inDegree(nextBBN) == 1) {
        auto oEdges = cfg_->outEdges(nextBBN);
        assert(oEdges.size() == 1);
        ControlFlowEdge& laterEdge = **oEdges.begin();
        BasicBlockNode& newDestNode = cfg_->headNode(laterEdge);

        // update CFG because jump dest moved.
        // the predicate is the same as the original
        // fall-trhough, but if later is a jump or call pass.
        // this is also a jump or call pass.
        ControlFlowEdge* newEdge =
            new ControlFlowEdge(
                edge.edgePredicate(), laterEdge.edgeType());
        cfg_->disconnectNodes(jumpingBB, nextBBN);
        cfg_->connectNodes(jumpingBB, newDestNode, *newEdge);

        cfg_->removeNode(nextBBN);
        finishBB(nextBBN, true);

        for (int i = 0;
             i < nextBBN.basicBlock().instructionCount(); i++) {
            Instruction& ins =
                nextBBN.basicBlock().instructionAtIndex(i);
            while(ins.moveCount()) {
                Move& move = ins.move(0);
                MoveNode & mn = ddg_->nodeOfMove(move);
                ddg_->removeNode(mn);
                delete &mn;
                ins.removeMove(move);
            }
            while (ins.immediateCount()) {
                Immediate& imm = ins.immediate(0);
                ins.removeImmediate(imm);
            }
        }
        delete &nextBBN;
        // cfg changed
        return true;
    } else {
        nextBBN.basicBlock().skipFirstInstructions(slotsFilled);
        // cfg not changed
        return false;
    }
}

/**
 * Constructor.
 */
CopyingDelaySlotFiller::CopyingDelaySlotFiller() {
}

/**
 *
 * Fills all delay slots for all BB's in the CFG.
 * 
 * @param cfg ControlFlowGraph where to fill delay slots.
 * @param ddg DataDependenceGraph containing data dependencies.
 */
void
CopyingDelaySlotFiller::fillDelaySlots(
    ControlFlowGraph& cfg, DataDependenceGraph& ddg,
    const TTAMachine::Machine& machine) {
    um_ = &UniversalMachine::instance();
    int delaySlots = machine.controlUnit()->delaySlots();

    cfg_ = &cfg;
    ddg_ = &ddg;

    // first fill only jumps
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            if (bbnStatus_[&bbn] == BBN_SCHEDULED ||
                bbnStatus_[&bbn] == BBN_FALLTHRU_FILLED) {
                fillDelaySlots(
                    bbn, delaySlots, false);
            }
        }
    }

    // then fill only fall-thru's.
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            if (bbnStatus_[&bbn] == BBN_JUMP_FILLED) {
                fillDelaySlots(
                    bbn, delaySlots, true);
            }
        }
    }

    // All is done. Then get rid of data which is no longer needed.
    for (std::map<BasicBlockNode*, BBNStates>::iterator i = 
             bbnStatus_.begin(); i != bbnStatus_.end(); i++) {
        if (i->second != BBN_ALL_DONE) {
            // todo: why is true required here
            finishBB(*i->first); //, true);
        }
    }
    // TODO: why is this not empty? finishBB should clear these.
    for (std::map<BasicBlock*, SimpleResourceManager*>::iterator i =
             resourceManagers_.begin(); i != resourceManagers_.end(); i++) {
        if (i->second != NULL) {
            SimpleResourceManager::disposeRM(i->second);
        }
    }
    resourceManagers_.clear();
//    assert(resourceManagers_.empty());
    tempResultNodes_.clear();
}

/**
 * Deletes all removed basic blocks. Can be called after bigddg is deleted.
 */
void CopyingDelaySlotFiller::finalizeProcedure() {
    AssocTools::deleteAllItems(killedBBs_);
}

/**
 * Checks if all jumps into given basic blocks are filled.
 *
 * @param bbn node to check for incoming filled jumps.
 * @return true if all incoming jumps are already filled.
*/
bool CopyingDelaySlotFiller::areAllJumpPredsFilled(BasicBlockNode& bbn) const {
    bool allJumpPredsFilled = true;
    ControlFlowGraph::EdgeSet inEdges = cfg_->inEdges(bbn);
    for (ControlFlowGraph::EdgeSet::iterator inIter =
             inEdges.begin(); inIter != inEdges.end();
         inIter++) {
        ControlFlowEdge& cfe = **inIter;
        if (cfe.isJumpEdge()) {
            BasicBlockNode& jumpOrigin = cfg_->tailNode(cfe);

            // if some jump to the ft-node has not been scheduled.
            if (jumpOrigin.isNormalBB() &&
                (bbnStatus_[&jumpOrigin] < BBN_JUMP_FILLED ||
                 bbnStatus_[&jumpOrigin] == BBN_FALLTHRU_FILLED)) {
                allJumpPredsFilled = false;
            }
        }
    }
    return allJumpPredsFilled;
}

/**
 * Checks whether all incoming jump basic blocks are scheduled.
 *
 * @param bbn basic block whose predecessors we are checking.
 * @return true if all BB's which jump into given BB are scheduled.
 */
bool
CopyingDelaySlotFiller::areAllJumpPredsScheduled(BasicBlockNode& bbn) const {
    bool allPredsScheduled = true;
    ControlFlowGraph::EdgeSet inEdges = cfg_->inEdges(bbn);
    for (ControlFlowGraph::EdgeSet::iterator inIter =
             inEdges.begin(); inIter != inEdges.end();
         inIter++) {
        ControlFlowEdge& cfe = **inIter;
        if (cfe.isJumpEdge()) {
            BasicBlockNode& jumpOrigin = cfg_->tailNode(cfe);

            // if some jump to the ft-node has not been scheduled.
            if (bbnStatus_[&jumpOrigin] == BBN_UNKNOWN &&
                jumpOrigin.isNormalBB()) {
                allPredsScheduled = false;
            }
        }
    }
    return allPredsScheduled;
}

/**
 * Executed after delay slots of a BB has been filled.
 *
 * If both jump and fal-thru are filled, reletes the resource manager.
 */
void
CopyingDelaySlotFiller::bbFilled(BasicBlockNode& bbn) {
    if (bbnStatus_[&bbn] == BBN_BOTH_FILLED) {
        finishBB(bbn);
    }
}

/**
 * Checks if jumps and fall-thrus into BB can be filled and fills them
 * if it can
 *
 * @param bbn just destination basic block.
 * @return if anything was filled.
 */
bool CopyingDelaySlotFiller::mightFillIncomingTo(BasicBlockNode& bbn) {
    if (bbnStatus_[&bbn] == BBN_UNKNOWN) {
        return false;
    }
    if (!areAllJumpPredsScheduled(bbn)) {
        return false;
    }

    bool cfgChanged = false;
    bool cfgChangedAfterRecheck = false;

    ControlFlowGraph::EdgeSet jumpEdges = cfg_->incomingJumpEdges(bbn);

    // first fill all incoming jumps.
    for (auto inIter = jumpEdges.begin(); inIter != jumpEdges.end();) {
        ControlFlowEdge& cfe = **inIter;
        BasicBlockNode& jumpOrigin = cfg_->tailNode(cfe);

        if (jumpOrigin.isNormalBB() &&
            (bbnStatus_[&jumpOrigin] == BBN_SCHEDULED ||
             bbnStatus_[&jumpOrigin] == BBN_FALLTHRU_FILLED)) {

            bool changed = fillDelaySlots(jumpOrigin, delaySlots_, false);
            cfgChanged |= changed;
            cfgChangedAfterRecheck |= changed;

            bbFilled(jumpOrigin);

            BasicBlockNode* fallThruSisterNode =
                cfg_->fallThruSuccessor(jumpOrigin);

            // so we have some left-behind sister NODE
            if (fallThruSisterNode != NULL &&
                bbnStatus_[fallThruSisterNode] > BBN_UNKNOWN &&
                bbnStatus_[&jumpOrigin] < BBN_TEMPS_CLEANED &&
                areAllJumpPredsFilled(*fallThruSisterNode)) {
                changed = fillDelaySlots(jumpOrigin, delaySlots_, true);
                cfgChanged |= changed;
                cfgChangedAfterRecheck |= changed;
                bbFilled(jumpOrigin);
                // may have been removed totally.
            }

            // If we changed the cfg, loafd the inedges again.
            if (cfgChangedAfterRecheck) {
                if (!cfg_->hasNode(bbn)) {
                    return true;
                }
                cfgChangedAfterRecheck = false;
                jumpEdges = cfg_->incomingJumpEdges(bbn);
                inIter = jumpEdges.begin();
                continue;
            }
        }
        inIter++;
    }

    // No Fts to fill if the whole Basic Block is gone!
    if (cfgChanged && !cfg_->hasNode(bbn)) {
        return true;
    }

    // then all incoming jumps should be filled, fall-throughs can fill
    auto ftEdge = cfg_->incomingFTEdge(bbn);
    // may still be call pass, which is not allowed.
    if (ftEdge != nullptr && ftEdge->isFallThroughEdge()) {
        BasicBlockNode& ftOrigin = cfg_->tailNode(*ftEdge);

        // fall thru-predecessor can be filled if it's scheduled.
        if (bbnStatus_[&ftOrigin] == BBN_JUMP_FILLED) {
            cfgChanged |= fillDelaySlots(ftOrigin, delaySlots_, true);
            bbFilled(ftOrigin);
        } else {
            // do not take space from backwards jumps, ie loop jumps.
            if (bbnStatus_[&ftOrigin] == BBN_SCHEDULED) {
                BasicBlockNode* jumpSisterNode =
                    cfg_->jumpSuccessor(bbn);
                if (jumpSisterNode != NULL &&
                    jumpSisterNode->originalStartAddress() >
                    bbn.originalStartAddress() &&
                    areAllJumpPredsFilled(*jumpSisterNode)) {

                    cfgChanged |= fillDelaySlots(ftOrigin, delaySlots_, true);
                    bbFilled(ftOrigin);
                }
            }
        }
    }
    return cfgChanged;
}

/**
 * Report to the ds filler that a bb has been scheduled.
 * tries to fill some delay slots and then tries to get rid of the
 * resource managers when no longer needed.
 *
 * @param bbn BasicBlockNode which has been scheduled.
 */
void
CopyingDelaySlotFiller::bbnScheduled(BasicBlockNode& bbn) {

    assert(bbnStatus_[&bbn] == BBN_UNKNOWN);
    // if we don't have the RM got the bb, cannot really do much with it.
    if (resourceManagers_.find(&bbn.basicBlock())==resourceManagers_.end()) {
        return;
    }

    bbnStatus_[&bbn] = BBN_SCHEDULED;

    // all successors.
    ControlFlowGraph::EdgeSet oEdges = cfg_->outEdges(bbn);

    bool fillableSuccessor = false;
    for (ControlFlowGraph::EdgeSet::iterator outIter = oEdges.begin();
         outIter != oEdges.end();) {
        ControlFlowEdge& cfe = **outIter;

        // cannot fill calls.
        if (!cfe.isCallPassEdge()) {
            fillableSuccessor = true;
            auto& head = cfg_->headNode(cfe);
            if (mightFillIncomingTo(head)) {
                // the filling might have removed the BBN.
                if (!cfg_->hasNode(bbn)) {
                    return;
                }
                oEdges = cfg_->outEdges(bbn);
                outIter = oEdges.begin();
                continue;
            }
        }
        outIter++;
    }

    if (!fillableSuccessor && bbnStatus_[&bbn] != BBN_ALL_DONE) {
        finishBB(bbn);
    }

    // can fill incoming jumps if all incoming BBs scheduled.
    mightFillIncomingTo(bbn);
}


/*
 * Adds a resource manager to the filler.
 *
 * @param bb Basic block which the resource manager handles.
 * @param rm Resource manager or the bb.
 */
void
CopyingDelaySlotFiller::addResourceManager(
    BasicBlock& bb,SimpleResourceManager& rm) {
    resourceManagers_[&bb] = &rm;
    rm.setMaxCycle(INT_MAX);
}

/**
 * Finds the immediate which contains the jump address.
 *
 * @param jumpIndex index of the instruction containin the jump in the BB.
 * @param jumpMove the move containing the jump.
 * @return Immediate containing jump address or NULL if not found.
 */
std::pair<TTAProgram::Move*, TTAProgram::Immediate*>
CopyingDelaySlotFiller::findJumpImmediate(
    int jumpIndex, Move& jumpMove, InstructionReferenceManager& irm) {
    BasicBlock& bb = static_cast<BasicBlock&>(jumpMove.parent().parent());
    Move* irMove = &jumpMove;
    int irIndex = jumpIndex;

    // Register copy or some unknown source.
    // should handle unlimited number of reg. copies.
    // Note: the jump address can be written outside the BB
    // if it's an indirect branch (e.g. LLVM address of label
    // extension).
    if (irMove->source().isGPR()) {
        const RegisterFile* rf = &irMove->source().registerFile();
        int regIndex = static_cast<int>(irMove->source().index());
        int found = false;
        int i;
        // if has refernces, the imm may be written in another BB.
        // we can not (yet) track that!
        if (irm.hasReference(bb.instructionAtIndex(irIndex))) {
            return std::pair<Move*,Immediate*>(NULL, NULL);
        }
        for (i = irIndex -1 ; i >= bb.skippedFirstInstructions() && !found; 
             i-- ) {
            Instruction &ins = bb.instructionAtIndex(i);
            // if has refernces, the imm may be written in another BB.
            // we can not (yet) track that!
            if (irm.hasReference(ins)) {
                return std::pair<Move*,Immediate*>(NULL, NULL);
            }
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
        // jump imm not found.
        if (i < bb.skippedFirstInstructions() && !found) {
            return std::pair<Move*,Immediate*>(NULL, NULL);
        }
    }
    if (irMove->source().isImmediate()) {
        return std::pair<Move*,Immediate*>(irMove, NULL);
    }

    // then read the actual immediate
    if (irMove->source().isImmediateRegister()) {
        const ImmediateUnit& immu = irMove->source().immediateUnit();
        int index = static_cast<int>(irMove->source().index());
        for (int i = irIndex -1; i >= 0; i--) {
            Instruction &ins = bb.instructionAtIndex(i);
            for (int j = 0; j < ins.immediateCount(); j++) {
                Immediate& imm = ins.immediate(j);
                if (imm.destination().index() == index &&
                    &imm.destination().immediateUnit() == &immu) {
                    return std::pair<Move*,Immediate*>(NULL, &imm);
                }
            }
            // if has refernces, the imm may be written in another BB.
            // we can not (yet) track that!
            if (irm.hasReference(ins)) {
                return std::pair<Move*,Immediate*>(NULL, NULL);
            }
        }
    }
    return std::pair<Move*,Immediate*>(NULL, NULL);
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
 * @param removeGuards how many first instructions need guard removed.
 * @param grIndex index of the guard register of the jump
 * @param grFile register file of the guard of the jump.
 * @param skippedJump if we can skip BB which jumps, sets the skipped jump here
 * @return of fill succeeded, false if failed.
 */
bool
CopyingDelaySlotFiller::tryToFillSlots(
    BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBNode, bool fallThru,
    Move* jumpMove, int slotsToFill, int removeGuards, int grIndex,
    RegisterFile* grFile, Move*& skippedJump, int delaySlots) {
    skippedJump = NULL;
    BasicBlock& blockToFill = blockToFillNode.basicBlock();
    SimpleResourceManager& rm = *resourceManagers_[&blockToFill];
    BasicBlock& nextBB = nextBBNode.basicBlock();
    SimpleResourceManager* nextRm = resourceManagers_[&nextBB];

    if (nextRm == NULL) {
        return false;
    }

    int firstCycleToFill = 
        rm.smallestCycle() + blockToFill.instructionCount() - slotsToFill;
    int nextBBStart = nextRm->smallestCycle();
    
    MoveNodeListVector moves(slotsToFill);

    // Collect all the moves to fill.
    if (!collectMoves(
            blockToFillNode, nextBBNode, moves, slotsToFill,fallThru, 
            removeGuards, jumpMove, grIndex, grFile, skippedJump,
            delaySlots)) {
        loseCopies(NULL);
        return false;
    }

    //check imm reads after (safeguard against same imm read twice)
    if (!checkImmediatesAfter(nextBB, slotsToFill)) {
        loseCopies(NULL);
        return false;
    }
    
    // now we have got all the movenodes we are going to fill.
    // then try to assign them to the block where they are being filled to.

    // set containing po result moves etc.
    DataDependenceGraph::NodeSet tempAssigns;
    if (tryToAssignNodes(moves, slotsToFill, firstCycleToFill,rm, nextBBStart, 
                         tempAssigns)) {
        // we succeeded. 
        // delete temporaty copies of other movenodes.
        loseCopies(&tempAssigns);

        AssocTools::append(tempAssigns, tempResultNodes_[&blockToFillNode]);
        return true;
    } else {
        loseCopies(&tempAssigns);
        unassignTempAssigns(tempAssigns, rm);
        // failing. delete created movenodes and report failure
        return false;
    }
}

/**
 * Collects are moves which are to be filled
 * 
 * @param blockToFillNode basic block node where to fill to
 * @param nextBBN basic blocknode where to fill from
 * @param moves place to store the collected nodes.
 * @param slotsToFill how many delay slots to fill
 * @param fallThru if filling fall-thru
 * @param removeGuards how many first instructions need guard removed.
 * @param jumpMove move which is the jump that is filled
 * @param grIndex index of the guard register of the jump
 * @param grFile register file of the guard of the jump.
 * @param skippedJump if we can skip BB which jumps, sets the skipped jump here
 *
 * @return true if collecting ok, false if failed
 */
bool
CopyingDelaySlotFiller::collectMoves(
    BasicBlockNode& blockToFillNode, BasicBlockNode& nextBBN,
    MoveNodeListVector& moves, int slotsToFill, bool fallThru,
    int removeGuards,
    Move* jumpMove, int grIndex, TTAMachine::RegisterFile* grFile, 
    Move*& skippedJump, int delaySlots) {
    PendingImmediateMap pendingImmediateMap;

    ControlFlowEdge* connectingEdge =
        *cfg_->connectingEdges(blockToFillNode, nextBBN).begin();
    BasicBlock& bb = blockToFillNode.basicBlock();
    BasicBlock& nextBB = nextBBN.basicBlock();
    SimpleResourceManager& rm = *resourceManagers_[&bb];
    SimpleResourceManager& nextRm = *resourceManagers_[&nextBB];

    int firstIndexToFill = 
        blockToFillNode.basicBlock().instructionCount() - slotsToFill;

    int cycleDiff = firstIndexToFill + rm.smallestCycle() - nextRm.smallestCycle();

    // Find all moves to copy and check their dependencies.
    // Loop thru instructions first..
    for (int i = 0; i < slotsToFill; i++) {
        Instruction& filler = nextBB.instructionAtIndex(i);
        if (filler.hasCall()) {
            return false;
        }
        // loop thru all moves in the instr
        for (int j = 0; j < filler.moveCount(); j++) {
            Move& oldMove = filler.move(j);
            bool dontGuard = i < removeGuards;
            // may fill jump if fills the whole BB and updates the jump.
            if (oldMove.isJump()) {
                if (!oldMove.isUnconditional()) {
                    // TODO: if first was uncond jump, and filling whole BB, 
                    // this could be allowed.
                    return false;
                }
                if (fallThru) {
                    // Allow another jump only to same instruction
                    // than where the filled jump is.
                    if (i != slotsToFill - delaySlots -1) {
                        return false;
                    }
                } else {
                    if (slotsToFill != nextBB.instructionCount() || 
                        // TODO: find a way to find the source imm of this jump
                        // even if LIMM or tempregcopy.
                        // then no need to abort here.
                        oldMove.source().isGPR() ||
                        oldMove.source().isImmediateRegister()) {
                        return false;
                    }
                    skippedJump = &oldMove;
                    continue; // can be skipped
                }
            }

            // check if it overwrites the guard of the jump
            if (writesRegister(oldMove, grFile, grIndex)) {
                // overwriting the guard reg as last thing does not matter,
                // only allow it in last imported instruction.
                if (i != slotsToFill-1) {
                    return false;
                }
            }
            
            // check all deps
            MoveNode& mnOld = ddg_->nodeOfMove(oldMove);
            if (!oldMove.isUnconditional()) {
                // Do not fill with guarded moves, if jump is guarded,
                // might need 2 guards.
                // Only allowed if removing the guards anyway.
                if (grFile != NULL) {
                    dontGuard = true;
                }
                // don't allow unconditionals before
                // BB start + guard latency (if guard written in last move
                // of previous BB)
                if (firstIndexToFill < mnOld.guardLatency()-1) {
                    return false;
                }
            }

            // if a move has no side effects, the guard can be omitted
            // and it can be scheduled before the guard is ready.
            if (dontGuard) {
                // TODO: allow even writes if the reg is not alive?
                if (oldMove.destination().isGPR()) {
                    return false;
                }

                if (oldMove.isTriggering()) {
                    Operation& o = oldMove.destination().operation();
                    if (o.writesMemory() || o.hasSideEffects() ||
                        o.affectsCount() != 0) {
                        return false;
                    }

                    // also may not read memory, because the address may be invalid,
                    // if the address comes with bypass from operation with different guard.
                    if (o.readsMemory()) {
                        if (mnOld.isSourceOperation() &&
                            mnOld.sourceOperation().triggeringMove()->move().isUnconditional() &&
                            i >= removeGuards) {
                            return false;
                        }
                        // and if the address comes from a variable.
                        if (mnOld.isSourceVariable()) {
                            return false;
                        }
                    }
                }
            }

            // check DDG that no data hazards.
            if (!checkIncomingDeps(mnOld, blockToFillNode, cycleDiff)) {
                return false;
            }

            // also copies move
            MoveNode& mn = getMoveNode(
                mnOld, blockToFillNode, connectingEdge->isBackEdge());
            Move& newMove = mn.move();

            // reads immediate?
            if (newMove.source().isImmediateRegister()) {
                TTAProgram::Terminal& src = newMove.source();
                TCEString immName = 
                    src.immediateUnit().name() + '.'
                    + Conversion::toString(src.index());
                
                PendingImmediateMap::iterator immWriteIter =
                    pendingImmediateMap.find(immName);

                // if no write to the imm reg found, fail.
                if (immWriteIter == pendingImmediateMap.end()) {
                    return false;
                }
                newMove.setSource(immWriteIter->second->copy());
                pendingImmediateMap.erase(immWriteIter);
            }

            if (jumpMove != NULL && !jumpMove->isUnconditional() &&
                !dontGuard) {
                
                if (fallThru) {
                    newMove.setGuard(CodeGenerator::createInverseGuard(
                                         jumpMove->guard()));
                } else {
                    newMove.setGuard(jumpMove->guard().copy());
                }
                MoveNode& jumpNode = ddg_->nodeOfMove(*jumpMove);
                ddg_->copyIncomingGuardEdges(jumpNode, mn);
            }
            moves.at(i).push_back(&mn);
        } // move-loop

        for (int j = 0; j < filler.immediateCount(); j++) {
            if (&blockToFillNode == &nextBBN) {
                return false;
            }

            TTAProgram::Immediate& imm = filler.immediate(j);
            const TTAProgram::Terminal& dst = imm.destination();
            TCEString immName = 
                dst.immediateUnit().name() + '.' + 
                Conversion::toString(dst.index());

            if (AssocTools::containsKey(pendingImmediateMap,immName)) {
                // there already is write to this imm reg without
                // a read to it? something is wrong, abort
                return false;
            }
            pendingImmediateMap[immName] = &imm.value();
        }
    }

    // make sure long guard latencies dont cause trouble with following instructions.
    if (nextBB.instructionCount() > slotsToFill) {
        Instruction& firstNotToFill = nextBB.instructionAtIndex(slotsToFill);
        // loop thru all moves in the instr
        for (int j = 0; j < firstNotToFill.moveCount(); j++) {
            Move& move = firstNotToFill.move(j);
            if (move.isUnconditional()) {
                break;
            }
	    
            MoveNode& mn = ddg_->nodeOfMove(move);
	    
            // check DDG that no data hazards.
            if (!checkIncomingDeps(mn, blockToFillNode, cycleDiff)) {
                return false;
            }
        }
    }

    // ok if no pending immediates (ie limm written, not read)
    return pendingImmediateMap.empty();
}

/**
 * Checks that no later uses of long immediates written in instructions
 * which are being filled to previous basic block.
 *
 * @param nextBB The BB where to instructions are filled from
 * @param slotsToFill how many slots are filled
 *
 * @return true if everything ok, false if immediates prevent filling
 */
bool
CopyingDelaySlotFiller::checkImmediatesAfter(
    BasicBlock& nextBB, int slotsToFill) {
    //check imm reads after (safeguard against same imm read twice)
    PendingImmediateMap pendingImmediateMap;
    
    for (int i = slotsToFill; i < nextBB.instructionCount(); i++) {
        Instruction& filler = nextBB.instructionAtIndex(i);
        for (int j=0; j < filler. moveCount(); j++) {
            Move& move = filler.move(j);
            if (move.source().isImmediateRegister()) {
                TTAProgram::Terminal& src = move.source();
                TCEString immName = 
                    src.immediateUnit().name() + '.'
                    + Conversion::toString(src.index());
                
                PendingImmediateMap::iterator immWriteIter =
                    pendingImmediateMap.find(immName);

                if (immWriteIter == pendingImmediateMap.end()) {
                    // read from immediate reg which ahs not been written
                    // after the filled instructions, ie the limm
                    // is at the instructions which are filled. do not allow 
                    return false;
                }
                pendingImmediateMap.erase(immWriteIter);
            }
        } // move loop

        for (int j = 0; j < filler.immediateCount(); j++) {
            TTAProgram::Immediate& imm = filler.immediate(j);
            const TTAProgram::Terminal& dst = imm.destination();
            TCEString immName = 
                dst.immediateUnit().name() + '.' + 
                Conversion::toString(dst.index());
            if(pendingImmediateMap.find(immName) != pendingImmediateMap.end()) {
                // same imm written twice without reading it. something wrong.
                return false;
            }
            pendingImmediateMap[immName] = &imm.value();
        } // imm loop
    } // instruction-loop

    // if immediates unbalanced at end of the bb. something wrong.
    if (!pendingImmediateMap.empty()) {
        return false;
    }
    return true;
}

/**
 * Checks that no incoming deps prevent the filling, ie. no data hazards.
 *
 * @param mnOld movenode which to fill from successor bb
 * @param blockToFillNode block containing the jump being filled.
 * @param firstCycleToFill index of first instruction where to fill
 * @return true if no data hazards, false if cannot fill.
 */
bool
CopyingDelaySlotFiller::checkIncomingDeps(
    MoveNode& mnOld, BasicBlockNode& blockToFillNode, int cycleDiff) {
    DataDependenceGraph::EdgeSet inEdges = ddg_->inEdges(mnOld);
    for (DataDependenceGraph::EdgeSet::iterator ieIter =
             inEdges.begin(); ieIter != inEdges.end(); ieIter++) {
        int mnCycle = mnOld.cycle();
        // slow
        DataDependenceEdge& ddEdge = **ieIter;
        MoveNode& pred = ddg_->tailNode(ddEdge);
        if (pred.isMove()) {
            BasicBlockNode& predBlock = ddg_->getBasicBlockNode(pred);
            if (!pred.isScheduled()) {
                continue;
            }
            
            if (&predBlock == &blockToFillNode) {
                int nodeCycle;
                int delay = 1;
                // guard latency.
                if (ddEdge.dependenceType() ==
                    DataDependenceEdge::DEP_WAR) {
                    if (ddEdge.guardUse()) {
                        delay = pred.guardLatency();
                    }
                    nodeCycle = pred.cycle() - delay+1;
                } else {
                    // if WAW, always 1
                    if (ddEdge.dependenceType() !=
                        DataDependenceEdge::DEP_WAW) {
                        if (ddEdge.guardUse()) {
                            delay = mnOld.guardLatency();
                        }
                    }
                    nodeCycle = pred.cycle()+delay;
                }
                if (nodeCycle > cycleDiff+mnCycle) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Assigns moves which have been copied to the succeeding basic block
 * into instructions in the basic block which is being filled.
 *
 * @param moves moves to assign
 * @param slotsToFill number of instructions to fill
 * @param firstCycleToFill cycle where to fill first instruction
 * @param rm resourcemanager to the bb being filled to.
 * @return true if the filling succeeded, false if failed.
 */
bool
CopyingDelaySlotFiller::tryToAssignNodes(
    MoveNodeListVector& moves,
    int slotsToFill, int firstCycleToFill, ResourceManager& rm,
    int nextBBStart, DataDependenceGraph::NodeSet& tempAssigns) {
    bool failed = false;

    // then try to assign the nodes
    for (unsigned int i = 0; i < (unsigned)slotsToFill && 
             i < moves.size() && !failed; i++) {
        list<MoveNode*>& movesInThisCycle = moves.at(i);
        int currentCycle = firstCycleToFill + i;

        for (std::list<MoveNode*>::iterator iter = movesInThisCycle.begin();
             iter != movesInThisCycle.end() && !failed; iter++) {
            MoveNode& mn = **iter;

            if (!mn.isScheduled()) {
                
                // if cannot assign move, immediate fail.
                if (!rm.canAssign(currentCycle, mn)) {
                    failed = true;
                    break;
                }
                
                rm.assign(currentCycle, mn);
            } else {
                if (mn.cycle() != currentCycle) {
                    failed = true;
                    break;
                }
            }
            assert(mn.isScheduled());
            assert(mn.cycle() == currentCycle);
            
            if (mn.move().source().isImmediateRegister()) {
                bool limmFound = false;
                for (int j = (firstCycleToFill+i-1); 
                     j >= firstCycleToFill && !limmFound; j--) {
                    const Instruction* ins = rm.instruction(j);
                    for (int k = 0; k < ins->immediateCount(); k++) {
                        Immediate& imm = ins->immediate(k);
                        if (imm.destination().equals(mn.move().source())) {
                            limmFound = true;
                            break;
                        }
                    }
                }
                // limm too early?
                if (!limmFound) {
                    failed = true;
                    break;
                }
            }

            // if not destination operand, no need to check
            // other moves of the operation.
            if (!mn.isDestinationOperation()) {
                continue;
            }

            // check that result and other operand scheduling is possible
            // if not, this can not be scheuduled either.
            // even though alone would be possible
            if (!tryToAssignOtherMovesOfDestOps(
                    mn, firstCycleToFill, rm, 
                    firstCycleToFill + (slotsToFill-1), nextBBStart,
                    tempAssigns)) {
                failed = true;
                break;
            }
        }
    }
    if (failed) {

        // and also movenodes assigned to delay slots.
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
        return false;
    }
    return true;
}

/**
 * Checks that all other moves of an operation can be scheduled
 * at same relative cycles than they were in the another BB.
 */
bool
CopyingDelaySlotFiller::tryToAssignOtherMovesOfDestOps(
    MoveNode& mn, int firstCycleToFill, ResourceManager& rm, int lastCycle,
    int nextBBStart, DataDependenceGraph::NodeSet& tempAssigns) {

    for (unsigned int i = 0; i < mn.destinationOperationCount(); i++) {
        ProgramOperation& po = mn.destinationOperation(i);
        if (!tryToAssignOtherMovesOfOp(po, firstCycleToFill, rm, lastCycle,
                                       nextBBStart, tempAssigns)) {
            return false;
        }
    }
    return true;
}
bool
CopyingDelaySlotFiller::tryToAssignOtherMovesOfOp(
    ProgramOperation& po, int firstCycleToFill, ResourceManager& rm, int lastCycle,
    int nextBBStart, DataDependenceGraph::NodeSet& tempAssigns) {

    // first inputs.
    if (!po.areInputsAssigned()) {
        for (int j = 0; j < po.inputMoveCount(); j++) {
            MoveNode& operMn = po.inputMove(j);
            // schedule only those not scheduled
            if (!operMn.isScheduled()) {
                int mnCycle =
                    firstCycleToFill +
                    oldMoveNodes_[&operMn]->cycle() -
                    nextBBStart;
                assert(operMn.isDestinationOperation());
                
                if (!rm.canAssign(mnCycle, operMn)) {
                    return false;
                } else {
                    rm.assign(mnCycle, operMn);
                    // need to be removed at end
                    if (mnCycle > lastCycle) {
                        tempAssigns.insert(&operMn);
                    }
                }
            }
        } // end for
    }

    // then outputs.
    for (int j = 0; j < po.outputMoveCount(); j++) {
        MoveNode& resMn = po.outputMove(j);
        if (!resMn.isScheduled()) {
            int mnCycle =
                firstCycleToFill + oldMoveNodes_[&resMn]->cycle() - 
                nextBBStart;
            assert(resMn.isSourceOperation());
            if (!rm.canAssign(mnCycle, resMn)) {
                return false;
                
            } else {
                rm.assign(mnCycle, resMn);
                if (mnCycle > lastCycle) {
                    tempAssigns.insert(&resMn);
                }
            }
        }
    }
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
CopyingDelaySlotFiller::getMoveNode(
    MoveNode& old, BasicBlockNode& bbn, bool fillOverBackEdge) {
    if (AssocTools::containsKey(moveNodes_,&old)) {
        return *moveNodes_[&old];
    } else {
        auto movePtr = getMove(old.move());
        MoveNode *newMN = new MoveNode(movePtr);
        ddg_->addNode(*newMN, bbn);
        ddg_->copyDependencies(old,*newMN, fillOverBackEdge);

        moveNodes_[&old] = newMN;
        oldMoveNodes_[newMN] = &old;
        mnOwned_[newMN] = true;
        if (old.isSourceOperation()) {
            newMN->setSourceOperationPtr(
                getProgramOperationPtr(
                    old.sourceOperationPtr(), bbn, fillOverBackEdge));
            assert(newMN->isSourceOperation());
        }
        if (old.isDestinationOperation()) {
            for (unsigned int i = 0; i < old.destinationOperationCount(); i++) {
                newMN->addDestinationOperationPtr(
                    getProgramOperationPtr(
                        old.destinationOperationPtr(i), bbn, fillOverBackEdge));
            }
            assert(newMN->isDestinationOperation());
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
ProgramOperationPtr
CopyingDelaySlotFiller::getProgramOperationPtr(
    ProgramOperationPtr old, BasicBlockNode& bbn, bool fillOverBackEdge) {
    if (AssocTools::containsKey(programOperations_, old.get())) {
        return programOperations_[old.get()];
    } else {
        ProgramOperationPtr po = 
            ProgramOperationPtr(
                new ProgramOperation(old->operation()));
        programOperations_[old.get()] = po;
        oldProgramOperations_[po.get()] = old;
        for (int i = 0; i < old->inputMoveCount();i++) {
            MoveNode& mn = old->inputMove(i);
            assert(mn.isDestinationOperation());
            po->addInputNode(getMoveNode(mn, bbn, fillOverBackEdge));
        }
        for (int j = 0; j < old->outputMoveCount();j++) {
            MoveNode& mn = old->outputMove(j);
            assert(mn.isSourceOperation());
            po->addOutputNode(getMoveNode(mn,bbn, fillOverBackEdge));
        }
        return po;
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
std::shared_ptr<Move>
CopyingDelaySlotFiller::getMove(Move& old) {
    if (AssocTools::containsKey(moves_,&old)) {
        return moves_[&old];
    } else {
        MoveNode& oldMN = ddg_->nodeOfMove(old);

        auto newMove = old.copy();
        newMove->setBus(um_->universalBus());

        Terminal& source = newMove->source();
        if (oldMN.isSourceOperation()) {
            assert(source.isFUPort());
            std::string fuName = source.functionUnit().name();
	    //TODO: which is the correct annotation here?
            TTAProgram::ProgramAnnotation srcUnit(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
                fuName);
            newMove->setAnnotation(srcUnit);

            const Operation &srcOp = oldMN.sourceOperation().operation();
            HWOperation& hwop = *um_->universalFunctionUnit().operation(
                srcOp.name());
            newMove->setSource(new TerminalFUPort(
                                   hwop, old.source().operationIndex()));
        } else {
            if (source.isRA()) {
                newMove->setSource(
                    new TerminalFUPort(
                        *um_->controlUnit()->returnAddressPort()));
            }
        }

        Terminal& dest = newMove->destination();
        if (oldMN.isDestinationOperation()) {
            assert(dest.isFUPort());

            std::string fuName = dest.functionUnit().name();
	    //TODO: which is the correct annotation here?
            TTAProgram::ProgramAnnotation dstUnit(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST, 
                fuName);
            newMove->setAnnotation(dstUnit);

            const Operation &dstOp = oldMN.destinationOperation().operation();
            HWOperation& hwop = *um_->universalFunctionUnit().operation(
                dstOp.name());
            newMove->setDestination(new TerminalFUPort(
                                   hwop, old.destination().operationIndex()));
        } else {
            if (dest.isRA()) {
                newMove->setDestination(
                    new TerminalFUPort(
                        *um_->controlUnit()->returnAddressPort()));
            }
        }


        moves_[&old] = newMove;

        // set guard of new move to be same as old move.
        if (!old.isUnconditional()) {
            TTAProgram::MoveGuard* g = old.guard().copy();
            newMove->setGuard(g);
        }
        return newMove;
    }
}

/**
 * Deletes MoveNodes and programOperations not put into final graph.
 *
 * Loses all bookkeeping of corresponging stuff between BB's
 */
void CopyingDelaySlotFiller::loseCopies(
    DataDependenceGraph::NodeSet* keptTempNodes) {

    std::list<MoveNode*> toDeleteNodes;
    for (std::map<MoveNode*,MoveNode*,MoveNode::Comparator>::iterator mnIter =
             moveNodes_.begin(); mnIter != moveNodes_.end();
         mnIter++ ) {
        MoveNode* second = mnIter->second;
        if (mnOwned_[second] == true) {
            mnOwned_.erase(second);

            if ((keptTempNodes == NULL || 
                 (keptTempNodes->find(second) == keptTempNodes->end()))
                && !second->isScheduled()) {
                // queue to be deleted
                toDeleteNodes.push_back(second);
            }
        }
    }
    moveNodes_.clear();
    mnOwned_.clear();
    oldMoveNodes_.clear();

    // actually delete the movenodes.
    for (std::list<MoveNode*>::iterator i = toDeleteNodes.begin();
         i != toDeleteNodes.end(); i++) {
        assert (!(*i)->isScheduled());
        ddg_->removeNode(**i);
        delete *i;
    }

    moves_.clear();

    programOperations_.clear();
    oldProgramOperations_.clear();
}

/**
 * Destructor.
 */
CopyingDelaySlotFiller::~CopyingDelaySlotFiller() {
    assert(programOperations_.size() == 0);
    assert(moveNodes_.size() == 0);
    assert(mnOwned_.size() == 0);
    assert(moves_.size() == 0);

    //should not be needed but lets be sure
//    loseCopies();
}


/**
 *
 * This method may get slow with a big machine
 *
 * Checks whether some of the moves in the given programOperation is copied
 * to the given BB. If they are, also the programOperation is copied to the
 * new BB.
 * 
 * @param po ProgramOperation to check.
 * @movesToCopy all the moves that are copies from one BB to another
 * @return whether some of Moves referenced by po are in movesToCopy.
 */
bool 
CopyingDelaySlotFiller::poMoved(
    ProgramOperationPtr po,  MoveNodeListVector& movesToCopy,
    DataDependenceGraph::NodeSet& tempAssigns) {
    for (int i = 0; i < po->inputMoveCount(); i++) {
        MoveNode& mn = po->inputMove(i);
        if (tempAssigns.find(&mn) != tempAssigns.end()) {
            return true;
        }
        for (unsigned int j = 0; j < movesToCopy.size(); j++) {
            std::list<MoveNode*>& moveList = movesToCopy.at(j);
            for (std::list<MoveNode*>::iterator iter = moveList.begin();
                 iter != moveList.end(); iter++) {
                if (&mn == *iter) {
                    return true;
                }
            }
        }
    }
    for (int i = 0; i < po->outputMoveCount(); i++) {
        MoveNode& mn = po->outputMove(i);
        if (tempAssigns.find(&mn) != tempAssigns.end()) {
            return true;
        }
    }
    return false;
}

/**
 * Finds the jump move and the index of the instruction where it is
 * from a basic block.
 *
 * @param bb basicBlock where to search the jump move
 */

std::pair<int, TTAProgram::Move*>
CopyingDelaySlotFiller::findJump(
    TTAProgram::BasicBlock& bb, ControlFlowEdge::CFGEdgePredicate* pred) {

    for (int i = bb.instructionCount()-1; i >= 0; i--) {
        Instruction& ins = bb.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++ ) {
            Move& move = ins.move(j);
            if (move.isJump()) {
                if (pred == nullptr) {
                    return std::pair<int, TTAProgram::Move*>(i, &move);
                }
                if (move.isUnconditional()) {
                    if (*pred == ControlFlowEdge::CFLOW_EDGE_NORMAL) {
                        return std::pair<int, TTAProgram::Move*>(i, &move);
                    }
                } else {
                    auto& guard = move.guard().guard();
                    if ((*pred == ControlFlowEdge::CFLOW_EDGE_FALSE)
                        == (guard.isInverted())) {
                        return std::pair<int, TTAProgram::Move*>(i, &move);
                    }
                }
            }
        }
        //cal not handled
        if (ins.hasCall()) {
            return std::pair<int, TTAProgram::Move*>(-1,NULL);
        }
    }
    // not found.
    return std::pair<int, TTAProgram::Move*>(-1,NULL);
}

/**
 * Updates jump instruction jump address to the new one.
 * Also updates CFG is it is changed due completely skipped BB.
 *
 * @param jumpBBN basic block whose delay slots are filled
 * @param fillingBBN next basic block where the instructions are taken
 * @param fillEdge CFG edge connecting the basic blocks.
 * @param jumpAddress jump address being updated to new one.
 * @param slotsFilled how many delay slots were filled.
 * @return true if removed a basic block or an cfg edge.
 */
bool
CopyingDelaySlotFiller::updateJumpsAndCfg(
    BasicBlockNode& jumpBBN, BasicBlockNode& fillingBBN,
    ControlFlowEdge& fillEdge,
    Move* jumpAddressMove, Immediate* jumpAddressImmediate,
    Move* jumpMove, int slotsFilled, Move* skippedJump) {

    bool cfgChanged = false;
    TerminalInstructionReference* jumpAddress = jumpAddressMove != NULL ?
        dynamic_cast<TerminalInstructionReference*>(
            &jumpAddressMove->source()) :
        dynamic_cast<TerminalInstructionReference*>(
            &jumpAddressImmediate->value());

    BasicBlock& nextBB = fillingBBN.basicBlock();
    InstructionReferenceManager& irm = cfg_->program()->
        instructionReferenceManager();
    // TODO: only the correct jump one, nto both
    assert(slotsFilled <= nextBB.instructionCount());

    // filled whole block, jumping to next bb.
    if (slotsFilled == nextBB.instructionCount()) {

        ControlFlowGraph::NodeSet nextBBs =
            cfg_->successors(fillingBBN);
        if (nextBBs.size() != 1) {
            std::string msg = "no succeessors but no jump";
            throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
        }

        BasicBlockNode& newDestNode = **nextBBs.begin();

        // update CFG because jump dest moved.
        ControlFlowEdge* newEdge = new ControlFlowEdge(fillEdge);
        cfg_->disconnectNodes(jumpBBN, fillingBBN);
        cfg_->connectNodes(jumpBBN, newDestNode, *newEdge);
        cfgChanged = true;

        if (skippedJump != NULL) {
            // if we skipped a jump. that jump may have already been filled,
            // and then the dest is the dest of that jump,
            // not beginning of a bb.
            if (skippedJump->isReturn()) {
                assert(jumpMove != NULL);
                jumpMove->setSource(skippedJump->source().copy());
                TTAProgram::ProgramAnnotation annotation(
                    TTAProgram::ProgramAnnotation::
                    ANN_STACKFRAME_PROCEDURE_RETURN);
                jumpMove->setAnnotation(annotation);
                if (jumpAddressMove != NULL && jumpAddressMove != jumpMove) {
                    jumpAddressMove->parent().removeMove(
                        *jumpAddressMove);
                } else {
                    // get rid of the insructionreference by setting
                    // value of the imm to 0.
                    // better way would be deleting the imm but it's
                    // much more complicated
                    if (jumpAddressImmediate != NULL) {
                        jumpAddressImmediate->setValue(
                            new TerminalImmediate(SimValue(0,1)));
                    }
                }
            }
            else {
                InstructionReference ir = 
                    skippedJump->source().instructionReference();
                jumpAddress->setInstructionReference(ir);
            }
        } else {
            // else the dest is the first instruction of the bb after the 
            // filling bb, skipping the skipped instructions.
            InstructionReference ir = irm.createReference(
                newDestNode.basicBlock().instructionAtIndex(
                    newDestNode.basicBlock().skippedFirstInstructions()));
            jumpAddress->setInstructionReference(ir);
        }
               
        // if filling block became dead, remove it.
        if (cfg_->inDegree(fillingBBN) == 0) {
            assert(!irm.hasReference(nextBB.instructionAtIndex(0)));

            // no fall-thru, removed only jump to bb.
            // remove whole bb then.
            cfg_->removeNode(fillingBBN);
            finishBB(fillingBBN, true);

            for (int i = 0; i < nextBB.instructionCount(); i++) {
                Instruction& ins = nextBB.instructionAtIndex(i);
                while(ins.moveCount()) {
                    Move& move = ins.move(0);
                    MoveNode & mn = ddg_->nodeOfMove(move);
                    ddg_->removeNode(mn);
                    delete &mn;
                    ins.removeMove(move);
                }
                while (ins.immediateCount()) {
                    Immediate& imm = ins.immediate(0);
                    ins.removeImmediate(imm);
                }
                    
            }
            delete &fillingBBN;
        }

    } else { // not filled whole block.
        assert(skippedJump == NULL);
        assert(slotsFilled >= nextBB.skippedFirstInstructions());
        InstructionReference ir = irm.createReference(
            nextBB.instructionAtIndex(slotsFilled));
        jumpAddress->setInstructionReference(ir);

        // remove the first instructions that are dead.
        if (cfg_->incomingFTEdge(fillingBBN) == nullptr
	    && &fillingBBN != &cfg_->firstNormalNode()) {
            int deadInsCount = 0;
            while (nextBB.instructionCount() > deadInsCount &&
                   !irm.hasReference(
                       nextBB.instructionAtIndex(deadInsCount))) {
                deadInsCount++;
            }
            nextBB.skipFirstInstructions(deadInsCount);
            if (deadInsCount >= nextBB.instructionCount()) {
                throw IllegalProgram(
                    __FILE__,__LINE__,__func__,
                    "BB got empty illegally");
            }
        }
    }
    return cfgChanged;
}

/**
 * Initializes the delay slot filler for given procedure.
 *
 * Has to be called before it can be used.
 *
 * @param cfg ControlFlowGraph of the procedure.
 * @ddg ddg whole-procedure ddg of the procedure.
 * @param machine machine we are scheduling to.
 * @param um universalmachine.
 */
void
CopyingDelaySlotFiller::initialize(
    ControlFlowGraph& cfg, DataDependenceGraph& ddg,
    const TTAMachine::Machine& machine) {
    cfg_ = &cfg;
    ddg_ = &ddg;
    um_ = &UniversalMachine::instance();

    delaySlots_ = machine.controlUnit()->delaySlots();
    bbnStatus_.clear();
}

void 
CopyingDelaySlotFiller::finishBB(BasicBlockNode& bbn, bool force) {

    std::map<BasicBlockNode*,DataDependenceGraph::NodeSet>::iterator
        i = tempResultNodes_.find(&bbn);

    if (i != tempResultNodes_.end()) {
        DataDependenceGraph::NodeSet& ns = i->second;
        if (!ns.empty()) {
            std::map<BasicBlock*,SimpleResourceManager*>::iterator rmi =
                resourceManagers_.find(&i->first->basicBlock());
            assert (rmi != resourceManagers_.end());
            SimpleResourceManager& rm = *rmi->second;
            unassignTempAssigns(ns, rm);
        }
    }

    bbnStatus_[&bbn] = BBN_TEMPS_CLEANED;

    if (!force) {
        // all predecessors. If some not scheduled, do not yet remove the  rm.
        // then stay in state BBN_TEMPS_CLEANED;
        ControlFlowGraph::EdgeSet iEdges = cfg_->inEdges(bbn);
        
        for (ControlFlowGraph::EdgeSet::iterator inIter = iEdges.begin();
             inIter != iEdges.end(); inIter++) {
            ControlFlowEdge& cfe = **inIter;
            // cannot fill calls.
            if (!cfe.isCallPassEdge()) {
                BasicBlockNode& prevBBN = cfg_->tailNode(cfe);
                if (prevBBN.isNormalBB() && 
                    bbnStatus_[&prevBBN] < BBN_BOTH_FILLED) {
                    return;
                }
            }
        }
    }

    // delete the RM. go to state BBN_ALL_DONE
    std::map<BasicBlock*,SimpleResourceManager*>::iterator rmi =
        resourceManagers_.find(&bbn.basicBlock());
    if (rmi != resourceManagers_.end()) {
        if (rmi->second != NULL) {
            SimpleResourceManager::disposeRM(rmi->second);
        }
        resourceManagers_.erase(rmi);
    }
    bbnStatus_[&bbn] = BBN_ALL_DONE;

}

void
CopyingDelaySlotFiller::unassignTempAssigns(
    DataDependenceGraph::NodeSet& tempAssigns, 
    SimpleResourceManager& rm) {
    for (DataDependenceGraph::NodeSet::iterator j = tempAssigns.begin(); 
         j != tempAssigns.end(); j++) {
        assert((**j).isScheduled());
        rm.unassign(**j);
        ddg_->removeNode(**j);
        delete *j;
    }
    tempAssigns.clear();
}
