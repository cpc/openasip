/**
 * @file ProgramDependenceNode.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ProgramDependenceNode.hh"

/**
 * Constructor creating Program Dependence Node from Control Dependence 
 * region node.
 * @param cdgNode node of CDG
 * @param nodeID unique ID of a node
 */
ProgramDependenceNode::ProgramDependenceNode(
    ControlDependenceNode& cdgNode,
    int nodeID)
    : GraphNode(nodeID), cdgNode_(&cdgNode) {
    region_ = true;
    predicate_ = false;
    mNode_ = NULL;
}

/**
 * Constructor creating Program Dependence Node from Data Dependence node.
 * @param mNode MoveNode of DDG
 * @param nodeID unique ID of a node
 * @param predicate indicates if created node is also predicate node, default
 * to false
 */
ProgramDependenceNode::ProgramDependenceNode(
    MoveNode& mNode,
    int nodeID, bool predicate)
    : GraphNode(nodeID), mNode_(&mNode), predicate_(predicate) {
    region_ = false;
}

/**
 * Empty destructor.
 */
ProgramDependenceNode::~ProgramDependenceNode() {}

/**
 * Returns content of a node as a string.
 * @return string representing content of a node
 */
std::string
ProgramDependenceNode::toString() const {
    if (isRegionNode()) {
        return cdgNode_->toString();
    }
    return mNode_->toString();
}
/**
 * Returns content of a node as a string in .dot format.
 * @return string representing content of a node in .dot format
 */
std::string 
ProgramDependenceNode::dotString() const {
    if (isPredicateMoveNode()) {
        return std::string("label=\"") + 
            toString() + "\",shape=box,color=green";
    }
    if (isRegionNode()) {
        return std::string("label=\"") 
            + toString() + "\",shape=box,color=blue";
    }
    if (isMoveNode() && moveNode().isMove() && moveNode().move().isCall()) {
        return std::string("label=\"") 
                    + toString() + "\",shape=box,color=red";
    }
    return std::string("label=\"") + toString() + "\"";
}
/**
 * Sets node to be predicate node
 */
void 
ProgramDependenceNode::setPredicateMoveNode() {
    predicate_ = true;
}

/**
 * Checks if a node is region node.
 * @return true if a node is region node of graph
 */
bool 
ProgramDependenceNode::isRegionNode() const {
    return region_;
}
/**
 * Checks is node is predicate MoveNode.
 * @return true if a node is predicate MoveNode
 */
bool 
ProgramDependenceNode::isPredicateMoveNode() const {
    return predicate_;
}
/**
 * Checks if node is MoveNode.
 * @return true if node is MoveNode.
 */
bool 
ProgramDependenceNode::isMoveNode() const {
    return mNode_ != NULL;
}
/**
 * Returns MoveNode corresponding to given node in DDG.
 * @return MoveNode corresponding to given node in DDG
 */
MoveNode& 
ProgramDependenceNode::moveNode() {
    return *mNode_;
}

/**
 * Returns MoveNode corresponding to given node in DDG as constant.
 * @return MoveNode corresponding to given node in DDG as constant
 */
const MoveNode& 
ProgramDependenceNode::moveNode() const {
    return *mNode_;
}
