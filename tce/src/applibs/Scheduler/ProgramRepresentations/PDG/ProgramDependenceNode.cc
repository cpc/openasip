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
 * @file ProgramDependenceNode.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "ProgramDependenceNode.hh"
#include "Move.hh"
#include "MoveNode.hh"
#include "Conversion.hh"
#include "Application.hh"

/**
 * Constructor creating Program Dependence Node which is empty.
 * Does not have related CDG node. For example Loop Close node.
 * @param type Type of the node
 */
ProgramDependenceNode::ProgramDependenceNode(
    NodeType type)
    : GraphNode(), type_(type), component_(-1), newFirstBB_(NULL),
    lastNode_(false) {
    mNode_ = NULL;
    cdgNode_ = NULL;
}

/**
 * Constructor creating Program Dependence Node from Control Dependence
 * region node.
 * @param cdgNode node of CDG
 * @param type Type of the node
 */
ProgramDependenceNode::ProgramDependenceNode(
    ControlDependenceNode& cdgNode,
    NodeType type)
    : GraphNode(), cdgNode_(&cdgNode), type_(type), component_(-1),
    newFirstBB_(NULL), lastNode_(false) {
    mNode_ = NULL;
}

/**
 * Constructor creating Program Dependence Node from Data Dependence node.
 * @param mNode MoveNode of DDG
 * @param type Type of the node
 */
ProgramDependenceNode::ProgramDependenceNode(
    MoveNode& mNode,
    NodeType type)
    : GraphNode(), mNode_(&mNode), type_(type), component_(-1),
    newFirstBB_(NULL), lastNode_(false) {
    cdgNode_ = NULL;
}

/**
 * Empty destructor.
 */
ProgramDependenceNode::~ProgramDependenceNode() {
    region_.clear();
    eec_.clear();
}

/**
 * Returns content of a node as a string.
 * @return string representing content of a node
 */
std::string
ProgramDependenceNode::toString() const {
    TCEString result;
    if (isRegionNode()) {
        if (cdgNode_ != NULL) {
            result += cdgNode_->toString() + ": " +
                Conversion::toString(nodeID());
        } else {
            /// Node added during strong components detection
            /// to collect edges pointing to loop entry from outside the loop.
            /// Do not have cdg equivalent if analysis was done directly on pdg
            result += "Collect_" +  Conversion::toString(nodeID());
        }
    }
    if (isLoopEntryNode()) {
        /// Loop entry is region node converted during detection of loops
        /// Exists in CDG even if loop detection was done on PDG
        result += "LoopEntry_" + Conversion::toString(nodeID()) +
            ": " + cdgNode_->toString();
    }
    if (isLoopCloseNode()) {
        /// Node added during strong components detection
        /// to collect edges pointing to loop entry from inside the loop.
        /// Do not have cdg equivalent if analysis was done directly on pdg
        if (cdgNode_ != NULL) {
            result += "LoopClose_" + Conversion::toString(nodeID()) +
                ": " + cdgNode_->toString();
        } else {
            result += "LoopClose_" + Conversion::toString(nodeID());
        }
    }
    if (isPredicateMoveNode()) {
        result += "Predicate: " + mNode_->toString() + ": " +
            Conversion::toString(nodeID());
    }
    if (isLastNode()) {
        result += "_LAST";
    }
    if (mNode_ != NULL && !isPredicateMoveNode()) {
        result+= mNode_->toString() + ": " + Conversion::toString(nodeID());
    }
    return result;
}
/**
 * Returns content of a node as a string in .dot format.
 * @return string representing content of a node in .dot format
 */
std::string
ProgramDependenceNode::dotString() const {
    if (isPredicateMoveNode()) {
        return TCEString("label=\"") +
            toString() + "\",shape=box,color=green";
    }
    if (isRegionNode()) {
        return TCEString("label=\"")
            + toString() + "\",shape=box,color=blue";
    }
    if (isMoveNode() && moveNode().isMove() && moveNode().move().isCall()) {
        return TCEString("label=\"")
                    + toString() + "\",shape=box,color=red";
    }
    if (isLoopEntryNode()) {
        return TCEString("label=\"")
            + toString() + "\",shape=box,color=red";
    }
    if (isLoopCloseNode()) {
        return TCEString("label=\"")
            + toString() + "\",shape=box,color=yellow";
    }

    return TCEString("label=\"") + toString() + "\"";
}
/**
 * Sets node to be predicate node
 */
void
ProgramDependenceNode::setPredicateMoveNode() {
    if (type_ != PDG_NODE_MOVE && type_ != PDG_NODE_PREDICATE) {
        TCEString msg = "Trying to create predicate move from Region in ";
        msg += toString() + "!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    type_ = PDG_NODE_PREDICATE;
}

/**
 * Returns MoveNode corresponding to given node in DDG.
 * @return MoveNode corresponding to given node in DDG
 */
MoveNode&
ProgramDependenceNode::moveNode() {
    if ((type_ == PDG_NODE_MOVE || type_ == PDG_NODE_PREDICATE)
        && mNode_ != NULL) {
        return *mNode_;
    } else {
        TCEString msg = "MoveNode type does not contain move in ";
        msg += toString() + "!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Returns MoveNode corresponding to given node in DDG as constant.
 * @return MoveNode corresponding to given node in DDG as constant
 */
const MoveNode&
ProgramDependenceNode::moveNode() const {
    if ((type_ == PDG_NODE_MOVE || type_ == PDG_NODE_PREDICATE)
        && mNode_ != NULL) {
        return *mNode_;
    } else {
        TCEString msg = "MoveNode type does not contain move in ";
        msg += toString() + "!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Returns CDGNode corresponding to given node in CDG.
 * @return CDGNode corresponding to given node in CDG
 */
ControlDependenceNode&
ProgramDependenceNode::cdgNode() {
    if ((type_ == PDG_NODE_REGION
        || type_ == PDG_NODE_LOOPENTRY
        || type_ == PDG_NODE_LOOPCLOSE)
        && cdgNode_ != NULL) {
        return *cdgNode_;
    } else {
        TCEString msg = "ControlNode type does not contain CDGNode in ";
        msg += toString() + "!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}
/**
 * Returns CDGNode corresponding to given node in CDG as constant.
 * @return CDGNode corresponding to given node in CDG as constant
 */
const ControlDependenceNode&
ProgramDependenceNode::cdgNode() const {
    if ((type_ == PDG_NODE_REGION
        || type_ == PDG_NODE_LOOPENTRY
        || type_ == PDG_NODE_LOOPCLOSE)
        && cdgNode_ != NULL) {
        return *cdgNode_;
    } else {
        TCEString msg = "ControlNode type does not contain CDGNode in ";
        msg += toString() + "!";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Add node to "region" set for computing serialization information
 *
 * @param node Node to add to the set
 */
void
ProgramDependenceNode::addToRegion(ProgramDependenceNode& node) {
    region_.insert(&node);
}

/**
 * Returns the "region" set for given node
 *
 * @return the "region" set for given node
 */
const ProgramDependenceNode::NodesInfo&
ProgramDependenceNode::region() {
    return region_;
}
/**
 * Add node to "eec" set for computing serialization information
 *
 * @param node Node to add to the set
 */

void
ProgramDependenceNode::addToEEC(ProgramDependenceNode& node) {
    eec_.insert(&node);
}

/**
 * Returns the "eec" set for given node
 *
 * @return the "eec" set for given node
 */

const ProgramDependenceNode::NodesInfo&
ProgramDependenceNode::eec() {
    return eec_;
}

/**
 * Sets the node to be loop entry node of a given component (loop)
 *
 * @param component Component of which the node is loop entry node
 *
 */
void
ProgramDependenceNode::setLoopEntryNode(int component) {
    type_ = PDG_NODE_LOOPENTRY;
    component_ = component;
}
void
ProgramDependenceNode::printRelations() const {
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
