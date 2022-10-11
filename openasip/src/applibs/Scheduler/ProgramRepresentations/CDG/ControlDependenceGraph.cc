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
 * @file ControlDependenceGraph.cc
 *
 * Implementation of prototype control dependence graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

#include <vector>
#include <algorithm>
#include <functional>
#include <list>
#include <map>

// these need to be included before Boost so we include a working
// and warning-free hash_map
#include "hash_set.hh"
#include "hash_map.hh"

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
IGNORE_COMPILER_WARNING("-Wunused-parameter")
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/timer.hpp>
#include <boost/format.hpp>
POP_COMPILER_DIAGS
POP_CLANG_DIAGS

#include "ControlDependenceGraph.hh"
#include "ControlFlowGraph.hh"
#include "BasicBlockNode.hh"
#include "ControlFlowEdge.hh"
#include "BaseType.hh"
#include "MapTools.hh"
#include "Conversion.hh"
#include "SetTools.hh"
#include "AssocTools.hh"
#include "SequenceTools.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "TerminalInstructionAddress.hh"
#include "Immediate.hh"

#define DEBUG_LEVEL 0
//#define  DEBUG_ANNOTATOR 

ControlDependenceGraph::~ControlDependenceGraph() {
    // removeNode() also removes edges and deletes them
    while (nodeCount() > 0) {
        ControlDependenceNode* n = &node(0);
        removeNode(*n);
        delete n;
    }
    for (unsigned int i = 0; i < strongComponents_.size(); i++) {
        strongComponents_[i].clear();
    }
    strongComponents_.clear();
}
/**
 * Reads ControlFlowGraph of procedure and creates ControlDependenceGraph
 * @param cGraph Control Flow Graph
 */

ControlDependenceGraph::ControlDependenceGraph(
    const ControlFlowGraph& cGraph)  :
    BoostGraph<Node, Edge>(cGraph.name()) ,
    startAddress_(TTAProgram::NullAddress::instance()),
    analyzed_(false), entryNode_(NULL), componentsDetected_(false) {

    program_ = cGraph.program();
    alignment_ = cGraph.alignment();
    cGraph_ = &const_cast<ControlFlowGraph&>(cGraph);
    computeDependence();
}
/**
 * Compute control flow post-dominance information (in the form of an
 * immediate post-dominator tree).
 * Compute the control dependencies of the control flow graph using the
 * pre-computed tree of immediate post-dominators.
 *
 * The base algorithm implemented is described in
 * K.D. Cooper, T.J. Harvey, K. Kennedy: "A Simple, Fast Dominance Algorithm"
 * and Ferrante, Ottenstein, Warren: "The Program Dependence Graphs and Its
 * Use in Optimisation"
 *
 * @throw Throws InvalidData exception in case CFG can not be analyzed
 */
void
ControlDependenceGraph::computeDependence() {

    BlockVector nodes;
    PostOrderMap poMap;
    PostOrder postOrder(poMap);

    createPostDominanceTree(nodes, postOrder);
    // For each edge we have  to record what is predicate it is controlled
    // by...
    DependenceMap dependencies;
    std::vector<Node*> cdNodes;
    detectControlDependencies(nodes, cdNodes, postOrder, dependencies);

    // edge added while creating CD dependencies, not part of CFG by
    // itself so it should be removed now that we have dominance tree
    cGraph_->removeEntryExitEdge();
    // set of control dependencies for given region node
    DependenceMap regionNodes;

    // Creates region node for each set of control dependencies
    // and adds the record to regionNodes
    for (unsigned int i = 0; i < iDomTree_.size();  i++) {
        Node* cNode = cdNodes[i];
        if (cNode->isEntryNode() || cNode->isExitNode()) {
            // Exit postdominate every node including Entry so
            // they do not have any dependencies
            continue;
        }
        DependenceMap::iterator rItr;
        rItr = regionNodes.begin();
        bool found = false;
        // Find if previously created region node have subset of dependencies
        // of currently tested node. If so, replace subset with dependence
        // on previously created region node
        // TODO: The tested set of created nodes should be sorted from
        // largest to smallest
        while (rItr != regionNodes.end()) {
            if (!MapTools::containsKey(dependencies, cNode)) {
                TCEString msg = (boost::format(
                    "Node %s of procedure %s is missing dependencies. "
                    "Most likely CFG with unreachable BB. Can not create CDG!")
                    % cNode->toString() % name()).str();
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
            if (findSubset(
                dependencies[cNode], (*rItr).second, (*rItr).first)){
                found = true;
            }
            rItr ++;
        }
        if (found == true && dependencies[cNode]->size() == 1) {
            // only one dependence and is identical
            // with already existing one, just create edge to existing region
            createControlDependenceEdge(
                *dependencies[cNode]->at(0).first, *cNode);
            continue;
        }
        if (dependencies[cNode]->size() > 0) {
            // Create new region node and add edge from it to tested node
            // record set of dependences for future reuse
            Node* newNode = new Node(Node::CDEP_NODE_REGION);
            addNode(*newNode);
            DependentOn* dOn = new DependentOn(*(dependencies[cNode]));
            regionNodes.insert(
                std::pair<Node*, DependentOn*>(newNode, dOn));
            createControlDependenceEdge(*newNode, *cNode);
        }
    }

    // create dependent edges INTO region nodes
    DependenceMap::iterator regionItr;
    regionItr = regionNodes.begin();
    while (regionItr != regionNodes.end()) {
        // For each region node, add edge from all nodes it is depending on
        // into the region node
        for (unsigned int i = 0; i < (*regionItr).second->size(); i++) {
            createControlDependenceEdge(
                *(*regionItr).second->at(i).first, *(*regionItr).first,
                (*regionItr).second->at(i).second);
        }
        regionItr++;
    }

    eliminateMultipleOutputs();
    /// Removes artificial Exit node, it is not dependent on anything
    /// Move edges from Entry's child region to Entry and delete region
    for (int i = 0; i < nodeCount(); i++) {
        Node& testNode = node(i);
        if (testNode.isExitNode()) {
            if (outDegree(testNode) == 0 && inDegree(testNode) == 0) {
                removeNode(testNode);
                delete &testNode;
                continue;
            }
        }

        if (testNode.isEntryNode()) {
            if (outDegree(testNode) != 1) {
                TCEString msg = (boost::format(
                    "Entry node of procedure %s has more then one child node."
                    "Invalid graph!") % name()).str();
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
            }
            Edge& edge = outEdge(testNode, 0);
            Node& entryChild = headNode(edge);
            moveOutEdges(entryChild, testNode);
            removeNode(entryChild);
            delete &entryChild;
        }
    }
    MapTools::deleteAllValues(regionNodes);
    MapTools::deleteAllKeys(regionNodes);
    MapTools::deleteAllValues(dependencies);
    MapTools::deleteAllKeys(dependencies);
    cdNodes.clear();
    nodes.clear();
}

/**
 * Internal helper. Find already existing region node in set of dependencies.
 *
 * Find if set of dependencies contains a subset that has already a region
 * node. If so, modifies dependencies to contain dependence to existing
 * region node.
 *
 * @param wholeSet Set of dependencies for node we want to test
 * @param subSet Set corresponding to some already existing region node
 * @param regNode Region node we are testing
 * @return True if the intersection was found
 */
bool
ControlDependenceGraph::findSubset(
    DependentOn* wholeSet,
    DependentOn* subSet,
    Node* regNode) {

    if (subSet->size() > wholeSet->size()) {
        return false;
    }
    std::vector<SourceType> missing;
    unsigned int numberOfFound = 0;
    for (unsigned int i = 0; i < wholeSet->size(); i++) {
        SourceType test = wholeSet->at(i);
        bool found = false;
        for (unsigned int j = 0; j < subSet->size(); j++) {
            SourceType tmp = subSet->at(j);
            if (test.first == tmp.first && test.second == tmp.second) {
                found = true;
                numberOfFound++;
            }
        }
        if (found == false) {
            missing.push_back(test);
        }
    }
    if (numberOfFound != subSet->size()) {
        return false;
    }
    wholeSet->clear();
    for (unsigned int i = 0; i < missing.size(); i++) {
        wholeSet->push_back(missing[i]);
    }
    wholeSet->push_back(
        SourceType(regNode,Edge::CDEP_EDGE_NORMAL));
    missing.clear();
    return true;
}

/**
 * Internal hepler. Compute the nearest common dominator of two nodes.
 *
 * Given two nodes (identified by their traversal order), find the nearest
 * common ancestor that dominates both.
 *
 * Note: this method should go in separate immediate dominator class.
 *
 * @param iDom Tree of immediate dominators.
 * @param node1 A graph node.
 * @param node2 Another graph node.
 * @return The post-order number of the nearest dominator of given nodes.
 */
int
ControlDependenceGraph::nearestCommonDom(
    std::vector<int>& iDom,
    int node1,
    int node2) const {

    int finger1 = node1;
    int finger2 = node2;
    do {
        while (finger1 < finger2) {
            finger1 = iDom[finger1];
        }
        while (finger2 < finger1) {
            finger2 = iDom[finger2];
        }
    } while (finger1 != finger2);
    return finger1;
}

/**
 * Create a control dependence edge between two basic blocks.
 *
 * Creates new Control Dependence edge between two basic blocks passed as
 * parameters
 * @param bTail The tail basic block.
 * @param bHead The head basic block.
 * @return The created control dependence edge.
 */
ControlDependenceEdge&
ControlDependenceGraph::createControlDependenceEdge(
    Node& bTail,
    Node& bHead,
    Edge::CDGEdgeType edgeValue) {

    Edge* theEdge = 0;
    try {
        // By construction, there should not! be duplication of CD edges!!!
        if (hasEdge(bTail, bHead)) {
            theEdge = graph_[connectingEdge(bTail, bHead)];
        } else {
            theEdge = new Edge(edgeValue);
            connectNodes(bTail, bHead, *theEdge);
        }
    } catch (const ObjectAlreadyExists& e) {
        throw ObjectAlreadyExists(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }
    return *theEdge;
}

/**
 * Internal helper. Creates a tree of immediate post dominators for
 * constructing a control dependencies
 *
 * @param nodes Will be filled with pointers to BasicBlocks indexed
 *      by post order number
 * @param postOrder Will be filled by post order numbers
 */
void
ControlDependenceGraph::createPostDominanceTree(
    BlockVector& nodes,
    PostOrder& postOrder) {

    // Add false edge from entry to exit
    cGraph_->addEntryExitEdge();
    ControlFlowGraph::ReversedGraph* revGraph_ = NULL;
    revGraph_ = &cGraph_->reversedGraph();

    bool modified = false;
    int counter = 0;
    std::vector<ControlFlowEdge*> addedEdges;
    BasicBlockNode& cfgExitNode = cGraph_->exitNode();
    do {
        ColorMap cMap;
        Color colors(cMap);
        modified = false;
        int tStamp(-1);
        boost::time_stamper<PostOrder, int, boost::on_finish_vertex>
                pOrderStamper(postOrder, tStamp);
        // the order of nodes within the reversed graph remains unchanged,
        // have to look for the sink node (of the original graph) when we
        // want to get the start node of the reverse graph.
        boost::depth_first_visit(
            *revGraph_, cGraph_->descriptor(cfgExitNode),
            boost::make_dfs_visitor(pOrderStamper), colors);
        // there can be just one node with post order number 0
        // if there are several, then some of them are not post
        // dominated by Exit - endless loop without break or return
        // we add and edge from one of those nodes to exit and redo
        // the dfs visit.
        std::vector<BasicBlockNode*> postZero;
        for (int i = cGraph_->nodeCount() - 1; i >=0; i--) {
            BasicBlockNode* testedNode = &cGraph_->node(i);
            if (postOrder[cGraph_->descriptor(*testedNode)] == 0) {
                postZero.push_back(testedNode);
            }
        }
        if (postZero.size() > 1) {
            for (unsigned int i = 0; i < postZero.size(); i++) {
                BasicBlockNode* testedNode = postZero[i];
                if (!testedNode->isEntryBB()) {
                    ControlFlowEdge* edge = new
                        ControlFlowEdge(
                            ControlFlowEdge::CFLOW_EDGE_LOOP_BREAK);
                    cGraph_->connectNodes(
                        *testedNode, cGraph_->exitNode(), *edge);
                    addedEdges.push_back(edge);
                    delete revGraph_;
                    revGraph_ = &cGraph_->reversedGraph();
                    modified = true;
                    break;
                }
            }
        }
        postZero.clear();
        counter++;
    } while (modified);
    delete revGraph_;
    revGraph_ = NULL;

    // tree of immediate dominators, mapping a node to its immediate
    // dominator; each node is represented by its inverted post-order number
    iDomTree_.resize(cGraph_->nodeCount());
    const int startNodePO = cGraph_->nodeCount() - 1;
    // create inverse map from post-order to node
    // initialise tree of immediate dominators
    nodes.resize(cGraph_->nodeCount());
    for (unsigned int i = 0; i < nodes.size(); i++) {
        nodes[postOrder[cGraph_->descriptor(cGraph_->node(i))]] =
            &(cGraph_->node(i));
        iDomTree_[i] = -1;
    }

    iDomTree_[startNodePO] = startNodePO;
    bool changed = true;
    while (changed) {
        changed = false;
        // traverse graph in reverse post-order, skipping start node
        for (int i = cGraph_->nodeCount() - 2; i >= 0; i--) {
            BasicBlockNode& b(*nodes[i]);
            int newDom;
            int predIndex;
            for (predIndex = 0; predIndex < cGraph_->outDegree(b);
                predIndex++) {
                BasicBlockNode& predecessor(cGraph_->headNode(
                    cGraph_->outEdge(b, predIndex)));
                int predPO = postOrder[cGraph_->descriptor(predecessor)];
                // Find first processed predecessor of b
                if (iDomTree_[predPO] != -1) {
                    break;
                }
            }
            // outgoing edges of original graph are in-edges of reverse graph
            BasicBlockNode& predecessor(cGraph_->headNode(
                cGraph_->outEdge(b, predIndex)));
            newDom = postOrder[cGraph_->descriptor(predecessor)];
            // because nodes are searched in inverse post-order, at least
            // one of predecessors of current node must have been processed
            if (newDom == -1) {
                TCEString message =
                    "Missing postOrder number of predecessor!";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, message);
            }

            for (predIndex = predIndex + 1;
                predIndex < cGraph_->outDegree(b);
                predIndex++) {
                BasicBlockNode& predecessor(cGraph_->headNode(
                    cGraph_->outEdge(b, predIndex)));
                int predPO = postOrder[cGraph_->descriptor(predecessor)];
                if (iDomTree_[predPO] != -1) {
                    newDom = nearestCommonDom(iDomTree_, newDom, predPO);
                }
            }
            if (newDom != iDomTree_[i]) {
                changed = true;
                iDomTree_[i] = newDom;
            }
        }
    }
    for (unsigned int i = 0; i < addedEdges.size(); i++) {
        cGraph_->removeEdge(*addedEdges[i]);
    }
}

/**
 * Return the entry node of the graph. Cache it after it's found for alter use
 *
 * @return The entry node of the graph.
 * @exception InstanceNotFound if the graph does not have a entry node.
 * @exception ModuleRunTimeError if the graph has multiple nodes that are
 *            recognised as entry nodes.
 */
ControlDependenceNode&
ControlDependenceGraph::entryNode() {
    if (entryNode_ == NULL) {
        Node* result = NULL;
        bool found = false;
        for (int i = 0; i < nodeCount(); i++) {
            if (inDegree(node(i)) == 0) {
                // sanity check
                if (!static_cast<Node&>(node(i)).isEntryNode()) {
                    // probably the entry node is not present
                    TCEString errorMsg = (boost::format(
                        "Graph %s has node %s with no input edges which is "
                        "not the entry node!\n") % name()
                        % node(i).toString()).str();
                    throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
                        errorMsg);
                }
                if (found == true) {
                    throw ModuleRunTimeError(
                        __FILE__, __LINE__, __func__,
                        "Corrupted graph. Found multiple entry nodes!");
                }
                result = dynamic_cast<Node*>(&node(i));
                found = true;
            }
        }
        if (found == false || result == NULL) {
            TCEString errorMsg = (boost::format(
                "Graph %s does not have entry node or has mutliple nodes with"
                " no input edges!") % name()).str();
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, errorMsg);
        }
        entryNode_ = result;
        return *result;
    } else {
        return *entryNode_;
    }
}

/**
 * Internal helper. Implemented detection of control dependencies.
 *
 * @param nodes Vector of basic block nodes indexed by reverse post order
 * number
 * @param cdNodes Vector of control dependence nodes indexed by reverse post
 * order number
 * @param postOrder  Post order numbering of nodes
 * @param dependencies For each node, the list of nodes it is dependent on
 */
void
ControlDependenceGraph::detectControlDependencies(
    BlockVector& nodes,
    std::vector<Node*>& cdNodes,
    PostOrder& postOrder,
    DependenceMap& dependencies) {

    std::map<BasicBlockNode*, Node*> BBCD;
    for (int i = 0; i < cGraph_->nodeCount(); i++) {
        BasicBlockNode* bbNode = &cGraph_->node(i);
        int nodeOutDegree = cGraph_->outDegree(*bbNode);
        if (!MapTools::containsKey(BBCD, bbNode)) {
            Node* cd = NULL;
            Node::NodeType typeOfNode = Node::CDEP_NODE_BB;
            if (nodeOutDegree == 2 && (!bbNode->isEntryBB())) {
                // 2 out edges in CFG means BB ends with conditional jump
                // therefore it is predicate BB
                typeOfNode = Node::CDEP_NODE_PREDICATE;
            } else if(nodeOutDegree > 2){
                /// Basic block is either 3way jump - forbidden
                /// or it is indirect jump with edges to all data-code
                /// rellocations. We can not deal with such situation
                /// at the moment. Fix cfg to original form and throw exception
                cGraph_->removeEntryExitEdge();
                TCEString msg = (boost::format(
                    "Basic block %s has out degree of %d. Most likely "
                    "indirect jump. Can not create CDG for that atm.")
                    % bbNode->toString() % nodeOutDegree).str();
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
            cd = new Node(typeOfNode, bbNode);
            addNode(*cd);
            BBCD.insert(std::pair<BasicBlockNode*,Node*>(
                bbNode, cd));
        }
        if (nodeOutDegree < 2) {
            /// node is not predicate
            continue;
        } else if(nodeOutDegree > 2){
            /// Basic block is either 3way jump - forbidden
            /// or it is indirect jump with edges to all data-code
            /// rellocations. We can not deal with such situation
            /// at the moment. Fix cfg to original form and throw exception
            cGraph_->removeEntryExitEdge();
            TCEString msg = (boost::format(
                "Basic block %s has out degree of %d. Most likely "
                "indirect jump. Can not create CDG for that atm.")
                % bbNode->toString() % nodeOutDegree).str();
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
        for (int j = 0 ; j < nodeOutDegree; j++) {
            Edge::CDGEdgeType edgeType =
                Edge::CDEP_EDGE_NORMAL;
            BasicBlockNode *t = &(cGraph_->headNode(
                cGraph_->outEdge(*bbNode, j)));

            int nPO = postOrder[cGraph_->descriptor(*bbNode)];
            int tPO = postOrder[cGraph_->descriptor(*t)];
            int nPoDom = iDomTree_[nPO];
            if (nPoDom == tPO) {
                // t is target of n, so if it post-dominate n
                // it is immediate post dominator!
                continue;
            }
            int runnerPo = tPO;
            if (cGraph_->outEdge(*bbNode, j).isTrueEdge()) {
                edgeType = Edge::CDEP_EDGE_TRUE;
            }
            if (cGraph_->outEdge(*bbNode, j).isFalseEdge()) {
                edgeType = Edge::CDEP_EDGE_FALSE;
            }
            SourceType newSource = SourceType(
                MapTools::valueForKey<Node*>(
                BBCD, bbNode), edgeType);

            while (nPoDom != runnerPo) {
                // Walk through postDominator tree
                // Store found CD in multimap
                Node* runnerCD = NULL;
                if (MapTools::containsKey(BBCD, nodes[runnerPo])) {
                    runnerCD = MapTools::valueForKey<Node*>(
                        BBCD, nodes[runnerPo]);
                } else {
                    if (cGraph_->outDegree(*nodes[runnerPo]) == 2 &&
                        !nodes[runnerPo]->isEntryBB()) {
                        // 2 out edges in CFG means BB ends with conditional
                        //jump therefore it is predicate BB
                        runnerCD = new Node(
                            Node::CDEP_NODE_PREDICATE, nodes[runnerPo]);
                    } else {
                        runnerCD = new Node(
                            Node::CDEP_NODE_BB, nodes[runnerPo]);
                    }
                    addNode(*runnerCD);
                    BBCD.insert(
                        std::pair<BasicBlockNode*,Node*>(
                            nodes[runnerPo], runnerCD));
                }
                if (MapTools::containsKey(dependencies, runnerCD)) {
                    DependentOn* dep = MapTools::valueForKey<DependentOn*>(
                        dependencies, runnerCD);
                    dep->push_back(newSource);
                } else {
                    DependentOn* dep = new DependentOn;
                    dep->push_back(newSource);
                    dependencies.insert(
                        std::pair<Node*, DependentOn*>(
                            runnerCD, dep));
                }
                runnerPo = iDomTree_[runnerPo];
            }
        }

    }
    for (unsigned int i = 0; i < nodes.size(); i++) {
        Node* cdNode;
        cdNode = MapTools::valueForKey<Node*>(
            BBCD, nodes[i]);
        cdNodes.push_back(cdNode);
    }
    BBCD.clear();
}

/**
 * Returns an alignment of procedure in original POM
 * @return Alignment take from original POM
 */
int
ControlDependenceGraph::alignment() const {
    return alignment_;
}

/**
 * Returns the pointer to Program object of POM procedure
 * belongs to.
 * @return Pointer to TTAProgram::Program
 */
TTAProgram::Program*
ControlDependenceGraph::program() const {
    return program_;
}

/**
 * Internal helper. Replaces several output edges with same truth value with
 * region node. Combines several output edges without truth value (indirect
 * jumps for example);
 */
void
ControlDependenceGraph::eliminateMultipleOutputs() {
    for (int i = 0; i < nodeCount(); i++) {
        if (outDegree(node(i)) > 1 && node(i).isBBNode()) {
            std::vector<Node*> trueLinks;
            std::vector<Node*> falseLinks;
            Node& currNode = node(i);
            int currDegree = outDegree(currNode);
            for (int j = 0; j < currDegree; j++) {
                Edge* currentOutEdge = &outEdge(currNode, j);
                if (currentOutEdge->isTrueEdge()) {
                    trueLinks.push_back(&headNode(*currentOutEdge));
                    continue;
                }
                if (currentOutEdge->isFalseEdge()) {
                    falseLinks.push_back(&headNode(*currentOutEdge));
                    continue;
                }
            }
            /// Combine all "true" children of predicate with region
            if (trueLinks.size() > 1) {
                Node* regNode = new Node(Node::CDEP_NODE_REGION);
                addNode(*regNode);
                createControlDependenceEdge(
                    currNode, *regNode, Edge::CDEP_EDGE_TRUE);
                for (unsigned int j = 0; j < trueLinks.size(); j++) {
                    createControlDependenceEdge(*regNode, *trueLinks[j]);
                    disconnectNodes(currNode, *(trueLinks[j]));
                }
            }
            /// Combine all "false" children of predicate with region
            if (falseLinks.size() > 1) {
                Node* regNode = new Node(Node::CDEP_NODE_REGION);
                addNode(*regNode);
                createControlDependenceEdge(
                    currNode, *regNode, Edge::CDEP_EDGE_FALSE);
                for (unsigned int j = 0; j < falseLinks.size(); j++) {
                    createControlDependenceEdge(*regNode, *falseLinks[j]);
                    disconnectNodes(currNode, *(falseLinks[j]));
                }
            }
        }
    }
}

/**
 * Performs serialization of ControlDependenceGraph, turning it into
 * Control Flow Graph.
 *
 * @return ControlFlowGraph representation of CDG
 * @throw InvalidData in case serialization of graph is not possible
 */
void
ControlDependenceGraph::analyzeCDG() {
    // CDG was previously analyzed
    if (analyzed()) {
        return;
    }
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\tStarting CDG serialization for %s with %d nodes and %d edges.\n")
            % name() % nodeCount() % edgeCount()).str();
    }

    CDGOrderMap componentMap;
    DescriptorMap rootMap;
    boost::timer timer;
    /// Find all strong components (loops) in a graph, loop entry nodes
    /// will have number identifying for which loop component they are entries
    /// and the strongComponents_ will contains all nodes that are part
    /// of each component
    long elapsed = 0;
    int componentCount = detectStrongComponents(componentMap, rootMap);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
        "\t\tStrong components: %d components, %d minutes and %d seconds.\n")
            % componentCount % (elapsed/60) % (elapsed%60)).str();
    }

    /// map to store post order information for all the nodes of a graph
    /// From now on, post order info in lastMap will not change
    CDGOrderMap lastMap;
    CDGOrder lastOrder(lastMap);
    /// map to store color information during dfs
    ColorMap colorMap;
    Color colorsDFS(colorMap);
    int fStamp(-1);
    /// boost::on_finish_vertex will give us post order numbering
    boost::time_stamper<CDGOrder, int, boost::on_finish_vertex>
        lastOrderStamper(lastOrder, fStamp);
    timer.restart();
    /// Sort nodes in post order, starting from entry node
    boost::depth_first_visit(
        graph_, descriptor(entryNode()),
        boost::make_dfs_visitor(lastOrderStamper), colorsDFS);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tPost order: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }

    timer.restart();
    /// Computes "region" information, for each node X, set of nodes that are
    /// executed when X is executed (for node Z, on all paths from X to entry,
    /// if node Z is region all children of Z will be in "region" of X
    computeRegionInfo(lastMap);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tRegion: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }

    timer.restart();
    /// Computes "eec" information, for each node X, set of nodes that are
    /// executed when any node in subgraph of X is executed, computed using
    /// region information
    computeEECInfo(lastMap);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tEEC: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }
    analyzed_ = true;
    timer.restart();
#if 0
    // This is really not needed, just for comparing with PDG edge generation
    // if necessary
    computeRelations(lastMap);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tRelations: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }
#endif
}

/**
 * Detects all strong components of a CDG graph (loops). Strong components are
 * maximal sets of nodes that are reachable from each other. Each node is member
 * of some component. If it is not in loop then node is components of it's own.
 * Augments graph with loop entry and close nodes.
 * Works iterativly, first detects maximal component, then proceeds to ones
 * that are "embedded".
 *
 * @param components After return contains for each node number of component
 * to which node belongs
 * @param roots After return contains for each node a node that is root of
 * component to which node belongs
 * @return returns number of components in a graph
 */
int
ControlDependenceGraph::detectStrongComponents(
    CDGOrderMap& components,
    DescriptorMap& roots) {

    if (componentsDetected_) {
        return strongComponents_.size();
    }

    std::vector<std::pair<Node*, Node*> >  backEdges;
    int componentCount = 0;
    int currentComponents = 0;
    /// Will repeat untill all the strong components will be found
    /// Including weirdly nested :-)
    do {
        CDGOrder componentOrder(components);
        Descriptors componentRoots(roots);
        currentComponents = 0;
        /// Node count will change with addition of close nodes
        currentComponents =  boost::strong_components(
            graph_, componentOrder, boost::root_map(componentRoots));

        // for each component add vector of nodes that belongs to it
        std::vector<std::set<Node*> >  componentVector;
        componentVector.resize(componentCount + currentComponents);

        /// If the number of components is identical to number of nodes
        /// there are no loops in graph
        if (currentComponents == nodeCount()) {
            componentCount = strongComponents_.size();
            break;
        }
        /// Add to strong components only those which are loops
        /// Store them as CDNode*, use of descriptors is not possible
        /// due to later addition of Nodes which will invalidate
        /// descriptors
        for (CDGOrderMap::iterator iterA = components.begin();
            iterA != components.end(); iterA ++) {
                Node* cNode = graph_[(*iterA).first];
                componentVector[(*iterA).second].insert(cNode);
        }

        for (unsigned int i = componentCount; i < componentVector.size(); i++) {
            if (componentVector[i].size() > 1) {
                std::set<Node*>& vector = componentVector[i];
                std::set<Node*> ref;
                int componentsSize = strongComponents_.size();
                for (std::set<Node*>::iterator iterB = vector.begin();
                    iterB != vector.end();
                    iterB++) {
                    ref.insert(*iterB);
                    // Set component number
                    if ((*iterB)->component() == -1){
                        (*iterB)->setComponent(componentsSize);
                    }
                }
                strongComponents_.push_back(ref);
            }
        }

        /// Detects all loop entry nodes
        /// Stores Nodes which are identified as loop entry nodes
        /// together with number to which loop they belong
        std::vector<std::pair<Node*,int> > newEntry;
        /// for each entry node, collect edges that points to it from outside
        /// the loop, deals only with newly added components
        std::map<Node*, std::vector<Node*> >  incomingToEntry;
        for (unsigned int i = componentCount;
            i < strongComponents_.size();
            i++) {
            std::set<Node*>& nodes = strongComponents_[i];
            for (std::set<Node*>::iterator iterD = nodes.begin();
                iterD != nodes.end();
                iterD++) {
                EdgeSet edges = inEdges(**iterD);
                for (EdgeSet::iterator ei = edges.begin();
                    ei != edges.end();
                    ei++) {
                    Node* testedNode = &tailNode(**ei);
                    if (nodes.find(testedNode) == nodes.end()) {
                        if (!(*iterD)->isLoopEntryNode(i)) {
                            /// This may change in which component node it
                            (*iterD)->setLoopEntryNode(i);
                            newEntry.push_back(
                                std::pair<Node*,int>(*iterD,i));
                            incomingToEntry[*iterD].push_back(testedNode);
                        } else {
                        /// Several nodes points to loop entry node
                        /// we create dummy region node to collect those
                        /// edges
                           incomingToEntry[*iterD].push_back(testedNode);
                        }
                    }
                }
            }
        }

        /// Adds close nodes for each loop entry node
        /// Node and edge descriptors are invalidated here!
        for (unsigned int j = 0; j < newEntry.size(); j++) {
            Node* loopNode = newEntry[j].first;
            std::set<Node*>& nodes =
                strongComponents_[newEntry[j].second];
            /// Create one "close" node for each loop entry
            Node* close = new Node(Node::CDEP_NODE_LOOPCLOSE);
            close->setComponent(newEntry[j].second);
            addNode(*close);
            /// Close node is also part of component
            strongComponents_[newEntry[j].second].insert(close);

            /// Detect edges to loop entry node from inside component and
            /// redirect them to close node
            EdgeSet edges = inEdges(*loopNode);
            std::vector<Edge*> storeEdges;
            for (EdgeSet::iterator ei = edges.begin();
                ei != edges.end();
                ei++) {
                Node* sourceNode = &tailNode(**ei);
                if (nodes.find(sourceNode) != nodes.end()){
                    storeEdges.push_back(*ei);
                }
            }
            /// Actually redirect edges
            for (unsigned int counter = 0;
                counter < storeEdges.size();
                counter++) {
                moveInEdge(*loopNode, *close, *storeEdges[counter]);
            }
            /// Back edge will be added later, after all loops are found
            backEdges.push_back(
                std::pair<Node*, Node*>(close, loopNode));

            /// Test if edges were redirected successfully
            if (inDegree(*close) == 0) {
                TCEString msg = (boost::format(
                    "Close node for loop entry node %s was not connected!\n")
                    % loopNode->toString()).str();
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
            }

            /// In case loop entry has multiple incoming edges
            /// outside loop, we add new region to group them together
            std::vector<Node*> tmp =
               MapTools::valueForKey<std::vector<Node*> >(
                  incomingToEntry, loopNode);
            if (tmp.size() > 1) {
                Node* collect = new Node(Node::CDEP_NODE_REGION);
                addNode(*collect);
                for (unsigned int i = 0; i < tmp.size(); i++) {
                    Node* input = tmp[i];
                    EdgeDescriptor ed = connectingEdge(*input, *loopNode);
                    Edge* e = graph_[ed];
                    moveInEdge(*loopNode, *collect, *e);
                }
                ControlDependenceEdge* edge2 =
                    new ControlDependenceEdge();
                connectNodes(*collect, *loopNode, *edge2);
            }
        }
        newEntry.clear();
        componentCount = strongComponents_.size();
    } while (true);

    // Add edges from close nodes to their respective loop entries
   for (unsigned int i = 0; i < backEdges.size(); i++) {
      createControlDependenceEdge(
         *backEdges[i].first, *backEdges[i].second, Edge::CDEP_EDGE_LOOPCLOSE);
    }
    backEdges.clear();
    componentsDetected_ = true;
    return componentCount;
}

/**
 * Compute the "region" information for each node of graph. Region is used to
 * compute "eec" to determine order in which sibling subgraphs will be in
 * resulting cfg.
 *
 * "Region" of node X is set of nodes that will be executed in case is X
executed.
 *
 * @param orderMap post order map of CDG graph, nodes will be augmented with
 * "region" information.
 */
void
ControlDependenceGraph::computeRegionInfo(const CDGOrderMap& orderMap){

   int mapSize = orderMap.size();
   if (mapSize == 0) {
      throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
         "No nodes in CDG graph for " + name() + "!");
   }
   /// Compute "region" information using reverse post-order processing
    for (int i = mapSize -1 ; i >= 0 ; i--) {
        NodeDescriptor des =
            MapTools::keyForValue<NodeDescriptor>(orderMap,i);
        Node* node = graph_[des];
        if (!node->isLoopEntryNode()) {
            /// For non loop entry nodes, simply compute region info
            /// and store it in the node
            Node::NodesInfo finalNodesInfo;
            regionHelper(node, finalNodesInfo);

            for (Node::NodesInfo::iterator k = finalNodesInfo.begin();
                k != finalNodesInfo.end(); k++) {
                node->addToRegion(**k);
            }
        } else if (node->region().size() == 0) {
            /// for loop entry node, find all other entry nodes
            /// of the same loop (component)
            /// final region info will be intersection of region infos from
            /// all the entry nodes in the same loop
            /// it will be same for all the nodes too (they are reachable)
            std::vector<Node*> entries;
            std::set<Node*> component = strongComponents_[node->component()];
            for (std::set<Node*>::iterator si = component.begin();
                si != component.end(); si++) {
                /// Test for loop entries of same component
                /// Loop entry of one component can be regular region node of
                /// other "larger" loop
                if ((*si)->isLoopEntryNode(node->component())) {
                    entries.push_back(*si);
                }
            }
            Node::NodesInfo finalNodesInfo;
            for (unsigned int i = 0; i < entries.size(); i++) {
                Node* entryNode = entries[i];
                regionHelper(entryNode, finalNodesInfo);
            }
            for (unsigned j = 0; j < entries.size(); j++) {
                Node* result = entries[j];
                for (Node::NodesInfo::iterator k = finalNodesInfo.begin();
                    k != finalNodesInfo.end(); k++) {
                    result->addToRegion(**k);
                }
            }
        }
    }
}

/**
 * Compute the "eec" information for each node of graph. Eec is used to
 * determine order in which sibling subgraphs needs to be scheduled in
 * resulting cfg.
 *
 * "eec" of node X is set of nodes that will be executed in case any of the
nodes
 * in subgraph of X will be executed.
 *
 * @param orderMap post order map of CDG graph, nodes augmented with
 * "region" information, "eec" information will be added.
 */
void
ControlDependenceGraph::computeEECInfo(const CDGOrderMap& orderMap) {
    int mapSize = orderMap.size();
    for (int i = 0; i < mapSize; i++) {
        NodeDescriptor des =
            MapTools::keyForValue<NodeDescriptor>(
            orderMap, i);
        Node* node = graph_[des];
         /// eec already exists, skip
         if (node->eec().size() > 0) {
            continue;
         }
        /// Found close node, eec(node) == intersection of all close
        /// nodes of same loop region(node) (close node is as leaf node)
        if (node->isLoopCloseNode() && node->eec().size() == 0) {
           std::vector<Node*> closeNodes;
           std::set<Node*> component = strongComponents_[node->component()];
          // collect all loop close nodes of same loop
           for (std::set<Node*>::iterator si = component.begin();
               si != component.end(); si++) {
               if ((*si)->isLoopCloseNode()) {
                   closeNodes.push_back(*si);
               } else if ((*si)->isRegionNode()
                    || (*si)->isPredicateNode()) {
                    (*si)->setLastNode();
                }
           }
           Node::NodesInfo finalInfo;
           Node::NodesInfo storeResult;
           finalInfo.insert(node->region().begin(),node->region().end());
           for (unsigned int i = 0; i < closeNodes.size(); i++) {
                SetTools::intersection(
                finalInfo, closeNodes[i]->region(), storeResult);
                finalInfo.swap(storeResult);
                storeResult.clear();
            }
            // add intersection of all region infos to each close node in loop
            for (unsigned j = 0; j < closeNodes.size(); j++) {
                Node* result = closeNodes[j];
                if(result->eec().size() != 0) {
                    TCEString msg = (boost::format(
                        "Close node %s in %s already has eec!\n")
                        % result->toString() % node->toString()).str();
                    throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
                }
                for (Node::NodesInfo::iterator k = finalInfo.begin();
                    k != finalInfo.end(); k++) {
                    result->addToEEC(**k);
                }
            }
            continue;
         }
        if (outDegree(*node) == 0) {
            /// Found leaf node, eec(node) == region(node)
            Node::NodesInfo regionX = node->region();
            for (Node::NodesInfo::iterator t = regionX.begin();
                t != regionX.end(); t++) {
                node->addToEEC( **t);
            }
            continue;
        } else {
            if (node->isPredicateNode()) {
            /// if node is predicate we also store pseudo information for
            /// part of basic block which does not compute predicate
            /// itself, it is just set of statements - leafs
                Node::NodesInfo regionX = node->region();
                for (Node::NodesInfo::iterator t = regionX.begin();
                    t != regionX.end(); t++) {
                    node->addToPseudoPredicateEEC( **t);
                }
            }

            /// Not a leaf node,
            /// eec(x) = intersection(eec(children(x)) \ children(x)
            NodeSet succ = successors(*node);
            Node::NodesInfo childNodes;
            // copy successors from NodeSet to NodesInfo otherwise
            // comparator function object in NodeSet makes chaos with
            // set_difference
            for (NodeSet::iterator su = succ.begin();
               su != succ.end(); su++) {
               childNodes.insert(*su);
            }
            Node::NodesInfo finalEEC;

            // Fill in candidate set with data from first successor
            finalEEC.insert(
               (*(childNodes.begin()))->eec().begin(),
               (*(childNodes.begin()))->eec().end());
            // compute intersection of successors eec
            for(Node::NodesInfo::iterator j = childNodes.begin();
               j != childNodes.end(); j++ ) {
               Node::NodesInfo storeResult;
               SetTools::intersection(finalEEC, (*j)->eec(), storeResult);
               finalEEC.swap(storeResult);
               storeResult.clear();
            }
            std::vector<Node*> result(finalEEC.size(), NULL);
            // compute set difference, returns iterator pointing to .end()
            // of the result
            std::vector<Node*>::iterator resultEnd =
               std::set_difference(finalEEC.begin(), finalEEC.end(),
                  childNodes.begin(), childNodes.end(), result.begin());
            // push resulting eec into the node eec info
            for (std::vector<Node*>::iterator t = result.begin();
                t != resultEnd; t++) {
                node->addToEEC(**t);
            }
        }
    }
}

/**
 * Defined relation between two sibling nodes based on their type and eec info
 */
ControlDependenceGraph::CompareResult
ControlDependenceGraph::compareSiblings(Node* a, Node* b) const {
    bool AinA = false;
    bool BinB = false;
    bool BinA = false;
    bool AinB = false;
    // both a and b are simple basic blocks, order is given by data dependencies
    // no need to test eec info
    if (a->isBBNode() && b->isBBNode()
        && !a->isPredicateNode() && !b->isPredicateNode()) {
        return ANY_ORDER;
    }

    if (AssocTools::containsKey(a->eec(), a)) {
        AinA = true;
    }
    if (AssocTools::containsKey(b->eec(), b)) {
        BinB = true;
    }
    if (AssocTools::containsKey(a->eec(), b)) {
        BinA = true;
    }
    if (AssocTools::containsKey(b->eec(), a)) {
        AinB = true;
    }
    if ((a->isLoopEntryNode() || a->isPredicateNode())
        && (b->isBBNode() && !b->isPredicateNode())
        && AinA == true) {
        if (a->isLastNode()) {
            return B_BEFORE_A;
        }
        return ANY_ORDER;
    }
    if ((a->isLoopEntryNode() || a->isPredicateNode())
        && (b->isLoopEntryNode() || b->isPredicateNode())
        && AinA == true && BinB == true) {
        if (a->isLastNode()) {
            return B_BEFORE_A;
        }
        if (b->isLastNode()) {
            return A_BEFORE_B;
        }
        return ANY_ORDER;
    }
    if ((a->isLoopEntryNode() || a->isPredicateNode())
        && (b->isBBNode() && !b->isPredicateNode())
        && AinA == false) {
        return B_BEFORE_A;
    }
    if ((a->isLoopEntryNode() || a->isPredicateNode())
        && (b->isLoopEntryNode() || b->isPredicateNode())
        && AinA == false && BinB == true) {
        return B_BEFORE_A;
    }
    if ((a->isLoopEntryNode() || a->isPredicateNode())
        && (b->isLoopEntryNode() || b->isPredicateNode())
        && AinA == false && BinB == false) {
        return UNORDERABLE;
    }
    if ((a->isRegionNode() && AinB == true)
        && (b->isBBNode() || b->isLoopEntryNode() || b->isPredicateNode())) {
        return B_BEFORE_A;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isLoopEntryNode() || b->isPredicateNode())
        && AinB == false) {
        return UNORDERABLE;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isRegionNode() && BinA == true)) {
        return B_BEFORE_A;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isRegionNode() && BinA == false)) {
        return UNORDERABLE;
    }
    if ((a->isLoopCloseNode() && AinB == true)
        && (b->isBBNode() || b->isPredicateNode() || b->isLoopEntryNode()
            || b->isRegionNode())) {
        return B_BEFORE_A;
    }
    if ((a->isLoopCloseNode() && AinB == false)
        && (b->isPredicateNode() || b->isLoopEntryNode()
            || b->isRegionNode())) {
        return UNORDERABLE;
    }
    /// FIXME:
    /// Unique region node rule is broken when creating loop
    /// entry nodes (removing loop back edge) and creating new region for
    /// incoming edges into loop entry - there can be another region
    /// which already has same set of dependences and loop entry was
    /// supposed to be child of that, but was not. Problem with detection
    /// of subsets of dependencies when creating region nodes!
    if ((a->isRegionNode() && AinB == true)
        && (b->isRegionNode() && BinA == true)) {
        if (a->isLastNode()) {
            return B_BEFORE_A;
        }
        if (b->isLastNode()) {
            return A_BEFORE_B;
        }
        return ANY_ORDER;
    }
    return ERROR;
}

/**
 * Helper function to compute actual region information for a node.
 * Called several times for a case when there are loop entry nodes
 * detected.
 *
 * @param node Node for which to compute region info
 * @param cdg Filtered PDG graph with only control dependence edges
 * @param finalNodesInfo stores actuall computed region info
 */
void
ControlDependenceGraph::regionHelper(
    Node* node,
    Node::NodesInfo& finalNodesInfo){

    std::vector<Node::NodesInfo> tmpResult;
    /// Find all incoming control dependence edges
    EdgeSet edges = inEdges(*node);
    for (EdgeSet::iterator ei = edges.begin();
        ei != edges.end();
        ++ei) {
        Node* previous = &tailNode(**ei);
        if (previous->isRegionNode()
            || previous->isLoopEntryNode()
            || previous->isEntryNode()) {
            if (previous->region().size() == 0 &&
                !previous->isEntryNode()) {
                /// If parent's region is not yet computed (should be btw)
                /// compute it before continuing.
                Node::NodesInfo addedNodesInfo;
                regionHelper(previous, addedNodesInfo);
                for (Node::NodesInfo::iterator k = addedNodesInfo.begin();
                    k != addedNodesInfo.end();
                    k++) {
                    previous->addToRegion(**k);
                }
            }
            /// region(node,parent) == region(parent) U childern(parent)
            Node::NodesInfo tmp = previous->region();
            EdgeSet outgoingEdges = outEdges(*previous);
            for (EdgeSet::iterator ei = outgoingEdges.begin();
                ei != outgoingEdges.end();
                ++ei) {
                Node* testedNode = &headNode(**ei);
                tmp.insert(testedNode);
            }
            tmpResult.push_back(tmp);
        } else if (previous->isPredicateNode()){
        /// region(node,parent) == region(parent)
            Node::NodesInfo tmp = previous->region();
            tmpResult.push_back(tmp);
        } else {
            if (!previous->isLoopCloseNode()) {
                TCEString message = (boost::format(
                    "Node: %s , parent %s.")
                    % node->toString() % previous->toString()).str();
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, message);
            }
        }
    }
    /// fill in final region info with info from first of parents
    if (tmpResult.size() > 0) {
        finalNodesInfo.insert(
            tmpResult[0].begin(), tmpResult[0].end());
    }
    /// region(node) = intersection(region(node,parent(node)))
    /// for all parents. finalNodesInfo is already initialized from
    /// counter 0
    for (unsigned int l = 1; l < tmpResult.size(); l++) {
        Node::NodesInfo storeResult;
        SetTools::intersection(
            finalNodesInfo, tmpResult[l], storeResult);
        finalNodesInfo.swap(storeResult);
        storeResult.clear();
    }
}

/**
 * Computes relation between every pair of nodes in a graph that has
 * common parent.
 *
 * @param orderMap Post order map of a graph
 */
void
ControlDependenceGraph::computeRelations(const CDGOrderMap& orderMap) {
    /// Process nodes in post order, guarantees child nodes will be processed
    /// before their parent
    int mapSize = orderMap.size();
    for (int i = 0; i < mapSize; i++) {
        NodeDescriptor des =
            MapTools::keyForValue<NodeDescriptor>(orderMap, i);
        Node* node = graph_[des];
        /// MoveNodes are skipped here, they are always child of region
        if (node->isRegionNode()
            || node->isLoopEntryNode()
            || node->isPredicateNode()) {
            processRegion(node);
            continue;
        }
    }
}

/**
 * "Sorts" all the child nodes of a region in topological order based on
 * their control and data dependencies.
 *
 * @param regionNode Region node who's child nodes to process
 * @param cdgGraph Control Dependence subraph
 */
void
ControlDependenceGraph::processRegion(Node* regionNode) {

    /// Get all child nodes of region node

    EdgeSet edges = outEdges(*regionNode);

    std::vector<std::pair<Node*, Node*> > newEdges;
    for (EdgeSet::iterator ei1 = edges.begin();
        ei1 != edges.end();
        ++ei1) {
        Node* node1 = &headNode(**ei1);

        /// node1 will be compared against all the other previously untested
        /// nodes
        EdgeSet::iterator ei2 = ei1;
        ei2++;
        for (; ei2 != edges.end(); ++ei2) {
            Node* node2 = &headNode(**ei2);

            /// Test relation between siblings, should never return ERROR twice!
            CompareResult result = compareSiblings(node1, node2);
            if (result == ERROR) {
                result = compareSiblings(node2, node1);
            }
            switch(result) {
                case A_BEFORE_B:
                    newEdges.push_back(std::pair<Node*, Node*>(node1, node2));
                    break;
                case B_BEFORE_A:
                    newEdges.push_back(std::pair<Node*, Node*>(node2, node1));
                    break;
                case UNORDERABLE:
                    if(Application::verboseLevel() > DEBUG_LEVEL) {
                        Application::logStream() << (boost::format(
                            "Nodes %s and %s can not be put into any order.\n")
                            % node1->toString()% node2->toString()).str();
                    }
                    break;
                case ANY_ORDER:
                    break;
                case ERROR:
                    throw ModuleRunTimeError(
                        __FILE__, __LINE__, __func__, (boost::format(
                            "Ordering error for A='%s' and B='%s'.")
                            % node1->toString() % node2->toString()).str());
                    break;
            }
        }
    }
    return;
#if 0
    /// This is just for testing
    /// Add actuall control dependence edges for serialization
    /// Edges are really not needed here, they will be added into PDG
    for (unsigned int i = 0; i < newEdges.size(); i++) {
        ControlDependenceEdge* direction = new ControlDependenceEdge();
        connectNodes(*newEdges[i].first, *newEdges[i].second, *direction);
    }
    return;
#endif
}
