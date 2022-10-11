#include "BFOptimization.hh"
#include "DataDependenceGraph.hh"

class BFRemoveLoopChecksAndJump : public BFOptimization {
public:
    BFRemoveLoopChecksAndJump(BF2Scheduler& sched) : 
        BFOptimization(sched) {}
    bool operator()();
    void undoOnlyMe();
private:

    typedef std::set<ProgramOperationPtr, ProgramOperationPtrComparator> 
        POSet;

    void removePoFromQueue(
        POSet& queuedToRemovePo, POSet& maybeRemovePo,
        DataDependenceGraph::NodeSet& queuedToRemoveMove, 
        DataDependenceGraph::NodeSet& maybeRemoveMove);

    void checkMaybePos(
        POSet& maybeRemove, POSet& queuedToRemove, POSet& finishedAlivePo);

    void removeMoveFromQueue(
        DataDependenceGraph::NodeSet& queuedToRemoveMove, 
        DataDependenceGraph::NodeSet& maybeRemoveMove,
        POSet& queuedToRemovePo, POSet& maybeRemovePo);

    void checkMaybeMoves(
        DataDependenceGraph::NodeSet& maybeRemove,
        DataDependenceGraph::NodeSet& queuedToRemove,
        DataDependenceGraph::NodeSet& aliveNodes);

    void checkAlivePO(
        POSet& queuedAlivePo, POSet& finishedAlivePo,
        DataDependenceGraph::NodeSet& queuedAliveMoves,
        DataDependenceGraph::NodeSet& finishedAliveMoves);
    
    void checkAliveMove(DataDependenceGraph::NodeSet& queuedAliveMoves,
                    DataDependenceGraph::NodeSet& finishedAliveMoves,
                    POSet& queuedAlivePo, POSet& finishedAlivePo);

    void queueAliveMove(
        MoveNode& mn, 
        POSet& queuedAlivePo, POSet& finishedAlivePo,
        DataDependenceGraph::NodeSet& queuedAliveMoves,
        DataDependenceGraph::NodeSet& finishedAliveMoves);

    DataDependenceGraph::NodeSet removedMoves_;    
    std::set<ProgramOperationPtr, ProgramOperationPtrComparator> removedPOs_;
};
