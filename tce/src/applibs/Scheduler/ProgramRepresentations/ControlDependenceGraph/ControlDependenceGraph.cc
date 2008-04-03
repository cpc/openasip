/**
 * @file ControlDependenceGraph.cc
 *
 * Implementation of prototype control dependence graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <iostream>

#include <vector>
#include <algorithm>
#include <functional>
#include <list>
#include <map>

#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/properties.hpp>

#include "ControlDependenceGraph.hh"
#include "ControlFlowGraph.hh"
#include "BasicBlockNode.hh"
#include "ControlFlowEdge.hh"
#include "BaseType.hh"
#include "MapTools.hh"
#include "Conversion.hh"

ControlDependenceGraph::~ControlDependenceGraph() {
    // removeNode() also removes edges and deletes them
    while (nodeCount() > 0) {
        ControlDependenceNode* n = &node(0);
        removeNode(*n);
        delete n;
    }
}
/**
 * Reads ControlFlowGraph of procedure and creates ControlDependenceGraph
 * @param cGraph Control Flow Graph
 */

ControlDependenceGraph::ControlDependenceGraph(
    const ControlFlowGraph& cGraph)  : 
    BoostGraph<ControlDependenceNode, ControlDependenceEdge>(
        cGraph.name()) , 
    startAddress_(TTAProgram::NullAddress::instance()) {
    
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
    std::vector<ControlDependenceNode*> cdNodes;
    detectControlDependencies(nodes, cdNodes, postOrder, dependencies);

    // edge added while creating CD dependencies, not part of CFG by
    // itself so it should be removed
    cGraph_->removeEntryExitEdge();
    // set of control dependencies for given region node
    DependenceMap regionNodes;

    // Creates region node for each set of control dependencies
    // and adds the record to regionNodes
    for (unsigned int i = 0; i < iDomTree.size();  i++) {
        ControlDependenceNode* b = cdNodes[i];
        if (!MapTools::containsKey(dependencies, b)) {
            continue;
        }
        DependenceMap::iterator rItr;
        rItr = regionNodes.begin();
        bool found = false;
        while (rItr != regionNodes.end()) {
            if (findSubset(dependencies[b], (*rItr).second, (*rItr).first)) {
                found = true;
            }
            rItr ++;
        }
        if (found == true && dependencies[b]->size() == 1) {
            // only one dependence or dependence identical
            // with already existing one
            createControlDependenceEdge(
                *dependencies[b]->at(0).first, *b);
            continue;
        }
        if (dependencies[b]->size() > 0) {
            // new region node
            ControlDependenceNode* cd = new ControlDependenceNode(
                nodeCount(), ControlDependenceNode::CDEP_NODE_REGION);
            addNode(*cd);
            DependentOn* dOn = new DependentOn(*(dependencies[b]));
            regionNodes.insert(
                std::pair<ControlDependenceNode*, DependentOn*>(cd, dOn));
            createControlDependenceEdge(*cd, *b);
        }
    }

    // created dependent edges INTO region nodes
    DependenceMap::iterator regionItr;
    regionItr = regionNodes.begin();
    while (regionItr != regionNodes.end()) {
        for (unsigned int i = 0; i < (*regionItr).second->size(); i++) {
            createControlDependenceEdge(
                *(*regionItr).second->at(i).first, *(*regionItr).first,
                (*regionItr).second->at(i).second);
        }
        regionItr++;
    }

    eliminateMultipleOutputs();
    // removes artificial Exit node, it is not dependent on anything
    for (int i = 0; i < nodeCount(); i++) {
        ControlDependenceNode& testNode = node(i);
        if (testNode.isExitNode()) {
            if (outDegree(testNode) == 0 && inDegree(testNode) == 0) {
                removeNode(testNode);
            }
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
    ControlDependenceNode* regNode) {

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
        SourceType(regNode,ControlDependenceEdge::CDEP_EDGE_NORMAL));
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
    ControlDependenceNode& bTail,
    ControlDependenceNode& bHead,
    ControlDependenceEdge::CDGEdgeType edgeValue) {

    ControlDependenceEdge* theEdge = 0;
    try {
        // By construction, there should not! be duplication of CD edges!!!
        if (hasEdge(bTail, bHead)) {
            theEdge = graph_[connectingEdge(bTail, bHead)];
        } else {
            theEdge = new ControlDependenceEdge(edgeCount(), edgeValue);
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
    // color map, needed by depth-first traversal algorithm
    typedef std::map < ControlFlowGraph::NodeDescriptor,
        boost::default_color_type > ColorMap;
    typedef boost::associative_property_map<ColorMap> Color;

    //
    bool modified = false;
    int counter = 0;
    std::vector<ControlFlowEdge*> addedEdges;
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
            *revGraph_, cGraph_->descriptor(cGraph_->exitNode()),
            boost::make_dfs_visitor(pOrderStamper), colors);
        // there can be just one node with post order number 0
        // if there are several, then some of them are not post
        // dominated by Exit - endless loop without break or return
        // we add and edge from one of those nodes to exit and redo
        // the dfs visit.
        std::vector<BasicBlockNode*> postZero;
        for (int i = cGraph_->nodeCount() - 1; i >=0; i--) {
            if (postOrder[cGraph_->descriptor(cGraph_->node(i))] == 0) {
                postZero.push_back(&cGraph_->node(i));
            }
        }
        if (postZero.size() > 1) {
            for (unsigned int i = 0; i < postZero.size(); i++) {
                if (!postZero[i]->isEntryBB()) {
                    ControlFlowEdge* edge = new
                    ControlFlowEdge(
                        cGraph_->edgeCount(),
                        ControlFlowEdge::CFLOW_EDGE_LOOP_BREAK,true);
                    cGraph_->connectNodes(
                        *postZero[i], cGraph_->exitNode(), *edge);
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
    } while (modified && counter < 10);
    delete revGraph_;
    revGraph_ = NULL;

    // tree of immediate dominators, mapping a node to its immediate
    // dominator; each node is represented by its inverted post-order number
    iDomTree.resize(cGraph_->nodeCount());
    const int startNodePO = cGraph_->nodeCount() - 1;
    // create inverse map from post-order to node
    // initialise tree of immediate dominators
    nodes.resize(cGraph_->nodeCount());
    for (unsigned int i = 0; i < nodes.size(); i++) {
        nodes[postOrder[cGraph_->descriptor(cGraph_->node(i))]] =
            &(cGraph_->node(i));
        iDomTree[i] = -1;
    }

    iDomTree[startNodePO] = startNodePO;
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
                if (iDomTree[predPO] != -1) {
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
                std::string message = 
                    "Missing postOrder number of predecessor!";
                throw InvalidData(__FILE__, __LINE__, __func__, message); 
            }            

            for (predIndex = predIndex + 1;
                predIndex < cGraph_->outDegree(b);
                predIndex++) {
                BasicBlockNode& predecessor(cGraph_->headNode(
                    cGraph_->outEdge(b, predIndex)));
                int predPO = postOrder[cGraph_->descriptor(predecessor)];
                if (iDomTree[predPO] != -1) {
                    newDom = nearestCommonDom(iDomTree, newDom, predPO);
                }
            }
            if (newDom != iDomTree[i]) {
                changed = true;
                iDomTree[i] = newDom;
            }
        }
    }
    for (unsigned int i = 0; i < addedEdges.size(); i++) {
        cGraph_->removeEdge(*addedEdges[i]);
    }
}

/**
 * Return the entry node of the graph.
 *
 * @return The entry node of the graph.
 * @exception InstanceNotFound if the graph does not have a entry node.
 * @exception InvalidData if the graph has multiple nodes that are
 *            recognised as entry nodes.
 */
ControlDependenceNode&
ControlDependenceGraph::entryNode() {
    ControlDependenceNode* result = NULL;
    bool found = false;
    bool unlinkedEntryNode = false;
    for (int i = 0; i < nodeCount(); i++) {
        if (inDegree(node(i)) == 0) {
            // sanity check
            if (!static_cast<ControlDependenceNode&>(node(i)).isEntryNode()) {
                // probably the entry node is not present
                unlinkedEntryNode = true;
                continue;
            }
            if (found == true) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Corrupted graph. Found multiple entry nodes.");
            }
            result = dynamic_cast<ControlDependenceNode*>(&node(i));
            found = true;
        }
    }
    if (found == false || result == NULL || unlinkedEntryNode == true) {
        string errorMsg("Graph does not have entry node.");
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    return *result;
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
    std::vector<ControlDependenceNode*>& cdNodes,
    PostOrder& postOrder,
    DependenceMap& dependencies) {

    std::map<BasicBlockNode*,ControlDependenceNode*> BBCD;
    for (int i = 0; i < cGraph_->nodeCount(); i++) {
        if (!MapTools::containsKey(BBCD, &(cGraph_->node(i)))) {
            ControlDependenceNode* cd = new ControlDependenceNode(
                nodeCount(), ControlDependenceNode::CDEP_NODE_BB, 
                &(cGraph_->node(i)));
            addNode(*cd);
            BBCD.insert(std::pair<BasicBlockNode*,ControlDependenceNode*>(
                &(cGraph_->node(i)), cd));
        }
        if (cGraph_->outDegree(cGraph_->node(i)) < 2) {
            // node is not predicate
            continue;
        }
        for (int j = 0 ; j < cGraph_->outDegree(cGraph_->node(i)); j++) {
            ControlDependenceEdge::CDGEdgeType edgeType =
                ControlDependenceEdge::CDEP_EDGE_NORMAL;
            BasicBlockNode *t = &(cGraph_->headNode(
                cGraph_->outEdge(cGraph_->node(i), j)));

            int nPO = postOrder[cGraph_->descriptor(cGraph_->node(i))];
            int tPO = postOrder[cGraph_->descriptor(*t)];
            int nPoDom = iDomTree[nPO];
            if (nPoDom == tPO) {
                // t is target of n, so if it post-dominate n
                // it is immediate post dominator!
                continue;
            }
            int runnerPo = tPO;
            if (cGraph_->outEdge(cGraph_->node(i), j).isTrueEdge()) {
                edgeType = ControlDependenceEdge::CDEP_EDGE_TRUE;
            }
            if (cGraph_->outEdge(cGraph_->node(i), j).isFalseEdge()) {
                edgeType = ControlDependenceEdge::CDEP_EDGE_FALSE;
            }
            SourceType newSource = SourceType(
                MapTools::valueForKey<ControlDependenceNode*>(
                BBCD, &(cGraph_->node(i))), edgeType);

            while (nPoDom != runnerPo) {
                // Walk through postDominator tree
                // Store found CD in multimap
                ControlDependenceNode* runnerCD = NULL;
                if (MapTools::containsKey(BBCD, nodes[runnerPo])) {
                    runnerCD = MapTools::valueForKey<ControlDependenceNode*>(
                        BBCD, nodes[runnerPo]);
                } else {
                    runnerCD = new ControlDependenceNode(nodeCount(),
                        ControlDependenceNode::CDEP_NODE_BB, nodes[runnerPo]);
                    addNode(*runnerCD);
                    BBCD.insert(
                        std::pair<BasicBlockNode*,ControlDependenceNode*>(
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
                        std::pair<ControlDependenceNode*, DependentOn*>(
                            runnerCD, dep));
                }
                runnerPo = iDomTree[runnerPo];
            }
        }
    }
    for (unsigned int i = 0; i < nodes.size(); i++) {
        ControlDependenceNode* cdNode;
        cdNode = MapTools::valueForKey<ControlDependenceNode*>(
            BBCD, nodes[i]);
        cdNodes.push_back(cdNode);
    }
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
 * region node
 */
void
ControlDependenceGraph::eliminateMultipleOutputs() {
    for (int i = 0; i < nodeCount(); i++) {
        if (outDegree(node(i)) > 1 && node(i).isBBNode()) {
            std::vector<ControlDependenceNode*> trueLinks;
            std::vector<ControlDependenceNode*> falseLinks;
            for (int j = 0; j < outDegree(node(i)); j++) {
                if (outEdge(node(i), j).isTrueEdge()) {
                    trueLinks.push_back(&headNode(outEdge(node(i), j)));
                }
                if (outEdge(node(i), j).isFalseEdge()) {
                    falseLinks.push_back(&headNode(outEdge(node(i), j)));
                }
            }
            if (trueLinks.size() > 1) {
                ControlDependenceNode* regNode = new ControlDependenceNode(
                    nodeCount(), ControlDependenceNode::CDEP_NODE_REGION);
                addNode(*regNode);
                createControlDependenceEdge(
                    node(i), *regNode, ControlDependenceEdge::CDEP_EDGE_TRUE);
                for (unsigned int j = 0; j < trueLinks.size(); j++) {
                    createControlDependenceEdge(*regNode, *trueLinks[j]);
                    disconnectNodes(node(i), *(trueLinks[j]));
                }
            }
            if (falseLinks.size() > 1) {
                ControlDependenceNode* regNode = new ControlDependenceNode(
                    nodeCount(), ControlDependenceNode::CDEP_NODE_REGION);
                addNode(*regNode);
                createControlDependenceEdge(
                    node(i), *regNode, 
                    ControlDependenceEdge::CDEP_EDGE_FALSE);
                for (unsigned int j = 0; j < falseLinks.size(); j++) {
                    createControlDependenceEdge(*regNode, *falseLinks[j]);
                    disconnectNodes(node(i), *(falseLinks[j]));
                }
            }
        }
    }
}
