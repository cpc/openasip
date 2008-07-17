/**
 * @file CycleLookBackSoftwareBypasser.cc
 *
 * Definition of CycleLookBackSoftwareBypasser interface.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel@cs.tut.fi)
 * @author Vladmír Guzma 2008 (vg@cs.tut.fi)
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
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

//#define MOVE_BYPASSER
/**
 * Constructor.
 *
 * @param cyclesToLookBack How many cycles to look back for the producer of
 *        result when bypassing the operand writes.
 * @param killDeadResults Whether dead results should be killed.
 */
CycleLookBackSoftwareBypasser::CycleLookBackSoftwareBypasser(
    int cyclesToLookBack, bool killDeadResults) : 
    cyclesToLookBack_(cyclesToLookBack),
    killDeadResults_(killDeadResults), selector_(NULL) {
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
 * @param ddg The data dependence grap in which the movenodes belong to.
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

        // result value or already bypassed - don't bypass this
    if (moveNode.isSourceOperation()) {
        return 0;
    }

    if (!moveNode.isDestinationOperation()){
        throw InvalidData(__FILE__, __LINE__, __func__, 
                          "Bypassed move is not Operand move!");
    }
    DataDependenceGraph::EdgeSet edges= ddg.inEdges(moveNode);
    DataDependenceGraph::EdgeSet::iterator edgeIter = edges.begin();
    DataDependenceEdge* bypassEdge = NULL;
    
    // find one incoming raw edge. if multiple, cannot bypass.
    while(edgeIter != edges.end()) {
        
        DataDependenceEdge& edge = *(*edgeIter);
        // if the edge is not a real reg/ra raw edge , skip to net edge
        if (edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            edge.dependenceType() != DataDependenceEdge::DEP_RAW ||
            edge.guardUse() ||
            edge.headPseudo()) {
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
    if (bypassEdge == NULL) {
        return 0;
    }

    MoveNode& source = ddg.tailNode(*bypassEdge);
            
    // source node is too far for our purposes
    ; // do not bypass this operand
    if (cyclesToLookBack_ != INT_MAX &&
        source.cycle() + cyclesToLookBack_ < moveNode.cycle()) {
        return 0;
    }

    if (!source.move().isUnconditional()) {
        // cannot bypass from conditional to unconditional
        if (moveNode.move().isUnconditional()) {
            return 0;
        }
        // cannot bypass from different guard.
        if (!moveNode.move().guard().guard().isEqual(
                source.move().guard().guard())){
            return 0;
        }
    }

    // do no bypass from reg-reg moves - antidependencies not handled
    if (source.isSourceOperation() || source.isSourceConstant()) {
#ifdef MOVE_BYPASSER
        int originalCycle = moveNode.cycle();
#endif
        rm.unassign(moveNode);
        storedSources_.insert(
            std::pair<MoveNode*,MoveNode*>(&moveNode, &source));
        
        ddg.mergeAndKeep(source, moveNode);
        int ddgCycle = ddg.earliestCycle(moveNode);
        if (ddgCycle != INT_MAX) {
#ifdef MOVE_BYPASSER
            ddgCycle = originalCycle;
#endif
            int cycle = rm.earliestCycle(ddgCycle, moveNode);
            if (cycle != -1) {
                rm.assign(cycle, moveNode);
                if (!moveNode.isScheduled()){
                    throw InvalidData(
                        __FILE__, __LINE__, __func__, 
                        "Move assignment failed");
                }                                               
                lastOperandCycle = std::max(lastOperandCycle, cycle);
                // only one bypass per operand is possible, no point to
                // test other edges of same moveNode
                return 1;
            }
        }
        // if node could not be bypassed, we return -1 and let
        // BBScheduler call removeBypass
        return -1;
    }
    // nothing bypassed
    return 0;
}

/**
 * Apply software bypassing to as many moves in the given MoveNodeGroup
 * as possible.
 *
 * This implementation works only if all operand writes have been scheduled.
 * It tries to bypass all the operand write moves.
 *
 * @param candidates The moves to which apply software bypassing, if possible.
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 * @return The count of bypassed moves.
 */
int
CycleLookBackSoftwareBypasser::bypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

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

        // find the trigger
        if (moveNode.move().isTriggering()) {
            trigger = &moveNode;
        }

        if (moveNode.move().isControlFlowMove()) {
            // don't try to bypass control flow moves, they have lot
            // of pseudo dependencies because of return values
            break;
        }

        // bypass this node.
        int rv = bypassNode(moveNode, lastOperandCycle, ddg, rm);

        if (rv == -1) {
            return -1; // failed, need cleanup
        }  else {
            bypassCounter += rv; 
        }
    }

    // try to reassing also moves that were not bypassed
    // detects max cycle to make sure trigger is not too early, later on
    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);
        if (!moveNode.isDestinationOperation() ||
            moveNode.move().isControlFlowMove()) {
            continue;
        }
        if (moveNode.isBypass()) {
            lastOperandCycle =
                std::max(lastOperandCycle, moveNode.cycle());
            continue;
        }
        int oldCycle = moveNode.cycle();
        rm.unassign(moveNode);
        int earliestCycle =
            rm.earliestCycle(ddg.earliestCycle(moveNode),moveNode);
        if (oldCycle > earliestCycle && earliestCycle != -1) {
            rm.assign(earliestCycle, moveNode);
        } else {
            rm.assign(oldCycle, moveNode);
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
    if (trigger != NULL) {
        if (trigger->cycle() < lastOperandCycle) {
            rm.unassign(*trigger);
            int newCycle = rm.earliestCycle(lastOperandCycle, *trigger);
            if (newCycle == -1) {
                // BBScheduler will call removeBypass
                return -1;
            }
            rm.assign(newCycle, *trigger);
            if (!trigger->isScheduled()){
                throw InvalidData(
                    __FILE__, __LINE__, __func__,"Trigger assignment failed");
            }                                                           
        }
    }
    return bypassCounter;
}

/**
 * Remove software bypassing from all moves in the given MoveNodeGroup
 * if possible.
 *
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

    for (int k = 0; k < candidates.nodeCount(); k++) {

        MoveNode& tempNode = candidates.node(k);
        if (MapTools::containsKey(storedSources_, &tempNode)) {
            // if node was bypassed, find a original source
            // and restore it, also unassign if bypass was
            // assigned
            MoveNode* tempSource =
                MapTools::valueForKey<MoveNode*>(storedSources_, &tempNode);
            storedSources_.erase(&tempNode);
            // if it was bypass and was scheduled we
            // need to unassign and restore, should allways be a case
            if (tempNode.isScheduled()) {
                rm.unassign(tempNode);
            }
            ddg.unMerge(*tempSource, tempNode);
            continue;
        }
        if (tempNode.isScheduled()) {
            // it wasn't bypassed but was scheduled as regular
            // operand, so we unschedule it also
            rm.unassign(tempNode);         
        }
    }
}

/**
 * Removes dead resuls for each of the scheduler operand moves, if applicable
 * after whole program operation was scheduled.
 *
 * @param candidates The moves for which remove the dead result moves. Have
 * to be scheduled.
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 * @return number of dead results killed
 */
int
CycleLookBackSoftwareBypasser::removeDeadResults(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    if (!killDeadResults_) {
        return 0;
    }

    for (int i = 0; i < candidates.nodeCount(); i++) {
        // For now, this version is called after operands AND results
        // were scheduled
        if (!candidates.node(i).isScheduled()) {
            return 0;
        }
    }
    // For each of bypassed moves, look for it's original source and
    // if that one has no more outgoing RAW edges and will be
    // overwriten in same basic block (WAW edge) remove it

    int resultsRemoved = 0;
    for (int i = 0; i < candidates.nodeCount(); i++) {
        MoveNode& moveNode = candidates.node(i);
        // Results in candidates are freshly scheduled so they have no
        // bypasses so far
        if (!moveNode.isBypass()) {
            continue;
        }

        if (!MapTools::containsKey(storedSources_, &moveNode)) {
            // In case the result was already removed by other move from same
            // candidates set (both operands were bypassed from same result
            // move) we continue
            continue;
        }

        MoveNode& testedMove = *MapTools::valueForKey<MoveNode*>(
            storedSources_, &moveNode);
        if (!testedMove.isSourceOperation()){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Stored source is not result move!");
        }                                                       

        if (!ddg.resultUsed(testedMove)) {
            // results that has no "use" later and are overwritten are death
            // if there are some other edges we don't do anything
            // Resuls is positively death
            // we need to properly get rid of it
            rm.unassign(testedMove);

            // we lost edges so our notifyScheduled does not notify
            // some antidependencies. store them for notification.
            DataDependenceGraph::NodeSet successors =
                ddg.successors(testedMove);
            ddg.removeNode(testedMove);
            // remove dead result also from map of sources and bypassed
            // moves, it should not by tried to get removed second time
            // by some other bypassed move from same candidate set
            MapTools::removeItemsByValue(storedSources_, &testedMove);
            resultsRemoved++;

            // we lost edges so our notifyScheduled does not notify
            // some antidependencies. notify them.
            if (selector_ != NULL) {
                for (DataDependenceGraph::NodeSet::iterator iter = 
                         successors.begin();
                     iter != successors.end(); iter++) {
                    selector_->mightBeReady(**iter);
                }
            }
        }
    }

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
