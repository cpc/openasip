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
