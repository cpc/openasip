/**
 * @file OperationNode.cc
 *
 * Implementation of OperationNode class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */


#include "OperationNode.hh"
#include "OperationDAG.hh"
#include "Operation.hh"
#include "TCEString.hh"

/**
 * Constructor.
 *
 * @param 
 */

OperationNode::OperationNode(Operation& op) :
    OperationDAGNode(), referencedOperation_(&op) {
}

OperationNode::OperationNode(const OperationNode& other) : 
    OperationDAGNode(), referencedOperation_(&(other.referencedOperation())) {
}


/**
 * Clones node. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the node. 
 */
GraphNode* 
OperationNode::clone() const {
    return new OperationNode(*this);
}

/**
 * Returns operation that is referred from current node.
 *
 * @return Operation that is referred from current node.
 */
Operation&
OperationNode::referencedOperation() const {
    assert(referencedOperation_ != NULL);
    return *referencedOperation_;
}

std::string 
OperationNode::toString() const {
    return referencedOperation_->name();
}
