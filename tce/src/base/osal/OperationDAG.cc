/**
 * @file OperationDAG.cc
 *
 * Implementation of operation directed acyclic graph class
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "OperationDAG.hh"
#include "OperationDAGEdge.hh"
#include "OperationNode.hh"

OperationDAG OperationDAG::null("NULL_DAG");

/**
 * Constructor.
 *
 * @param name The graph can be named for debugging purposes.
 */
OperationDAG::OperationDAG(const std::string& name) : 
     BoostGraph<OperationDAGNode,OperationDAGEdge>(name) {
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
 * Deletes all MoveNodes and ProgramOperations.
 */
OperationDAG::~OperationDAG() {
    for (int i = 0; i < edgeCount(); i++) {
        delete &edge(i);
    }
    
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
const std::set<OperationDAGNode*>&
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
