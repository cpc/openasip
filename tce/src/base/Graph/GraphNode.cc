/**
 * @file GraphNode.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "GraphNode.hh"
#include "Conversion.hh"

GraphNode::GraphNode() : nodeID_(idCounter_++) {}

GraphNode::~GraphNode() { }

/**
 * Clones node. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the node. 
 */
GraphNode* 
GraphNode::clone() const {
    return new GraphNode(*this);
}

/**
 * Returns a node description as a string. 
 *
 * Used for printing graph into .dot file. This is used for a label in 
 * the default dotString() implementation.
 *
 * @return The node ID as a string.
 */
std::string
GraphNode::toString() const {
    return Conversion::toString(nodeID());
}

/**
 * Returns the string that should be placed in the node's properties section 
 * in the GraphViz Dot string.
 *
 * This can be overridden in the derived class to add different properties 
 * (e.g., colors, etc.) to different type of nodes. The default 
 * implementation only sets the label property.
 *
 * @return String describing the Dot properties of the Node.
 */
std::string 
GraphNode::dotString() const {
    return std::string("label=\"") + toString() + "\"";
}

/**
 * Returns a node ID as integer.
 *
 * This id is unique among all the nodes in the graph.
 *
 * @return The nodeID as integer.
 */
int
GraphNode::nodeID() const {
    return nodeID_;
}

/**
 * Comparison based on node ID's for maps and sets.
 */
bool 
GraphNode::Comparator::operator()(
    const GraphNode* mn1, const GraphNode* mn2) const {
    if (mn1 == NULL) {
        return false;
    }
    if (mn2 == NULL) {
        return true;
    }
    return mn1->nodeID() < mn2->nodeID();
}

int GraphNode::idCounter_ = 0;
