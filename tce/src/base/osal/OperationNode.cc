/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OperationNode.cc
 *
 * Implementation of OperationNode class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "OperationNode.hh"
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
