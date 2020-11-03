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
 * @file ControlDependenceEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * control dependence edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_DEPENDENCE_EDGE_HH
#define TTA_CONTROL_DEPENDENCE_EDGE_HH

#include "GraphEdge.hh"

/**
 * Edge of the control dependence graph (control dependence edge). Each edge
 * implies control dependency.
 */
class ControlDependenceEdge : public GraphEdge {
public:
    friend class ProgramDependenceGraph;
    enum CDGEdgeType {
        CDEP_EDGE_NORMAL,
        CDEP_EDGE_TRUE,
        CDEP_EDGE_FALSE,
        CDEP_EDGE_LOOPCLOSE};

    ControlDependenceEdge(
        CDGEdgeType edgeType = CDEP_EDGE_NORMAL) :
        edgeType_(edgeType) {}

    virtual ~ControlDependenceEdge();

    TCEString toString() const;
    bool isNormalEdge() const { return edgeType_ == CDEP_EDGE_NORMAL; }
    bool isTrueEdge() const { return edgeType_ == CDEP_EDGE_TRUE; }
    bool isFalseEdge() const { return edgeType_ == CDEP_EDGE_FALSE; }
    bool isLoopCloseEdge() const { return edgeType_ == CDEP_EDGE_LOOPCLOSE;}
    CDGEdgeType edgeType() const { return edgeType_; }

protected:
    void invertEdgePredicate();
private:
    CDGEdgeType edgeType_;
};

#endif
