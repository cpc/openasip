/**
 * @file ReadyMoveNodeGroupList.hh
 *
 * Declaration of ReadyMoveNodeGroupList class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_READY_MOVE_NODE_GROUP_LIST_HH
#define TTA_READY_MOVE_NODE_GROUP_LIST_HH

#include <vector>
#include <queue>

/**
 * A function object for prioritizing the ready list according to the move's
 * distance from farthest sink node.
 *
 */
class RLPriorityCriticalPath:
    public std::binary_function<MoveNode*, MoveNode*, bool> {
public:
    /**
     * Compares two nodes according to their priority in the ready list.
     *
     * @param a Node a.
     * @param b Node b.
     * @return True if b should be scheduled before a (b greater than a).
     */
    bool operator()(MoveNodeGroup& a, MoveNodeGroup& b) {

        if (b.isScheduled()) {
            // keep scheduled MoveNodeSets on a top of a queue
            // so they will be poped out
            return true;
        }
        if (a.isScheduled()) {
            // keep scheduled MoveNodeSets on a top of a queue
            // so they will be poped out
            return false;
        }
        // Compute distances only once, it is expensive operation on graph
        int aSinkDistance = a.maxSinkDistance();
        int bSinkDistance = b.maxSinkDistance();

        if (bSinkDistance == aSinkDistance) {
            return b.node(0).nodeID() < a.node(0).nodeID();
        }
        // the higher the sink distance, the higher the priority
        return bSinkDistance > aSinkDistance;
    }
};

/// A prioritized list for the ready-to-be-scheduled move node groups.
typedef std::priority_queue<
    MoveNodeGroup, std::vector<MoveNodeGroup>,
    RLPriorityCriticalPath> ReadyMoveNodeGroupList;

#endif
