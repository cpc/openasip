/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
