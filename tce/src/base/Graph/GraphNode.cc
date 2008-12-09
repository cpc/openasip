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
 * @file GraphNode.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
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
