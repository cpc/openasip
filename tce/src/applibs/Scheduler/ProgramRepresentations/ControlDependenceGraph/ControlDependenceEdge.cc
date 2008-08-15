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
 * @file ControlDependenceEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * control dependence edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ControlDependenceEdge.hh"

ControlDependenceEdge::~ControlDependenceEdge() {}

/**
 * Returns a predicate of edge as string.
 * @return The value of edge - true, false, neutral
 */
std::string
ControlDependenceEdge::toString() const {
    if (isTrueEdge()) {
        return "True";
    }
    if (isFalseEdge()) {
        return "False";
    }
    return "";
}

/**
 * Returns true if edge type is oridinary control dependence edge
 * @return true if edge is normal
 */
bool
ControlDependenceEdge::isNormalEdge() const {
    return edgeType_ == CDEP_EDGE_NORMAL;
}
/**
 * Returns true if edge type is true
 * @return true if edge is true
 */
bool
ControlDependenceEdge::isTrueEdge() const {
    return edgeType_ == CDEP_EDGE_TRUE;
}
/**
 * Returns true if edge type is false
 * @return true if edge predicate is false
 */
bool
ControlDependenceEdge::isFalseEdge() const {
    return edgeType_ == CDEP_EDGE_FALSE;
}
/**
 * Returns the type of the edge
 * @return the predicate of edge
 */
ControlDependenceEdge::CDGEdgeType
ControlDependenceEdge::edgeType() const {
    return edgeType_;
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
