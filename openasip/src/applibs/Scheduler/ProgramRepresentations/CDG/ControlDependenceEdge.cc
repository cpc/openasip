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
 * @file ControlDependenceEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * control dependence edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "ControlDependenceEdge.hh"

ControlDependenceEdge::~ControlDependenceEdge() {}

/**
 * Returns a predicate of edge as string.
 * @return The value of edge - true, false, neutral
 */
TCEString
ControlDependenceEdge::toString() const {
    TCEString label = "";
    if (isTrueEdge()) {
        label += "True";
    }
    if (isFalseEdge()) {
        label += "False";
    }
    if (isLoopCloseEdge()) {
        label += "Close";
    }
    return label;
}

/**
 * Inverts the predicate of the edge
 * @todo this should be handled better, after the ownership of CDG is moved
 * into the PDG or so
 */
void
ControlDependenceEdge::invertEdgePredicate(){
    if (isNormalEdge()) {
        return;
    }
    if (isTrueEdge()) {
        edgeType_ = CDEP_EDGE_FALSE;
    } else {
        edgeType_ = CDEP_EDGE_TRUE;
    }
}
