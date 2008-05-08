/**
 * @file TerminalNode.cc
 *
 * Implementation of TerminalNode class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "TerminalNode.hh"
#include "Conversion.hh"

/**
 * Constructor.
 */
TerminalNode::TerminalNode(int operandIndex) :
    OperationDAGNode(), operandIndex_(operandIndex) {}

/**
 * Copoy constructor.
 */
TerminalNode::TerminalNode(const TerminalNode& other) :
    OperationDAGNode(), operandIndex_(other.operandIndex()) {}

/**
 * Clones node. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the node. 
 */
GraphNode* 
TerminalNode::clone() const {
    return new TerminalNode(*this);
}

int 
TerminalNode::operandIndex() const {
    return operandIndex_;
}

std::string 
TerminalNode::toString() const {
    return "IO(" + Conversion::toString(operandIndex_) + ")";
}
