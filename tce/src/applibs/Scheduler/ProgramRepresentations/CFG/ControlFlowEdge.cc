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
 * @file ControlFlowEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "ControlFlowEdge.hh"

/**
 * Constructor creates Control Flow Edge of given type
 *
 * @param edgePredicate The truth value of edge (or normal)
 * @param edgeType Define if edge represents jump, if false the it is call
 */
ControlFlowEdge::ControlFlowEdge(
    CFGEdgePredicate edgePredicate,
    CFGEdgeType edgeType) :
    edgePredicate_(edgePredicate), edgeType_(edgeType), backEdge_(false) {
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
TCEString
ControlFlowEdge::toString() const {
    TCEString result = "";
    if (isJumpEdge()) {
        result += "Jump_";
    }
    if (isFallThroughEdge()) {
        result += "FallThrough_";
    }
    if (isCallPassEdge()) {
        result += "CallPass_";
    }
    if (isBackEdge()) {
        result += "BackEdge_";
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
    return edgeType_ == CFLOW_EDGE_CALL;
}

/**
 * Returns true if the edge is representing jump.
 *
 * @return True if the edge represents jump.
 */
bool
ControlFlowEdge::isJumpEdge() const {
    return edgeType_ == CFLOW_EDGE_JUMP;
}

/**
 * Returns true if the edge is representing fall through.
 *
 * @return True if the edge represents fall through.
 */
bool
ControlFlowEdge::isFallThroughEdge() const {
    return edgeType_ == CFLOW_EDGE_FALLTHROUGH;
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

/**
 * Returns true if edge is loop back edge
 *
 * @return True if edge is loop back edge
 */
bool
ControlFlowEdge::isBackEdge() const {
    return backEdge_;
}

