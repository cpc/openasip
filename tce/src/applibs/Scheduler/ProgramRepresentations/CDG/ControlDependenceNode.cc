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
#include "Conversion.hh"
#include "Application.hh"

/**
 * Destructor.
 */
ControlDependenceNode::~ControlDependenceNode() {
    region_.clear();
    eec_.clear();
    pseudoPredicateEEC_.clear();
}
/**
 * Returns instruction at given index in basic block.
 *
 * @param index Index of instruction in basic block
 * @return The instruction at given index
 */
TTAProgram::Instruction&
ControlDependenceNode::instruction(int index) const {
    if (!isBBNode()) {
        std::string msg = 
            "Trying to read from non basic block node" + toString() + "!";
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
   TCEString result = "";
   if (isRegionNode()) {
      result += "Region " + Conversion::toString(nodeID());
      if (isLastNode()) {
          result += "_LAST";
      }
      return result;
   }
   if (isLoopEntryNode()) {
      result += "Loop Entry " + Conversion::toString(nodeID());
      return result;
   }
   if (isEntryNode()) {
      result += "Entry";
      return result;
   }
   if (isLoopCloseNode()) {
      result += "Close " + Conversion::toString(nodeID());
      return result;
   }

   if (isExitNode()) {
      result += "Exit";
      return result;
   }
   if (isPredicateNode()) {
      result += "Predicate(" + code_->toString() +")";
      if (isLastNode()) {
          result += "_LAST";
      }
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
    if (isBBNode() || isPredicateNode()) {
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
    /// Predicate nodes are BB which ends with conditional jump in CFG
    return (type_ == CDEP_NODE_BB) || isPredicateNode();
}

bool
ControlDependenceNode::isEntryNode() const {
    return type_ == CDEP_NODE_BB && code_->isEntryBB();
}

bool
ControlDependenceNode::isExitNode() const {
    return type_ == CDEP_NODE_BB && code_->isExitBB();
}

bool
ControlDependenceNode::isLoopEntryNode() const {
    return type_ == CDEP_NODE_LOOPENTRY;
}

bool
ControlDependenceNode::isLoopEntryNode(int component) const {
    return type_ == CDEP_NODE_LOOPENTRY && component == component_;
}

bool
ControlDependenceNode::isLoopCloseNode() const {
    return type_ == CDEP_NODE_LOOPCLOSE;
}

void
ControlDependenceNode::setLoopEntryNode(int component) {
    if (!isRegionNode() && !isEntryNode()) {
        TCEString msg = "Loop entry node \'" + toString();
        msg += "\' is not a Region node!";
        throw InvalidData(
            __FILE__, __LINE__, __func__, msg);
    } else {
        type_ = CDEP_NODE_LOOPENTRY;
    }
    /// In case node was previously in other component as regular
    /// region, mark it as loop entry of current component
    if (component_ != component) {
        component_ = component;
    }
}
ControlDependenceNode::NodeType
ControlDependenceNode::type() const {
    return type_;
}

/**
 * Add CDG node to "region" set for computing serialization information
 *
 * @param node Control Dependence Node to add to the set
 */
void
ControlDependenceNode::addToRegion(ControlDependenceNode& node) {
    region_.insert(&node);
}

/**
 * Returns the "region" set for given node
 *
 * @return the "region" set for given node
 */
const ControlDependenceNode::NodesInfo&
ControlDependenceNode::region() {
    return region_;
}
/**
 * Add CDG node to "eec" set for computing serialization information
 *
 * @param node Control Dependence Node to add to the set
 */

void
ControlDependenceNode::addToEEC(ControlDependenceNode& node) {
    eec_.insert(&node);
}

/**
 * Returns the "eec" set for given node
 *
 * @return the "eec" set for given node
 */

const ControlDependenceNode::NodesInfo&
ControlDependenceNode::eec() {
    return eec_;
}

/**
 * Add CDG node to "pseduo eec" set for computing serialization information
 * case node is predicate basic block. Only actuall predicate move will have
 * predicate eec, rest of moves of basic block needs regular 'leaf' eec
 * computation
 *
 * @param node Control Dependence Node to add to the set
 */

void
ControlDependenceNode::addToPseudoPredicateEEC(ControlDependenceNode& node) {
    pseudoPredicateEEC_.insert(&node);
}

/**
 * Returns the "pseudo eec" set for given node, applicable for predicate nodes
 *
 * @return the "eec" set for given node
 */

const ControlDependenceNode::NodesInfo&
ControlDependenceNode::pseudoPredicateEEC() {
    return pseudoPredicateEEC_;
}

void
ControlDependenceNode::printRelations() const {
    Application::logStream() << "Relations: ";
    for (NodesInfo::const_iterator iter = region_.begin();
        iter != region_.end();
        iter ++) {
        Application::logStream() << (*iter)->toString() << ", ";
    }
    Application::logStream() << std::endl;
    Application::logStream() << "EEC: ";
    for (NodesInfo::const_iterator iter = eec_.begin();
        iter != eec_.end();
        iter ++) {
        Application::logStream() << (*iter)->toString() << ", ";
    }
    Application::logStream() << std::endl;

}
