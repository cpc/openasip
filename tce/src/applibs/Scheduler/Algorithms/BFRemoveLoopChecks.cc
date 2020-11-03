#include "BFRemoveLoopChecks.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include "Operation.hh"

bool
BFRemoveLoopChecksAndJump::operator()() {

    POSet queuedToRemovePo;
    POSet maybeRemovePo;
    POSet queuedAlivePo;
    POSet finishedAlivePo;

    DataDependenceGraph::NodeSet queuedToRemoveMove;
    DataDependenceGraph::NodeSet maybeRemoveMove;
    DataDependenceGraph::NodeSet queuedAliveMoves;
    DataDependenceGraph::NodeSet finishedAliveMoves;

    DataDependenceGraph& rootddg = static_cast<DataDependenceGraph&>
        (*ddg().rootGraph());

    int count = 0;
    for (int i = 0; i < ddg().nodeCount(); i++) {
        MoveNode& mn = ddg().node(i);
        if (mn.move().isJump()) {
            assert(mn.destinationOperationCount() == 1);
            queuedToRemovePo.insert(mn.destinationOperationPtr());
            continue;
        }

        // Mark other nodes alive
        if (mn.isDestinationOperation()) {
            ProgramOperation& po = mn.destinationOperation();
            const Operation& op = po.operation();
            if (op.hasSideEffects() || op.writesMemory() ||
                op.isControlFlowOperation()) {
                queueAliveMove(mn, queuedAlivePo, finishedAlivePo,
                               queuedAliveMoves, finishedAliveMoves);
                continue;
            }
        }

        int outd = rootddg.outDegree(mn);
        for (int j = 0; j < outd; j++) {
            DataDependenceEdge& e = rootddg.outEdge(mn,j);
            if (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
                e.dependenceType() == DataDependenceEdge::DEP_WAR) {
                continue;
            }
        
            // edges out from this BB are considered alive
            MoveNode& head = rootddg.headNode(e);                    
            if (!ddg().hasNode(head)) {
                queueAliveMove(mn, queuedAlivePo, finishedAlivePo,
                               queuedAliveMoves, finishedAliveMoves);
            }
        }
    }
    
    // propagate alive status
    while (!queuedAlivePo.empty() || !queuedAliveMoves.empty()) {
        if (!queuedAlivePo.empty()) {
            checkAlivePO(queuedAlivePo, finishedAlivePo, queuedAliveMoves, finishedAliveMoves);
        } 
        if (!queuedAliveMoves.empty()) {
            checkAliveMove(queuedAliveMoves, finishedAliveMoves, queuedAlivePo, finishedAlivePo);
        }
    }

    // then remove POs and movenodes that are not alive
    while (!queuedToRemovePo.empty() || !maybeRemovePo.empty() ||
           !queuedToRemoveMove.empty() || !maybeRemoveMove.empty()) {
        if (!queuedToRemovePo.empty()) {
            removePoFromQueue(queuedToRemovePo, maybeRemovePo,
                              queuedToRemoveMove, maybeRemoveMove);
            count++;
            continue;
        }

        if (!queuedToRemoveMove.empty()) {
            removeMoveFromQueue(queuedToRemoveMove, maybeRemoveMove,
                                queuedToRemovePo, maybeRemovePo);
            count++;
            continue;
        }

        if (!maybeRemovePo.empty()) {
            checkMaybePos(maybeRemovePo, queuedToRemovePo, finishedAlivePo);
        }
        
        if (!maybeRemoveMove.empty()) {
            checkMaybeMoves(maybeRemoveMove, queuedToRemoveMove, finishedAliveMoves);
        }
    }
    for (auto i: removedPOs_) {
        ddg().dropProgramOperation(i);
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "count of removed POs: " << count << std::endl;
#endif
    return count!=0;
}

void BFRemoveLoopChecksAndJump::checkAlivePO(
    POSet& queuedAlivePo, POSet& finishedAlivePo,
    DataDependenceGraph::NodeSet& queuedAliveMoves,
    DataDependenceGraph::NodeSet& finishedAliveMoves) {
    ProgramOperationPtr po = *queuedAlivePo.begin();
   for (int i = 0; i < po->inputMoveCount(); i++) {
        MoveNode& mn = po->inputMove(i);
        int ind = ddg().inDegree(mn);
        for (int j = 0; j < ind; j++) {
            DataDependenceEdge& e = ddg().inEdge(mn,j);
            if (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
                e.dependenceType() == DataDependenceEdge::DEP_WAR) {
                continue;
            }
            queueAliveMove(ddg().tailNode(e), queuedAlivePo, finishedAlivePo,
                           queuedAliveMoves, finishedAliveMoves);
        }
   }
   queuedAlivePo.erase(po);
   finishedAlivePo.insert(po);
}

void BFRemoveLoopChecksAndJump::queueAliveMove(
    MoveNode& mn, 
    POSet& queuedAlivePo, POSet& finishedAlivePo,
    DataDependenceGraph::NodeSet& queuedAliveMoves,
    DataDependenceGraph::NodeSet& finishedAliveMoves) {

    if (mn.isSourceOperation()) {
        if (finishedAlivePo.find(mn.sourceOperationPtr()) == finishedAlivePo.end()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "alive PO: " << mn.toString() << std::endl;
#endif
            queuedAlivePo.insert(mn.sourceOperationPtr());
        }
    } else if (mn.isDestinationOperation()) {
        if (finishedAlivePo.find(mn.destinationOperationPtr()) == finishedAlivePo.end()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "alive PO: " << mn.toString() << std::endl;
#endif
            queuedAlivePo.insert(mn.destinationOperationPtr());
        }
    } else {
        if (finishedAliveMoves.find(&mn) == finishedAliveMoves.end()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "alive MN: " << mn.toString() << std::endl;
#endif
            queuedAliveMoves.insert(&mn);
        }
    }
}

void BFRemoveLoopChecksAndJump::checkAliveMove(
    DataDependenceGraph::NodeSet& queuedAliveMoves,
    DataDependenceGraph::NodeSet& finishedAliveMoves,
    POSet& queuedAlivePo, POSet& finishedAlivePo) {
    MoveNode* mn = *queuedAliveMoves.begin();
    int ind = ddg().inDegree(*mn);
    for (int j = 0; j < ind; j++) {
        DataDependenceEdge& e = ddg().inEdge(*mn,j);
        if (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
            e.dependenceType() == DataDependenceEdge::DEP_WAR) {
            continue;
        }
        queueAliveMove(ddg().tailNode(e), queuedAlivePo, finishedAlivePo,
                       queuedAliveMoves, finishedAliveMoves);
    }
    queuedAliveMoves.erase(mn);
    finishedAliveMoves.insert(mn);
}





void BFRemoveLoopChecksAndJump::removePoFromQueue(
    POSet& queuedToRemovePo, POSet& maybeRemovePo,
    DataDependenceGraph::NodeSet& queuedToRemoveMove, 
    DataDependenceGraph::NodeSet& maybeRemoveMove) {
    ProgramOperationPtr po = *queuedToRemovePo.begin();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Removing programoperation: " << po->toString()
              << std::endl;
#endif
    for (int i = 0; i < po->inputMoveCount(); i++) {
        MoveNode& mn = po->inputMove(i);
        int ind = ddg().inDegree(mn);
        for (int j = 0; j < ind; j++) {
            DataDependenceEdge& e = ddg().inEdge(mn,j);
            if (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
                e.dependenceType() == DataDependenceEdge::DEP_WAR) {
                continue;
            }
            MoveNode& tail = ddg().tailNode(e);
            if (!tail.isSourceOperation()) {
                if (tail.isDestinationOperation()) {
                    ProgramOperationPtr tailOp = tail.destinationOperationPtr();
                    if (queuedToRemovePo.find(tailOp) == queuedToRemovePo.end()) {
                        maybeRemovePo.insert(tailOp);
                    }
                } else {
                    if (queuedToRemoveMove.find(&tail) == queuedToRemoveMove.end()) {
                        maybeRemoveMove.insert(&tail);
                    }
                }
            } else {
                ProgramOperationPtr tailOp = tail.sourceOperationPtr();
                // if already is to be remoed, no need to put tho
                // checking queue again
                if (queuedToRemovePo.find(tailOp) == queuedToRemovePo.end()) {
                    maybeRemovePo.insert(tailOp);
                }
            }
        }
        ddg().dropNode(mn);
        removedMoves_.insert(&mn);
    }
    
    for (int i = 0; i < po->outputMoveCount(); i++) {
        MoveNode& mn = po->outputMove(i);
        ddg().dropNode(mn);
        removedMoves_.insert(&mn);
    }
    
    // TODO: remove the PO from the ddg!
    removedPOs_.insert(po);
    queuedToRemovePo.erase(queuedToRemovePo.begin());
}

void BFRemoveLoopChecksAndJump::removeMoveFromQueue(
    DataDependenceGraph::NodeSet& queuedToRemoveMove, 
    DataDependenceGraph::NodeSet& maybeRemoveMove,
    POSet& queuedToRemovePo, POSet& maybeRemovePo) {
    MoveNode* mn = *queuedToRemoveMove.begin();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tRemoving move: " << mn->toString()
              << std::endl;
#endif
    int ind = ddg().inDegree(*mn);
    for (int j = 0; j < ind; j++) {
        DataDependenceEdge& e = ddg().inEdge(*mn,j);
        if (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
            e.dependenceType() == DataDependenceEdge::DEP_WAR) {
            continue;
        }
        MoveNode& tail = ddg().tailNode(e);
        if (!tail.isSourceOperation()) {
            if (tail.isDestinationOperation()) {
                ProgramOperationPtr tailOp = tail.destinationOperationPtr();
                // if already is to be remoed, no need to put tho
                // checking queue again
                if (queuedToRemovePo.find(tailOp) == queuedToRemovePo.end()) {
                    maybeRemovePo.insert(tailOp);
                }
            } else {
                if (queuedToRemoveMove.find(&tail) == queuedToRemoveMove.end()) {
                    maybeRemoveMove.insert(&tail);
                }
            }
        } else {
            ProgramOperationPtr tailOp = tail.sourceOperationPtr();
            // if already is to be remoed, no need to put tho
            // checking queue again
            if (queuedToRemovePo.find(tailOp) == queuedToRemovePo.end()) {
                maybeRemovePo.insert(tailOp);
            }
        }
    }
    ddg().dropNode(*mn);
    removedMoves_.insert(mn);
    
    // TODO: remove the PO from the ddg!
    removedMoves_.insert(mn);
    queuedToRemoveMove.erase(queuedToRemoveMove.begin());
}

void BFRemoveLoopChecksAndJump::checkMaybePos(POSet& maybeRemove, POSet& queuedToRemove, POSet& alivePos) {
    ProgramOperationPtr po = *maybeRemove.begin();
    if (alivePos.find(po) == alivePos.end()) {
        queuedToRemove.insert(po);
    }
    maybeRemove.erase(maybeRemove.begin());
}

void BFRemoveLoopChecksAndJump::checkMaybeMoves(
    DataDependenceGraph::NodeSet& maybeRemove,
    DataDependenceGraph::NodeSet& queuedToRemove,
    DataDependenceGraph::NodeSet& aliveNodes) {
    MoveNode* mn = *maybeRemove.begin();
    if (aliveNodes.find(mn) == aliveNodes.end()) {
        queuedToRemove.insert(mn);
    }
    maybeRemove.erase(mn);
}

void
BFRemoveLoopChecksAndJump::undoOnlyMe() {
    for (auto i: removedMoves_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Restoring node: " << (*i).toString() << std::endl;
#endif
        ddg().restoreNodeFromParent(*i);
    }
    removedMoves_.clear();

    for (auto i: removedPOs_) {
        ddg().addProgramOperation(i);
    }
}
