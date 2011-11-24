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
 * @file ControlDependenceNode.cc
 *
 * Prototype control dependence graph of TTA program representation:
 * implementation of graph node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "BasicBlockNode.hh"
#include "BasicBlock.hh"
#include "ControlDependenceNode.hh"
#include "Exception.hh"


/**
 * Returns instruction at given index in basic block.
 *
 * @param index Index of instruction in basic block
 * @return The instruction at given index
 */
TTAProgram::Instruction&
ControlDependenceNode::instruction(int index) const {
    if (!isBBNode()) {
        std::string msg = "Trying to read from non basic block node!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    return code_->basicBlock().instructionAtIndex(index);
}

/**
 * Returns the type of basic block as string.
 *
 * @note Used in writting graph to .dot file.
 * @return The type of basic block
 */
std::string
ControlDependenceNode::toString() const {
    std::string result = "";
    if (isRegionNode()) {
        result += "Region";
        return result;
    }
    if (isEntryNode()) {
        result += "Entry";
        return result;
    }
    if (isExitNode()) {
        result += "Exit";
        return result;
    }
    if (isPredicateNode()) {
        result += "Predicate";
        return result;
    }
    if (isBBNode()) {
        return code_->toString();
    }
    return result;
}

/**
 * Returns a basic block refered by the control dependence node
 * @return the Basic block
 */
BasicBlockNode*
ControlDependenceNode::basicBlockNode() const {
    if (isBBNode()) {
        return code_;
    }
    std::string msg = "Trying to read from non basic block node!";
    throw InvalidData(__FILE__, __LINE__, __func__, msg);
    return NULL;
}

/**
 * Returns number of instructions in basic block
 * @return number of instructions
 */
int
ControlDependenceNode::instructionCount() const {
    if (isBBNode()) {
        return code_->basicBlock().instructionCount();
    }
    std::string msg = "Trying to read from non basic block node!";
    throw InvalidData(__FILE__, __LINE__, __func__, msg);
    return 0;
}

bool
ControlDependenceNode::isControlDependenceNode() const {
    return true;
}

bool
ControlDependenceNode::isRegionNode() const {
    return type_ == CDEP_NODE_REGION;
}

bool
ControlDependenceNode::isPredicateNode() const {
    return type_ == CDEP_NODE_PREDICATE;
}

bool
ControlDependenceNode::isBBNode() const {
    return type_ == CDEP_NODE_BB;
}

bool
ControlDependenceNode::isEntryNode() const {
    return type_ == CDEP_NODE_BB && code_->isEntryBB();
}

bool
ControlDependenceNode::isExitNode() const {
    return type_ == CDEP_NODE_BB && code_->isExitBB();
}

ControlDependenceNode::NodeType
ControlDependenceNode::type() const {
    return type_;
}
