/**
 * @file GraphEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "GraphEdge.hh"
#include "Conversion.hh"

GraphEdge::~GraphEdge() { }

GraphEdge::GraphEdge() : edgeID_(edgeCounter_++) {}

GraphEdge::GraphEdge(const GraphEdge& edge) : edgeID_(edgeCounter_++) {}

/**
 * Clones edge. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the edge. 
 */
GraphEdge* 
GraphEdge::clone() const {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Cloning must be done in subclass or use copy constructor.");
    return NULL;
}

/**
 * Return the label of the edge as a string. 
 *
 * Used for printing graph in GraphViz .dot file.
 *
 * @return The edge ID as a string
 */
std::string
GraphEdge::toString() const {
    return Conversion::toString(edgeID());
}

/**
 * Returns the string that should be placed in the edge's properties section in
 * the GraphViz Dot string.
 *
 * This can be overridden in the derived class to add different properties (e.g.,
 * colors, etc.) to different type of edges. The default implementation only
 * sets the label property.
 *
 * @return String describing the Dot properties of the edge.
 */
std::string 
GraphEdge::dotString() const {
    return std::string("label=\"") + toString() + "\"";
}


/**
 * Return edge ID as integer.
 *
 * @return The edge ID as integer
 */
int
GraphEdge::edgeID() const {
    return edgeID_;
}

/**
 * Compariron based on edge ID's for maps and sets.
 */
bool 
GraphEdge::Comparator::operator()(GraphEdge* e1, GraphEdge* e2) const {
    return e1->edgeID_ < e2->edgeID_;
}

int GraphEdge::edgeCounter_ = 0;
