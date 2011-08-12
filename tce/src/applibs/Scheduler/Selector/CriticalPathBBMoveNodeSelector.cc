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
 * @file CriticalPathBBMoveNodeSelector.cc
 *
 * Implementation of CriticalPathBBMoveNodeSelector interface.
 *
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CriticalPathBBMoveNodeSelector.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "POMDisassembler.hh"
#include "ProgramOperation.hh"
#include "Procedure.hh"
#include "BasicBlock.hh"
#include "SpecialRegisterPort.hh"

//#define DEBUG_OUTPUT__
//#define WRITE_DOT_SNAPSHOTS

/**
 * Add the unscheduled root nodes of the DDG to the ready list
 */
void
CriticalPathBBMoveNodeSelector::initializeReadylist() {
    DataDependenceGraph::NodeSet roots = ddg_->rootNodes();
    for (DataDependenceGraph::NodeSet::iterator i = roots.begin(); 
         i != roots.end(); 
         ++i) {
        MoveNode& node = **i;

        // a hack to avoid adding the root operations multiple times
        // (the count of input moves): only "announce" the move to the first
        // operand (every operation should have one input operand)
        if (!node.isDestinationOperation())
            mightBeReady(**i);
        else if (node.move().destination().operationIndex() == 1) {
            mightBeReady(**i); 
        }
    }

}

/**
 * Constructor. Creates subgraph of the given big graph
 *
 * @param bigDDG big ddg containing more than just the basic block
 * @param bb basic block for this selector.
 */
CriticalPathBBMoveNodeSelector::CriticalPathBBMoveNodeSelector(
    DataDependenceGraph& bigDDG, TTAProgram::BasicBlock& bb, 
    const TTAMachine::Machine& machine)
    throw (ModuleRunTimeError) : ddgOwned_(true) {
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
CriticalPathBBMoveNodeSelector::CriticalPathBBMoveNodeSelector(
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& machine) : 
    ddgOwned_(true) {
    
    DataDependenceGraphBuilder ddgBuilder;
    ddg_ = ddgBuilder.build(bb,DataDependenceGraph::INTRA_BB_ANTIDEPS);
    ddg_->setMachine(machine);
    
#ifdef WRITE_DOT_SNAPSHOTS
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile("ddg.dot");
#endif

    initializeReadylist();
}

/**
 * Constructor.
 *
 * @param ddg The data dependence graph from which to select moves.
 *            Selector does not take the ownership of the ddg.
 */
CriticalPathBBMoveNodeSelector::CriticalPathBBMoveNodeSelector(
    DataDependenceGraph& ddg, const TTAMachine::Machine& machine) : 
    ddg_(&ddg), ddgOwned_(false) {
    
    ddg_->setMachine(machine);
    
#ifdef WRITE_DOT_SNAPSHOTS
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile("ddg.dot");
#endif

    initializeReadylist();
}

/**
 * Destructor.
 */
CriticalPathBBMoveNodeSelector::~CriticalPathBBMoveNodeSelector() {
    if (ddgOwned_) {
        delete ddg_;
    }
}

/**
 * Returns a group of move nodes which should be scheduled next.
 *
 * @return Move node group.
 */
MoveNodeGroup
CriticalPathBBMoveNodeSelector::candidates() {

    // find a MoveNodeGroup with unscheduled MoveNodes
    while (readyList_.size() > 0) {
        MoveNodeGroup moves = readyList_.top();
        if (!moves.isAlive() || moves.isScheduled())
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
 * Returns the DDG used internally.
 *
 * This is needed temporarily only until the MoveNodeManager is done.
 *
 * @return The DDG.
 */
DataDependenceGraph&
CriticalPathBBMoveNodeSelector::dataDependenceGraph() {
    return *ddg_;
}

/**
 * This should be called by the client as soon as a MoveNode is scheduled
 * in order to update the internal state of the selector.
 *
 * @param node The scheduled MoveNode.
 */
void
CriticalPathBBMoveNodeSelector::notifyScheduled(MoveNode& node) {

    assert(node.isScheduled() && "Notifying scheduled even though it isn't.");
    DataDependenceGraph::NodeSet succ = ddg_->successors(node);
    for (DataDependenceGraph::NodeSet::iterator i = succ.begin(); 
         i != succ.end(); ++i) {
        MoveNode& successor = **i;

        // we schedule operations as entities, so if the successor is a
        // (result) move, it's already in the ready list along with the
        // move itself
        if (!successor.inSameOperation(node))
            mightBeReady(**i);
    }

#ifdef WRITE_DOT_SNAPSHOTS
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile("ddg.dot");
#endif
}

/**
 * Adds the given move node (along with the other possible move nodes in the 
 * same operation) to the ready list in case all its parents in the DDG have 
 * been scheduled.
 *
 * In case the node belongs to an operation, also checks that the other 
 * operand moves are also ready. In that case adds all the nodes in the said 
 * MoveOperation to the ready list in a single MoveNodeGroup. 
 *
 * @param node Move node that might be ready.
 */
void
CriticalPathBBMoveNodeSelector::mightBeReady(MoveNode& node) {

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
        for (int inputIndex = 0; inputIndex < operation.inputMoveCount();
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
            // let's add also the output move(s) to the MoveNodeGroup
            for (int outputIndex = 0; 
                 outputIndex < operation.outputMoveCount();
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
 * Returns true in case the move is "data ready", that is, all its 
 * predecessors have been scheduled.
 *
 * It should be noted that moves within same operation are treated 
 * specially. Result move is considered ready even if the operands
 * moves are not to allow scheduling all moves in the same operation
 * as a single entity. Additionally, as we are considering a basic block
 * at a time, the branch operation is considered never ready before 
 * all the other moves in the basic block have been scheduled.
 *
 * @param node Move to check.
 * @return True if the move is ready to be scheduled.
 */
bool
CriticalPathBBMoveNodeSelector::isReadyToBeScheduled(MoveNode& node) 
    const {

    // the control flow move(s) are ready only if all other moves have been
    // scheduled. In rare case of conditional branching with SPU operation set
    // branch operation can have 2 moves, condition register and destination.
    if (node.move().isControlFlowMove() && 
        ddg_->nodeCount() - ddg_->scheduledNodeCount() > 1) {
        DataDependenceGraph::NodeSet unscheduledMoves = ddg_->unscheduledMoves();	  
        for (DataDependenceGraph::NodeSet::iterator i = unscheduledMoves.begin(); 
            i != unscheduledMoves.end(); ++i) {	
            if ((*i)->move().isControlFlowMove() == false) {
                return false;
            }
        }
    }
    return ddg_->predecessorsReady(node);
}
