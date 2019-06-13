/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ControlFlowEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_FLOW_EDGE_HH
#define TTA_CONTROL_FLOW_EDGE_HH

#include "GraphEdge.hh"

/**
 * Edge of the control flow graph (control flow edge). Each edge implies a
 * possible change of execution flow from the source basic block to the
 * target basic block.
 *
 */
class ControlFlowEdge : public GraphEdge {
public:
    enum CFGEdgePredicate {
        CFLOW_EDGE_NORMAL,
        CFLOW_EDGE_TRUE,
        CFLOW_EDGE_FALSE,
        CFLOW_EDGE_LOOP_BREAK,
        CFLOW_EDGE_FAKE
    };
    enum CFGEdgeType {
        CFLOW_EDGE_JUMP,
        CFLOW_EDGE_CALL,
        CFLOW_EDGE_FALLTHROUGH
    };

    ControlFlowEdge(
        CFGEdgePredicate edgePredicate = CFLOW_EDGE_NORMAL,
        CFGEdgeType edgeType = CFLOW_EDGE_JUMP);

    virtual ~ControlFlowEdge();

    bool isControlFlowEdge() const;
    bool isNormalEdge() const;
    bool isTrueEdge() const;
    bool isFalseEdge() const;
    bool isCallPassEdge() const;
    bool isJumpEdge() const;
    bool isFallThroughEdge() const;
    bool isLoopBreakEdge() const;
    bool isBackEdge() const;
    TCEString toString() const;
    void setPredicate(CFGEdgePredicate pred) { edgePredicate_ = pred; }

    CFGEdgePredicate edgePredicate() { return edgePredicate_; }
    CFGEdgeType edgeType() { return edgeType_; }

    /// Add property to edge to mark is as back edge - loop edge
    /// DO NOT USE unless you know what you are doing!!!
    void setBackEdge() { backEdge_ = true;}

private:
    CFGEdgePredicate edgePredicate_;
    CFGEdgeType edgeType_;
    bool backEdge_;
};

#endif
