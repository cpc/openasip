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
 * @file ReadyMoveNodeGroupList.hh
 *
 * Declaration of ReadyMoveNodeGroupList class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
