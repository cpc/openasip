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
 * @file ControlFlowEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ControlFlowEdge.hh"

/**
 * Constructor creates Control Flow Edge of given type
 *
 * @param edgePredicate The truth value of edge (or normal)
 * @param isJump Define if edge represents jump, if false the it is call
 */
ControlFlowEdge::ControlFlowEdge(
    CFGEdgePredicate edgePredicate,
    bool isJump) :
    edgePredicate_(edgePredicate), isJumpEdge_(isJump) {
}

/**
 * Destructor
 */
ControlFlowEdge::~ControlFlowEdge() { }

/**
 * Returns type as a string. Helper for graph output to dot file.
 *
 * @return String representing type of edge.
 */
std::string
ControlFlowEdge::toString() const {
    std::string result = "";
    if (isJumpEdge()) {
        result += "Jump_";
    }
    if (isCallPassEdge()) {
        result += "CallPass_";
    }
    if (isNormalEdge()) {
        result += "normal";
    }
    if (isTrueEdge()) {
        result += "true";
    }
    if (isFalseEdge()) {
        result += "false";
    }
    if (isLoopBreakEdge()) {
        result += "break";
    }
    return result;
}

/**
 * Returns true if object represents ControlFlowEdge.
 *
 * @return True if object is control flow edge
 */
bool
ControlFlowEdge::isControlFlowEdge() const {
    return true;
}
/**
 * Returns true if the edge is representing unconditional jump
 *
 * @return True if jump is not guarded
 */
bool
ControlFlowEdge::isNormalEdge() const {
    return edgePredicate_ == CFLOW_EDGE_NORMAL;
}

/**
 * Returns true if edge represents conditional control flow that is
 * taken if condition evaluates to true.
 *
 * @return True if edge represents True path
 */
bool
ControlFlowEdge::isTrueEdge() const {
    return edgePredicate_ == CFLOW_EDGE_TRUE;
}
/**
 * Returns true if edge represents conditional control flow that is
 * taken if condition evaluates to false.
 *
 * @return True if edge represents False path
 */
bool
ControlFlowEdge::isFalseEdge() const {
    return edgePredicate_ == CFLOW_EDGE_FALSE;
}
/**
 * Returns true if edge represents control flow over the call in code.
 *
 * @return True if edge is split in basic block when call happened.
 */
bool
ControlFlowEdge::isCallPassEdge() const {
    return !isJumpEdge();
}

/**
 * Returns true if the edge is representing jump.
 *
 * @return True if the edge represents jump.
 */
bool
ControlFlowEdge::isJumpEdge() const {
    return isJumpEdge_ == true;
}

/**
 * Returns true if edge was added to break infinite
 * loop for control dependence computation.
 *
 * @return True if edge was added to break infinite loop
 */
bool
ControlFlowEdge::isLoopBreakEdge() const {
    return edgePredicate_ == CFLOW_EDGE_LOOP_BREAK;
}

