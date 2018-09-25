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
 * @file OperationDAG.cc
 *
 * Implementation of operation directed acyclic graph class
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "OperationDAG.hh"
#include "OperationDAGEdge.hh"
#include "OperationNode.hh"
#include "OperationPimpl.hh"
#include "Operation.hh"

OperationDAG OperationDAG::null;

OperationDAG::OperationDAG() :
     BoostGraph<OperationDAGNode,OperationDAGEdge>("NULL_DAG") {
}

/**
 * Constructor.
 *
 * @param name The graph can be named for debugging purposes.
 */
OperationDAG::OperationDAG(const OperationPimpl& op) :
    BoostGraph<OperationDAGNode,OperationDAGEdge>(op.name()),
    op_(&op) {
}

/**
 * Copy constructor.
 *
 * @param name The graph can be named for debugging purposes.
 */
OperationDAG::OperationDAG(const OperationDAG& other) : 
    BoostGraph<OperationDAGNode,OperationDAGEdge>(other) {
}

/**
 * Returns true if dag is the simplest possible.
 *
 * @return true if dag is the simplest possible.
 */
bool
OperationDAG::isTrivial() const { 
    int opNodeCount = 0;
    for (int i = 0; i < nodeCount(); i++) {
        
        if (dynamic_cast<OperationNode*>(&node(i)) != NULL) {
            opNodeCount++;
            if (opNodeCount > 1) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Destructor.
 *
 * Deletes all Nodes. Edges are destroyed by destructor of base class.
 */
OperationDAG::~OperationDAG() {
    for (int i = 0; i < nodeCount(); i++) {
        delete &node(i);
    }
}

/**
 * Finds recursively step count to root for an operation and writes data to
 * operation step map.
 *
 * Once O(n), after O(log(n))
 *
 * @param node Node, whose step count is wanted.
 * @return Number of maximum steps that are needed to reach this 
 *         node from root nodes.
 */
int 
OperationDAG::stepsToRoot(const OperationDAGNode& node) const {
    
    if (stepMap_.find(&node) == stepMap_.end()) {
        int maxSteps = 0;

        for (int i = 0; i < inDegree(node); i++) {
            OperationDAGNode& rootNode = tailNode(inEdge(node, i));
            int routeSteps = stepsToRoot(rootNode);            
            if (routeSteps > maxSteps) {
                maxSteps = routeSteps;
            }
        }
        
        if (inDegree(node) == 0) {
            stepMap_[&node] = 0;
        } else {
            stepMap_[&node] = maxSteps + 1;
        }
    }

    return stepMap_[&node];
}

/**
 * Returns set of end nodes of a DAG.
 *
 * @return Set of end nodes of a DAG. 
 */
const OperationDAG::NodeSet&
OperationDAG::endNodes() const {
    if (endNodes_.empty()) {
        for (int i = 0; i < nodeCount(); i++) {
            OperationDAGNode& curr = node(i);
            if (outDegree(curr) == 0) {
                endNodes_.insert(&curr);
            }
        }
    }
    return endNodes_;
} 
