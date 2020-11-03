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
 * @file CycleLookBackSoftwareBypasser.cc
 *
 * Definition of CycleLookBackSoftwareBypasser interface.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @author Vladmír Guzma 2008 (vg-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "CycleLookBackSoftwareBypasser.hh"

#include "MoveNodeGroup.hh"
#include "DataDependenceGraph.hh"
#include "ResourceManager.hh"
#include "MapTools.hh"
#include "TerminalFUPort.hh"
#include "SpecialRegisterPort.hh"
#include "MoveNodeSelector.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "Bus.hh"
#include "SimpleResourceManager.hh"
#include "SchedulerCmdLineOptions.hh"
#include "ProgramOperation.hh"
#include "Move.hh"
// DEBUG:
#include "SimpleResourceManager.hh"
#include "Instruction.hh"
#include "POMDisassembler.hh"
#include "TerminalImmediate.hh"

//#define MOVE_BYPASSER
/**
 * Constructor.
 *
 */
CycleLookBackSoftwareBypasser::CycleLookBackSoftwareBypasser() :
    cyclesToLookBack_(5), cyclesToLookBackNoDRE_(1),
    killDeadResults_(true), bypassFromRegs_(true), bypassToRegs_(true), 
    selector_(NULL) {

    SchedulerCmdLineOptions* opts = 
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    if (opts != NULL) {
        if (opts->bypassDistance() > -1) {
            cyclesToLookBack_ = opts->bypassDistance();
        }
        if (opts->noDreBypassDistance() > -1) {
            cyclesToLookBackNoDRE_ = opts->noDreBypassDistance();
        }
        killDeadResults_ = opts->killDeadResults();
    }
}

/**
 * Empty destructor.
 */
CycleLookBackSoftwareBypasser::~CycleLookBackSoftwareBypasser() {
}

/**
 * Tries to bypass a MoveNode.
 *
 * @param moveNode MoveNode to bypass.
 * @param lastOperandCycle in which contains last cycle of operands of the
 *        operation
 * @param ddg The data dependence graph in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 * @return -1 if failed and need fixup, 1 is succeeded, 0 if did not bypass.
 */
int
CycleLookBackSoftwareBypasser::bypassNode(
    MoveNode& moveNode,
    int& lastOperandCycle,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    int cyclesToLookBack = cyclesToLookBack_;
    if (moveNode.isDestinationVariable() && !bypassToRegs_) {
        return 0;
    }
    
    int originalCycle = moveNode.cycle();
    int latestCycle = originalCycle;
    
    DataDependenceGraph::EdgeSet edges = ddg.inEdges(moveNode);
    DataDependenceGraph::EdgeSet::iterator edgeIter = edges.begin();
    DataDependenceEdge* bypassEdge = NULL;

    // find one incoming raw edge. if multiple, cannot bypass.
    while (edgeIter != edges.end()) {

        DataDependenceEdge& edge = *(*edgeIter);
        // if the edge is not a real reg/ra raw edge, skip to next edge
        if (edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            edge.dependenceType() != DataDependenceEdge::DEP_RAW ||
            edge.guardUse() || edge.headPseudo()) {
            edgeIter++;
            continue;
        }

        if (bypassEdge == NULL) {
            bypassEdge = &edge;
        } else {
            // cannot bypass if multiple inputs
            return 0;
        }
        edgeIter++;
    }

    // if no bypassable edge found, cannot bypass
    if (bypassEdge == NULL || bypassEdge->isBackEdge()) {
        return 0;
    }

    MoveNode& source = ddg.tailNode(*bypassEdge);

    if (!source.isScheduled()) {
        return 0;
    }

    // don't bypass from incoming tempregcopies of immediates
    if (source.isSourceConstant() &&
        source.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE)) {
        return 0;
    }

    // if cannot kill dead result, divide bypass dist by 2
    if (source.isSourceOperation() &&
        moveNode.isDestinationOperation() &&
        cyclesToLookBack != INT_MAX && 
        cyclesToLookBack > cyclesToLookBackNoDRE_ &&
        (ddg.regRawSuccessorCount(source, true) > 1 ||
         ddg.regRawSuccessorCount(source, false) < 
         static_cast<DataDependenceGraph*>(ddg.rootGraph())->
         regRawSuccessorCount(source, false))) {
        cyclesToLookBack = cyclesToLookBackNoDRE_;
    }

    // source node is too far for our purposes
    // do not bypass this operand
    if (cyclesToLookBack != INT_MAX &&
        source.cycle() + cyclesToLookBack < moveNode.cycle()) {
        return 0;
    }

    if (!ddg.guardsAllowBypass(source, moveNode)) {
        return 0;
    }

    // if dest register, only allow input from reg or imm.
    // and limit the read to same as original read
    // in order to not make live ranges longer, limiting schdule
    if (!moveNode.isDestinationOperation()) {
        latestCycle = source.cycle();
    }
    if (!(source.isSourceOperation() || source.isSourceConstant())) {
        if (!bypassFromRegs_) {
            return 0;
        }
        // handle antidependencies from reg-reg moves.
        DataDependenceGraph::NodeSet warSuccs =
            ddg.regWarSuccessors(source);
        for (DataDependenceGraph::NodeSet::iterator i = warSuccs.begin();
             i != warSuccs.end();i++) {
            MoveNode& mn = **i;
            if (mn.isScheduled()) {
                if (mn.cycle() < latestCycle) {
                    latestCycle = std::min(latestCycle, mn.cycle());
                }
            } 
        }
        
        // redundant latest check to prevent loops in ddg.
        for (edgeIter = edges.begin(); edgeIter != edges.end(); edgeIter++) {
            DataDependenceEdge& edge = **edgeIter;
            if (&edge != bypassEdge) {
                MoveNode& tail = ddg.tailNode(edge);
                if (!tail.isScheduled()) {
                    return 0;
                }
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                    if (tail.cycle() > latestCycle) {
                        return 0;
                    }
                } else {
                    if (tail.cycle() > latestCycle-1) {
                        return 0;
                    }
                }
            }
        }
    } 
    rm.unassign(moveNode);
    storedSources_.insert(
        std::pair<MoveNode*, MoveNode*>(&moveNode, &source));
    
    // if mergeandkeep fails, undo bypass and return 0
    if (!ddg.mergeAndKeepUser(source, moveNode)) {
        rm.assign(originalCycle, moveNode);
        assert(moveNode.isScheduled());
        storedSources_.erase(&moveNode);
        return 0;
    }

    if (moveNode.move().source().isImmediateRegister()) {
        moveNode.move().setSource(
            static_cast<SimpleResourceManager&>(rm).immediateValue(source)->copy());
    }

    // then try to assign the bypassed node..
    int ddgCycle = ddg.earliestCycle(moveNode);
    if (!moveNode.move().isUnconditional()) {
        ddgCycle = std::max(ddgCycle, moveNode.guardLatency() - 1);
    }
    if (ddgCycle != INT_MAX) {
#ifdef MOVE_BYPASSER
        ddgCycle = originalCycle;
#endif
        // remove dead results now?
        int sourceCycle = source.cycle();
        bool sourceRemoved = false;
        // Check if machine is forcing disabling DRE,
        // if so disable when first move is bypassed.
        if (killDeadResults_ && 
            source.move().bus().machine()->alwaysWriteResults()) {
            killDeadResults_ = false;
        }
        // disable DRE if ii != 0
        // only kill if dest is op

        // >8058 fails
        // >8059 works
        if (killDeadResults_ && !ddg.resultUsed(source)) { // && source.nodeID() > 8058) {
            // if restoring lated
            sourceCycles_[&source] = sourceCycle;
            sourceRemoved = true;
            // results that has no "use" later and are overwritten are
            // dead if there are some other edges we don't do anything
            // Resuls is positively death
            // we need to properly get rid of it
            sourceBuses_[&source] = &source.move().bus();
            rm.unassign(source);
        }

        int cycle = rm.earliestCycle(ddgCycle, moveNode);
        if (cycle != -1 && cycle <= latestCycle) {
            rm.assign(cycle, moveNode);
            if (!moveNode.isScheduled()){
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Move assignment failed");
            }
            lastOperandCycle = std::max(lastOperandCycle, cycle);
            // only one bypass per operand is possible, no point to
            // test other edges of same moveNode
            if (sourceRemoved) {
                ddg.copyDepsOver(source, true, false);
            }
            bypassCount_++;
            return 1;
        } else {
            // undo only this bypass.
            // allow other moves of same po to be bypassed.
            ddg.unMergeUser(source, moveNode);
            if (!rm.canAssign(originalCycle, moveNode)) {
                // Cannot return to original position. getting problematic.
                // return source to it's position
                if (sourceRemoved) {
                    assert(sourceBuses_[&source] != NULL);
                    source.move().setBus(*sourceBuses_[&source]);
                    assert(rm.canAssign(sourceCycle, source));
                    rm.assign(sourceCycle, source);
                    sourceCycles_.erase(&source);
                    sourceBuses_.erase(&source);
                }
                // Try if we can assign it to some earlier place.
                ddgCycle = ddg.earliestCycle(moveNode);
                if (!moveNode.move().isUnconditional()) {
                    ddgCycle = std::max(ddgCycle, moveNode.guardLatency()-1);
                }
                int ec = rm.earliestCycle(ddgCycle, moveNode);
                if (ec != -1 && ec < originalCycle) {
                    rm.assign(ec, moveNode);
                    return 0;
                } else {
                    // Need to abort bypassing.
                    return -1;
                }
            }
            rm.assign(originalCycle, moveNode);
            assert(moveNode.isScheduled());
            storedSources_.erase(&moveNode);

            // return source to it's position
            if (sourceRemoved) {
                assert(sourceBuses_[&source] != NULL);
                source.move().setBus(*sourceBuses_[&source]);
                assert(rm.canAssign(sourceCycle, source));
                rm.assign(sourceCycle, source);
                sourceCycles_.erase(&source);
                sourceBuses_.erase(&source);
            }
            return 0;
        }
    }
    // if node could not be bypassed, we return -1 and let
    // BBScheduler call removeBypass
    return -1;
}

/**
 * Apply software bypassing to as many moves in the given MoveNodeGroup
 * as possible.
 *
 * This implementation works only if all operand writes have been scheduled.
 * It tries to bypass all the operand write moves.
 *
 * @param candidates The moves to which apply software bypassing, if possible.
 * @param ddg The data dependence graph in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 * @return The count of bypassed moves.
 */
int
CycleLookBackSoftwareBypasser::bypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm,
    bool bypassTrigger) {

    // bypassing disabled with 0
    if (cyclesToLookBack_ <= 0) {
        return 0;
    }
#ifdef MOVE_BYPASSER
    cyclesToLookBack_ = 1;
#endif
    int bypassCounter = 0;

    MoveNode* trigger = NULL;
    int lastOperandCycle = 0;

    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);

        // result value or already bypassed - don't bypass this
        if (moveNode.isSourceOperation()) {
            continue;
        }

        // find the trigger, will try to bypass it as the last one
        if (moveNode.move().isTriggering()) {
            trigger = &moveNode;
            continue;
        }

        // bypass this node.
        int rv = bypassNode(moveNode, lastOperandCycle, ddg, rm);
        if (rv == -1) {
            return -1; // failed, need cleanup
        }  else {
            bypassCounter += rv;
        }
    }
    // Try to bypass triggering move, if possible ok, if not
    // it will be rescheduled at the end after all not bypassed
    // moves are tried to be rescheduled
    bool triggerWasBypassed = false;
    if (trigger != NULL && !trigger->move().isControlFlowMove() &&
        !trigger->isSourceOperation() && bypassTrigger) {
        int rv = bypassNode(*trigger, lastOperandCycle, ddg, rm);
        if (rv == -1) {
            return -1; // failed, need cleanup
        }  else {
            bypassCounter += rv;
            if (rv == 1) {
                triggerWasBypassed = true;
            }
        }
    }
    // at this point the schedule might be broken in case we
    // managed to bypass the trigger and it got pushed above
    // an operand move

    // try to reschedule the moves that were not bypassed because
    // earlier operand moves might allow scheduling also the trigger
    // earlier thus shorten the schedule

    // this might fix the schedule in case the previous loop
    // managed to push trigger above an operand move
    // if not, the last loop fixes the situation

    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);
        if (!moveNode.isDestinationOperation() ||
            moveNode.move().isControlFlowMove()) {
            continue;
        }
        // Bypassed moves and bypassed trigger are not rescheduled here
        if (moveNode.isBypass() ||
            (trigger == &moveNode && triggerWasBypassed)) {
            lastOperandCycle =
                std::max(lastOperandCycle, moveNode.cycle());
            continue;
        }
        int oldCycle = moveNode.cycle();
        int earliestCycleDDG = ddg.earliestCycle(moveNode);

        if (earliestCycleDDG >= oldCycle) {
            continue;
        }
        rm.unassign(moveNode);
        int earliestCycle = earliestCycleDDG;

        if (!moveNode.move().isUnconditional()) {
            earliestCycle = 
                std::max(earliestCycleDDG, moveNode.guardLatency() - 1);
        }
        earliestCycle = rm.earliestCycle(earliestCycle, moveNode);
        if ((oldCycle > earliestCycle && earliestCycle != -1) ||
            (trigger == &moveNode && earliestCycle != -1)){
            try {
                rm.assign(earliestCycle, moveNode);
            } catch (const Exception& e) {
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
                    e.errorMessageStack());
            }
        } else {
            try {
                if (!rm.canAssign(oldCycle, moveNode)) {
                    return -1;
                }
                rm.assign(oldCycle, moveNode);
            } catch (const Exception& e) {
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
                    e.errorMessageStack());
            }
        }
        lastOperandCycle =
            std::max(lastOperandCycle, moveNode.cycle());
        if (!moveNode.isScheduled()){
            throw InvalidData(
                __FILE__, __LINE__, __func__, "Move assignment failed");
        }

    }
    // Traditional test if trigger is not earlier then some operand,
    // could happen due to bypass of trigger
    // todo: this could be optimized by trying to uxchange cycles of
    // trigger and operand in this case.
    if (trigger != NULL) {
        try{
        if (trigger->cycle() < lastOperandCycle) {
            rm.unassign(*trigger);
            int newCycle = rm.earliestCycle(lastOperandCycle, *trigger);
            int latestDDG = ddg.latestCycle(*trigger);
            if (newCycle == -1 || newCycle > latestDDG) {
                triggerAbortCount_++;
                // BBScheduler will call removeBypass
                return -1;
            }
            rm.assign(newCycle, *trigger);
            if (!trigger->isScheduled()){
                throw InvalidData(
                    __FILE__, __LINE__, __func__,"Trigger assignment failed");
            }
        }
        } catch (const Exception& e) {
            throw ModuleRunTimeError(
                __FILE__, __LINE__, __func__,e.errorMessageStack());
        }
    }
    return bypassCounter;
}

/**
 * Remove software bypassing from all moves in the given MoveNodeGroup
 * if possible.
 *
 * @param candidates The moves from which to remove software bypassing, if
 * possible.
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 */
void
CycleLookBackSoftwareBypasser::removeBypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    // bypassing disabled
    if (cyclesToLookBack_ <= 0) {
        return;
    }

    // Some of operand bypassing attempts failed
    // Unschedule all operands, unassign them also since in BBSched
    // this is followed by another attempt to schedule operands

    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);
        if (moveNode.isScheduled()) {
            rm.unassign(moveNode);
        }
    }

    for (int k = 0; k < candidates.nodeCount(); k++) {
        MoveNode& moveNode = candidates.node(k);
        removeBypass(moveNode, ddg, rm);
    }
}

void 
CycleLookBackSoftwareBypasser::removeBypass(
    MoveNode& moveNode,
    DataDependenceGraph& ddg,
    ResourceManager& rm, bool restoreSource) {
    
    // bypassing disabled
    if (cyclesToLookBack_ <= 0) {
        return;
    }
    
    if (MapTools::containsKey(storedSources_, &moveNode)) {
        // if node was bypassed, find a original source
        // and restore it, also unassign if bypass was
        // assigned
        MoveNode* tempSource =
            MapTools::valueForKey<MoveNode*>(storedSources_, &moveNode);
        assert(tempSource != NULL);
        storedSources_.erase(&moveNode);
        // if it was bypass and was scheduled we
        // need to unassign and restore, should allways be a case
        if (moveNode.isScheduled()) {
            rm.unassign(moveNode);
        }

        // if we unassigned the source of the bypass, restore it.
        if (!tempSource->isScheduled()) {
            std::map<MoveNode*, int>::iterator cycleIter = 
                sourceCycles_.find(tempSource);
            if (cycleIter != sourceCycles_.end()) {
                if (restoreSource) {
                    assert(sourceBuses_[tempSource] != NULL);
                    tempSource->move().setBus(*sourceBuses_[tempSource]);
                    assert(rm.canAssign(cycleIter->second, *tempSource));
                    rm.assign(cycleIter->second, *tempSource); // fails here. somebody else uses the bus?
                }
                sourceCycles_.erase(cycleIter);
                sourceBuses_.erase(tempSource);
            }
        }
        ddg.unMergeUser(*tempSource, moveNode);
	    bypassCount_--;
    }
    // this is only for afterwards cleanup.
    if (MapTools::containsKey(removedStoredSources_, &moveNode)) {
        MoveNode* tempSource =
            MapTools::valueForKey<MoveNode*>(removedStoredSources_, &moveNode);
        assert(tempSource != NULL);
        removedStoredSources_.erase(&moveNode);

        if (moveNode.isScheduled()) {
            rm.unassign(moveNode);
        }
        ddg.unMergeUser(*tempSource, moveNode);
    }
}

int
CycleLookBackSoftwareBypasser::removeDeadResults(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm,
    std::set<std::pair<TTAProgram::Move*, int> >& removedMoves) {

    for (int i = 0; i < candidates.nodeCount(); i++) {
        // For now, this version is called after operands AND results
        // were scheduled
        if (!candidates.node(i).isScheduled()) {
            return 0;
        }
    }

    // For each of bypassed moves, look for its original source and
    // if that one has no more outgoing RAW edges and will be
    // overwritten in same basic block (WAW edge) remove it

    int resultsRemoved = 0;
    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);
        // Results in candidates are freshly scheduled so they have no
        // bypasses so far
        if (!MapTools::containsKey(storedSources_, &moveNode)) {
            // In case the result was already removed by other move from same
            // candidates set (both operands were bypassed from same result
            // move) we continue
            continue;
        }

        // the original result move for the bypassed move
        MoveNode& resultMove = 
            *MapTools::valueForKey<MoveNode*>(storedSources_, &moveNode);

        // if killed this one, finish the kill

        // try to get the original cycle of the result move
        std::map<MoveNode*, int>::iterator srcIter = 
            sourceCycles_.find(&resultMove);
        if (srcIter != sourceCycles_.end()) {

            // we lose edges so our notifyScheduled does not notify
            // some antidependencies, store them for notification.
            DataDependenceGraph::NodeSet successors =
                ddg.successors(resultMove);

            successors.erase(&resultMove); // if WaW to itself, remove it.

            ddg.dropNode(resultMove);
            removedNodes_.insert(&resultMove);

            // remove dead result also from map of sources and bypassed
            // moves, it should not by tried to get removed second time
            // by some other bypassed move from same candidate set
            for (std::map<MoveNode*, MoveNode*, MoveNode::Comparator>::
                     iterator j = storedSources_.begin(); 
                 j != storedSources_.end();) {
                if (j->second == &resultMove) {
                    removedStoredSources_[j->first] = &resultMove;
                    storedSources_.erase(j++);
                } else {
                    j++;
                }
            }

            removedMoves.insert(
                std::make_pair(&resultMove.move(), (*srcIter).second));

            sourceCycles_.erase(srcIter);
            resultsRemoved++;
            deadResultCount_++;
	    
            // we lost edges so our notifyScheduled does not notify
            // some antidependencies. notify them.
            if (selector_ != NULL) {
                for (DataDependenceGraph::NodeSet::iterator iter =
                         successors.begin();
                     iter != successors.end(); iter++) {
                    // don't notify other dead results
                    if (sourceCycles_.find(*iter) == sourceCycles_.end()) {
                        selector_->mightBeReady(**iter);
                    }
                }
            }
        } else {
            // we might have some orhpan nodes because some antideps have moved
            // from user node to source node,
            // so make sure notify the scheduler about them.
            if (ddg.hasNode(resultMove)) {
                DataDependenceGraph::NodeSet successors =
                    ddg.regWawSuccessors(resultMove);
                if (selector_ != NULL) {
                    for (DataDependenceGraph::NodeSet::iterator iter =
                             successors.begin();
                         iter != successors.end(); iter++) {
                        // don't notify other dead results
                        if (sourceCycles_.find(*iter) == sourceCycles_.end()) {
                            selector_->mightBeReady(**iter);
                        }
                    }
                }
            }
        }

        // also remove dummy move to itself moves, as the bypassed move.
        if (moveNode.move().source().equals(
                moveNode.move().destination())) {

            // we lost edges so our notifyScheduled does not notify
            // some antidependencies. store them for notification.
            DataDependenceGraph::NodeSet successors =
                ddg.successors(moveNode);
            
            successors.erase(&moveNode); // if WaW to itself, rremove it.

            rm.unassign(moveNode);

            // this may lead to extra raw edges.
            static_cast<DataDependenceGraph*>(ddg.rootGraph())->
                copyDepsOver(moveNode, true, true);

            ddg.dropNode(moveNode);
            removedNodes_.insert(&moveNode);

            // we lost edges so our notifyScheduled does not notify
            // some antidependencies. notify them.
            if (selector_ != NULL) {
                for (DataDependenceGraph::NodeSet::iterator iter =
                         successors.begin();
                     iter != successors.end(); iter++) {
                    // don't notify other dead results
                    if (sourceCycles_.find(*iter) == sourceCycles_.end()) {
                        selector_->mightBeReady(**iter);
                    }
                }
            }
        }
    }


    assert(sourceCycles_.empty());

    return resultsRemoved;
}

/*
 * Registers the selector being used to the bypasser.
 *
 * If the bypasser has been registered to the selector,
 * bypasses can notify the selector about dependence changes.
 * Currently it notifies the successors of a node being removed due
 * dead result elimination.
 *
 * @param selector selector which bypasser notifies on some dependence changes.
 */
void
CycleLookBackSoftwareBypasser::setSelector(MoveNodeSelector* selector) {
    selector_ = selector;
}

/** 
 * Clears the storedSources data structure, when the old values in it are
 * not needed anymore, allowing them to be deleted by the objects who
 * own them.
 *
 * Delete node from sourceCycles structure, as thesse are not deleted 
 * elsewhere.
 */
void
CycleLookBackSoftwareBypasser::clearCaches(DataDependenceGraph& ddg,
    bool removeDeletedResults) {
    storedSources_.clear();
    sourceBuses_.clear();
    if (removeDeletedResults) {
        // TODO: somebody should also delete these
        for (DataDependenceGraph::NodeSet::iterator i = removedNodes_.begin();
             i != removedNodes_.end(); i++) {
            if (ddg.rootGraph() != &ddg) {
                ddg.rootGraph()->removeNode(**i);
            }
            delete *i;
        }
    }
    removedNodes_.clear();
    removedStoredSources_.clear();
}

void
CycleLookBackSoftwareBypasser::printStats() {
    std::cerr << "Bypasser statistics: " << std::endl
              << "\tBypasses: " << bypassCount_ << std::endl
              << "\tKilled results: " << deadResultCount_ << std::endl
              << "\tTrigger too early aborts: " << triggerAbortCount_ << std::endl;
}

int CycleLookBackSoftwareBypasser::bypassCount_ = 0;
int CycleLookBackSoftwareBypasser::deadResultCount_ = 0;
int CycleLookBackSoftwareBypasser::triggerAbortCount_ = 0;
