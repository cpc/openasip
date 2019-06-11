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
 * @file MoveNodeGroup.cc
 *
 * Implementation of MoveNodeGroup class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include <climits>

#include "MoveNodeGroup.hh"
#include "Application.hh"
#include "DataDependenceGraph.hh"
#include "POMDisassembler.hh"
#include "ProgramOperation.hh"

/**
 * Constructor.
 */
MoveNodeGroup::MoveNodeGroup() : ddg_(NULL) {
}

/**
 * Constructor.
 *
 * @param ddg The data dependence graph the moves in the group belong to.
 */
MoveNodeGroup::MoveNodeGroup(const DataDependenceGraph& ddg) : 
    ddg_(&ddg) {
}


/**
 * Destructor.
 */
MoveNodeGroup::~MoveNodeGroup() {
}

/**
 * Adds a node to the MoveNodeGroup.
 *
 * @param node The MoveNode being added to the group.
 */
void
MoveNodeGroup::addNode(MoveNode& node) {
    nodes_.push_back(&node);
}

/**
 * Calculates the last cycle in which dependences make it possible to
 * try schedule the leader node.
 *
 * @return The earliest possible cycle
 */
int
MoveNodeGroup::latestCycle() const
    throw (NotAvailable) {
    abortWithError("Not yet implemented");
    return 0;
}

/**
 * Calculates the earliest cycle in which dependences make it possible to
 * try schedule the leader node.
 *
 * In case the MoveNodeGroup is not (known) to be part of a DDG, or
 * at least one of the predecessors of the MoveNodeGroup are unscheduled,
 * this function returns INT_MAX. If the node is a root node, the function
 * returns 0.
 *
 * @todo What is the "leader node", in general, in an unassigned operation?
 *
 * @return The earliest possible cycle all nodes could be scheduled according
 * to their precedences.
 */
int
MoveNodeGroup::earliestCycle() const {

    if (ddg_ == NULL)
        return INT_MAX;

    int cycle = 0;
    // check the predecessors of all nodes in the MoveNodeGroup
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        MoveNode& node = *nodes_.at(i);
        int nodeCycle = ddg_->earliestCycle(node);

        // the result read moves have unscheduled predecessors in case
        // the operand moves are not yet scheduled, ignore that
        if (nodeCycle == INT_MAX) {
            if (!node.isSourceOperation()) {
                return INT_MAX;
            }
        } else {
            cycle = std::max(nodeCycle, cycle);
        }
    }

    return cycle;
}

/**
 * Returns the number of movenodes in the group
 *
 * @return The number of nodes.
 */
int
MoveNodeGroup::nodeCount() const {
    return nodes_.size();
}

/**
 * Returns one MoveNode contained in this group.
 *
 * @param index The index of node which to return
 *
 * @return The node being returned.
 */
MoveNode&
MoveNodeGroup::node(int index) const {
    return *nodes_.at(index);
}

/**
 * Returns true if all moves in the move group have been scheduled.
 *
 * An empty MoveNodeGroup is considered always scheduled.
 *
 * @return True if scheduled.
 */
bool
MoveNodeGroup::isScheduled() const {
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        if (!nodes_.at(i)->isScheduled())
            return false;
    }
    return true;
}

/**
 * Returns true if all moves in the move group are alive.
 *
 * An empty MoveNodeGroup is considered always alive.
 *
 * @return True if scheduled.
 */
bool
MoveNodeGroup::isAlive() const {
    if (ddg_ != NULL) {
        for (std::size_t i = 0; i < nodes_.size(); ++i) {
            if (!ddg_->hasNode(*nodes_.at(i))) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Returns the maximum distance from any movenode in the group to any sink.
 *
 * Calling this may cause forever loop if the DDG has loops.
 *
 * @return maximum distance from any movenode in the group to any sink.
 */
int
MoveNodeGroup::maxSinkDistance() const {
    if (ddg_ == NULL) {
        return 0;
    }
    int maxSD = 0;
    for (int i = 0; i < nodeCount(); i++) {
        int sinkDistance = ddg_->maxSinkDistance(node(i));
        maxSD = std::max (maxSD, sinkDistance);
    }
    return maxSD;
}

/**
 * Returns the maximum distance from any source node to any movenode
 * in the group.
 *
 * Calling this may cause forever loop if the DDG has loops.
 *
 * @return maximum distance from any source node to any node in the group.
 */
int
MoveNodeGroup::maxSourceDistance() const {
    if (ddg_ == NULL) {
        return 0;
    }
    int maxSD = 0;
    for (int i = 0; i < nodeCount(); i++) {
        int sourceDistance = ddg_->maxSourceDistance(node(i));
        maxSD = std::max (maxSD, sourceDistance);
    }
    return maxSD;
}

/**
 *  Returns the disassembly of moves in NodeGroup as a string.
 * 
 * @return string with disasembly of NodeGroup moves.
 */
std::string
MoveNodeGroup::toString() const {
    std::string result = "";
    for (int i = 0; i < nodeCount(); i++) {
        result += node(i).toString() + " ; ";
    }

    if (isOperation()) {
        result += "[PO: " + programOperationPtr()->toString() + "] ";
    }
    return result;
}
