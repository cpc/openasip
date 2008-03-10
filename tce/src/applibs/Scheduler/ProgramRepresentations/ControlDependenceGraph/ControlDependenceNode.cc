/**
 * @file ControlDependenceNode.cc
 *
 * Prototype control dependence graph of TTA program representation:
 * implementation of graph node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
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
