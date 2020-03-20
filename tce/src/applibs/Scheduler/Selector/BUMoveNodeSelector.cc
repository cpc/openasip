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
 * @file BUMoveNodeSelector.cc
 *
 * Implementation of BUMoveNodeSelector interface.
 *
 * @author Vladim’r Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "BUMoveNodeSelector.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "POMDisassembler.hh"
#include "ProgramOperation.hh"
#include "Procedure.hh"
#include "BasicBlock.hh"
#include "SpecialRegisterPort.hh"
#include "Terminal.hh"

//#define DEBUG_OUTPUT__
//#define WRITE_DOT_SNAPSHOTS

//#define ABORT_ORPHAN_NODES
//#define WARN_ORPHAN_NODES

#ifdef ABORT_ORPHAN_NODES
#define WARN_ORPHAN_NODES
#endif

/**
 * Add the unscheduled sink nodes of the DDG to the ready list
 */
void
BUMoveNodeSelector::initializeReadylist() {
    DataDependenceGraph::NodeSet sinks = ddg_->sinkNodes();
    for (DataDependenceGraph::NodeSet::iterator i = sinks.begin(); 
         i != sinks.end(); 
         ++i) {
        mightBeReady(**i);
    }
}

/**
 * Constructor. Creates subgraph of the given big graph
 *
 * @param bigDDG big ddg containing more than just the basic block
 * @param bb basic block for this selector.
 */
BUMoveNodeSelector::BUMoveNodeSelector(
    DataDependenceGraph& bigDDG, TTAProgram::BasicBlock& bb,
    const TTAMachine::Machine& machine)
    : ddgOwned_(true) {
    try {
        ddg_ = bigDDG.createSubgraph(bb);
        ddg_->setMachine(machine);
    } catch (InstanceNotFound& inf) {
        ModuleRunTimeError e(
            __FILE__,__LINE__,__func__,"Creation of subgraph failed");
        e.setCause(inf);
        throw e;
    }
    initializeReadylist();
}

/**
 * Constructor.
 *
 * @param bb The basic block from which to select moves.
 */
BUMoveNodeSelector::BUMoveNodeSelector(
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) : 
    ddgOwned_(true) {    
    DataDependenceGraphBuilder ddgBuilder;
    ddg_ = ddgBuilder.build(
        bb,DataDependenceGraph::INTRA_BB_ANTIDEPS, machine);
    ddg_->setMachine(machine);
    initializeReadylist();
#if 0    
        ddg_->setCycleGrouping(true);
        ddg_->writeToDotFile("ddg.dot");    
#endif
}

/**
 * Constructor.
 *
 * @param ddg The data dependence graph from which to select moves.
 *            Selector does not take the ownership of the ddg.
 */
BUMoveNodeSelector::BUMoveNodeSelector(
    DataDependenceGraph& ddg, const TTAMachine::Machine& machine) : 
    ddg_(&ddg), ddgOwned_(false) {    
    ddg_->setMachine(machine);
    initializeReadylist();
}
/**
 * Destructor.
 */
BUMoveNodeSelector::~BUMoveNodeSelector() {
    if (ddgOwned_) {
        delete ddg_;
    }
}

/**
 * Returns DDG used internally.
 */
DataDependenceGraph&
BUMoveNodeSelector::dataDependenceGraph() {
    return *ddg_;
}
/**
 * Returns a group of move nodes which should be scheduled next.
 *
 * @return Move node group.
 */
MoveNodeGroup
BUMoveNodeSelector::candidates() {
    
    // find a MoveNodeGroup with unscheduled MoveNodes
    while (readyList_.size() > 0) {
        MoveNodeGroup moves = readyList_.top();
        if (!moves.isAlive() || moves.isScheduled() ||
            !isReadyToBeScheduled(moves))
            readyList_.pop();
        else
            return moves;
    }
    // nothing in ready list, let's see if there are "orphan" nodes
    // still to schedule
    if (ddg_->nodeCount() - ddg_->scheduledNodeCount() > 0) {
        DataDependenceGraph::NodeSet unscheduled = ddg_->unscheduledMoves();
        for (DataDependenceGraph::NodeSet::iterator i = unscheduled.begin(); 
             i != unscheduled.end();
             ++i) {
            MoveNode& node = **i;
#ifdef WARN_ORPHAN_NODES
            std::cerr << "Found orphan node: " << node.toString() << std::endl;
            ddg_->writeToDotFile("oprhan.dot");
#ifdef ABORT_ORPHAN_NODES
            abortWithError("orphan node!");
#endif
#endif
            mightBeReady(node);
        }
    }
    
    // did we find new nodes?
    while (readyList_.size() > 0) {
        MoveNodeGroup moves = readyList_.top();
        if (moves.isScheduled())
            readyList_.pop();
        else
            return moves;
    }
    
    // return an empty move node group
    return MoveNodeGroup();
}

/**
 * This should be called by the client as soon as a MoveNode is scheduled
 * in order to update the internal state of the selector.
 *
 * @param node The scheduled MoveNode.
 */
void
BUMoveNodeSelector::notifyScheduled(MoveNode& node) {

    assert(node.isScheduled() && "Notifying scheduled even though it isn't.");
    DataDependenceGraph::NodeSet pred = ddg_->predecessors(node);
    for (DataDependenceGraph::NodeSet::iterator i = pred.begin(); 
         i != pred.end(); ++i) {
        MoveNode& predecessor = **i;

        // we schedule operations as entities, so if the predecessor is a
        // (operand) move, it's already in the ready list along with the
        // move itself
        if (!predecessor.inSameOperation(node))
            mightBeReady(**i);
    }
}

/**
 * Adds the given move node (along with the other possible move nodes in the 
 * same operation) to the ready list in case all its children in the DDG have 
 * been scheduled.
 *
 * In case the node belongs to an operation, also checks that the other 
 * result moves are also ready. In that case adds all the nodes in the said 
 * MoveOperation to the ready list in a single MoveNodeGroup. 
 *
 * @param node Move node that might be ready.
 */
void
BUMoveNodeSelector::mightBeReady(MoveNode& node) {

    if (node.isScheduled()) {
        return;
    }

    if (!isReadyToBeScheduled(node)) 
        return;

    if (node.isDestinationOperation() || node.isSourceOperation()) {
        // it's a trigger, result, or operand move, let's see if all the
        // moves of the operation are ready to be scheduled
        ProgramOperation& operation = 
            (node.isDestinationOperation()?
             (node.destinationOperation()) : node.sourceOperation());
        bool allReady = true;
        MoveNodeGroup moves(*ddg_);
        for (int outputIndex = 0; outputIndex < operation.outputMoveCount();
             ++outputIndex) {
            MoveNode& m = operation.outputMove(outputIndex);
            if (&m != &node) {
                if (!isReadyToBeScheduled(m)) {
                    allReady = false;
                    break;
                } 
            } 
            if (!m.isScheduled())
                moves.addNode(m);
        }
        if (allReady) {
            // let's add also the input move(s) to the MoveNodeGroup
            for (int inputIndex = 0; 
                 inputIndex < operation.inputMoveCount();
                 ++inputIndex) {
                MoveNode& m = operation.inputMove(inputIndex);
                if (&m != &node) { 
                    if (!isReadyToBeScheduled(m)) {
                        allReady = false;
                        break;
                    }
                }
                if (!m.isScheduled())
                    moves.addNode(m);
            }
            if (allReady) {
                readyList_.push(moves);
            }
        }
    } else if ((node.isSourceVariable() || node.move().source().isRA()) &&
               (node.isDestinationVariable() || 
                node.move().destination().isRA())) {
        // it's a register to register move, we can always schedule these
        // as soon as all the dependencies are satisfied
        // handle RA -> ireg also as a register to register move
        if (isReadyToBeScheduled(node)) {
            MoveNodeGroup move(*ddg_);
            move.addNode(node);
            readyList_.push(move);
        }
    } else if (node.isSourceConstant() && node.isDestinationVariable()) {
        if (isReadyToBeScheduled(node)) {
            MoveNodeGroup move(*ddg_);
            move.addNode(node);
            readyList_.push(move);
        }
        
    } else {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            (boost::format("Illegal move '%s'.") % 
             POMDisassembler::disassemble(node.move())).str());
    }

}

/**
 * Queues all nodes of PO into queue, if they are not already in another set.
 *
 * @param po ProgramOperation whose nodes to add
 * @param nodes nodes where the final result is. if node is here, do not add
 *        to queue
 * @param queue quque where to add the nodes.
 */
void BUMoveNodeSelector::queueOperation(
    ProgramOperation& po,
    const DataDependenceGraph::NodeSet& nodes,
    DataDependenceGraph::NodeSet& queue) {
    for (int j = 0; j < po.inputMoveCount(); j++) {
        MoveNode& inputMove = po.inputMove(j);
        // only add if not already added
        if (nodes.find(&inputMove) == nodes.end()) {
            queue.insert(&inputMove);
        }
    }

    for (int j = 0; j < po.outputMoveCount(); j++) {
        MoveNode& outputMove = po.outputMove(j);
        // only add if not already added
        if (nodes.find(&outputMove) == nodes.end()) {
            queue.insert(&outputMove);
        }
    }
}

/**
 * Returns true in case the move is "data ready", that is, all its 
 * successors have been scheduled.
 *
 * It should be noted that moves within same operation are treated 
 * specially.  Additionally, as we are considering a basic block
 * at a time, the branch operation is considered always ready before 
 * all the other moves in the basic block have been scheduled.
 *
 * @param node Move to check.
 * @return True if the move is ready to be scheduled.
 */
bool
BUMoveNodeSelector::isReadyToBeScheduled(MoveNode& node) 
    const {
    // the control flow move(s) are ready as they are at the end of the basic
    // blocks.
    // TODO: make sure CF moves are scheduled proper number of delay slots
    // from the end of the block
    if (node.move().isControlFlowMove()) {
        return true;    
    }
    return ddg_->successorsReady(node);
}

bool
BUMoveNodeSelector::isReadyToBeScheduled(MoveNodeGroup& nodes) const {
    for (int i = 0; i < nodes.nodeCount(); i++) {
        MoveNode& mn = nodes.node(i);
        if (!isReadyToBeScheduled(mn)) {
            return false;
        }
    }
    return true;
}
