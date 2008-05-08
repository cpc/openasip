/**
 * @file ConstantNode.cc
 *
 * Implementation of ConstantNode class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "ConstantNode.hh"
#include "Conversion.hh"

/**
 * Constructor.
 */
ConstantNode::ConstantNode(int value) :
    OperationDAGNode(), value_(value) {}

/**
 * Cpoy constructor.
 */
ConstantNode::ConstantNode(const ConstantNode& other) :
    OperationDAGNode(), value_(other.value()) {}

/**
 * Clones node. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the node. 
 */
GraphNode* 
ConstantNode::clone() const {
    return new ConstantNode(*this);
}

int 
ConstantNode::value() const {
    return value_;
}

std::string 
ConstantNode::toString() const {
    return "(" + Conversion::toString(value_) + ")";
}
