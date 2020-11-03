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
 * @file ProgramDependenceGraph.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <utility>

// include this before boost to avoid deprecation
// warnings
#include "hash_set.hh"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/exception.hpp>
#include <boost/timer.hpp>
#include <boost/format.hpp>
#include <boost/graph/graphviz.hpp>

#include "ProgramDependenceGraph.hh"
#include "UniversalMachine.hh"
#include "Procedure.hh"
#include "POMDisassembler.hh"
#include "MapTools.hh"
#include "Exception.hh"
#include "MoveGuard.hh"
#include "SequenceTools.hh"
#include "InstructionReferenceManager.hh"
#include "InstructionReference.hh"
#include "TerminalInstructionReference.hh"
#include "TerminalFUPort.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "Move.hh"
#include "Instruction.hh"
#include "Program.hh"
#include "DataDependenceGraph.hh"
#include "BasicBlock.hh"

#define DEBUG_LEVEL 1

ProgramDependenceGraph::~ProgramDependenceGraph() {
    // Boost destructor will delete nodes from graph
    for (int i = 0; i < nodeCount(); i++) {
        Node* n = &node(i);
        delete n;
    }
    for (unsigned int i = 0; i < strongComponents_.size(); i++) {
        strongComponents_[i].clear();
    }
    strongComponents_.clear();
}

/**
 * Constructor.
 * Build Program Dependence Graph from Control and Data Dependence graphs.
 * @param cdg Control Dependence Graph of procedure
 * @param ddg Data Dependence Graph of procedure
 */
ProgramDependenceGraph::ProgramDependenceGraph(
    ControlDependenceGraph& cdg,
    DataDependenceGraph& ddg) :
    BoostGraph<ProgramDependenceNode,ProgramDependenceEdge>(cdg.name()),
    cdg_(&cdg),
    ddg_(&ddg),
    entryNode_(NULL),
    ddgEntryNode_(NULL),
    insCount_(0),
    wrongCounter_(0) {

    ControlToProgram cNodePNode;
    BBToCD BBNodeCDNode;
    MoveNodeToPDGNode movePD;
    MovesInCD moveToCD;
    program_ = cdg.program();

    // Creates PDG node for each region node of CDG
    for (int i = 0; i < cdg.nodeCount(); i++) {
        ControlDependenceNode& cnode = cdg.node(i);
        if (cnode.isRegionNode()
            || cnode.isEntryNode()
            || cnode.isLoopEntryNode()
            || cnode.isLoopCloseNode()) {

            ProgramDependenceNode::NodeType type =
                ProgramDependenceNode::PDG_NODE_REGION;
            if (cnode.isLoopCloseNode()) {
                type = ProgramDependenceNode::PDG_NODE_LOOPCLOSE;
            }
            if (cnode.isLoopEntryNode()) {
                type = ProgramDependenceNode::PDG_NODE_LOOPENTRY;
            }

            ProgramDependenceNode* newNode = new
                ProgramDependenceNode(cnode, type);
            if (cnode.isLastNode()) {
                newNode->setLastNode();
            }
            addNode(*newNode);
            cNodePNode.insert(
                std::pair<ControlDependenceNode*, ProgramDependenceNode*>(
                    &cnode, newNode));
            if (cnode.isEntryNode()) {
                BBNodeCDNode.insert(
                    std::pair<BasicBlockNode*, ControlDependenceNode*>(
                        cnode.basicBlockNode(), &cnode));
                entryNode_ = newNode;
            }
        } else {
            BBNodeCDNode.insert(
                std::pair<BasicBlockNode*, ControlDependenceNode*>(
                    cnode.basicBlockNode(), &cnode));
        }
    }
    assert(entryNode_ != NULL && "Entry node of the graph was not defined!");
    // Copies edges between region nodes of CDG into the PDG
    for (int i = 0; i < cdg.edgeCount(); i++) {
        ControlDependenceEdge& edge = cdg.edge(i);
        ControlDependenceNode* headNode;
        ControlDependenceNode* tailNode;
        headNode = &cdg.headNode(edge);
        tailNode = &cdg.tailNode(edge);
         // CDG predicate node is special type of BB node with 2 outgoing
         // edges
        // Entry node is special kind of BB, which we are also interested in
        // here
        if (!headNode->isBBNode()
            && !headNode->isPredicateNode()
            && (!tailNode->isBBNode() || tailNode->isEntryNode())
            && !tailNode->isPredicateNode()) {
            // headNode is one with arrow in boost graphs
            ProgramDependenceNode* sourceNode;
            ProgramDependenceNode* targetNode;
            try {
               sourceNode = MapTools::valueForKey<ProgramDependenceNode*>(
                   cNodePNode, tailNode);
               targetNode = MapTools::valueForKey<ProgramDependenceNode*>(
                   cNodePNode, headNode);
            } catch (const Exception& e) {
                  throw InvalidData(
                     __FILE__, __LINE__, __func__, e.errorMessageStack());
            }
            ProgramDependenceEdge* pEdge;
            pEdge = new ProgramDependenceEdge(edge);
            connectNodes(*sourceNode, *targetNode, *pEdge);
        }
    }

    // Add each MoveNode of DDG also to PDG
    for (int i = 0; i < ddg.nodeCount(); i++) {
        Node* newNode = NULL;
        Node::NodeType typeOfNode = Node::PDG_NODE_MOVE;
        // Guarded jumps and calls becomes predicates
        MoveNode& node = ddg.node(i);
        if (node.isMove() &&
            node.move().isControlFlowMove() &&
            !node.move().isUnconditional()) {
            typeOfNode =  Node::PDG_NODE_PREDICATE;
        } else if (node.isMove() &&
            node.move().isControlFlowMove() &&
            node.move().isJump() &&
            !node.move().isReturn()){
            /// Ignore unconditional jumps
            continue;
        }
        newNode = new ProgramDependenceNode(node, typeOfNode);
        addNode(*newNode);
        if(!node.isMove()) {
            ddgEntryNode_ = newNode;
        }
        movePD.insert(std::pair<MoveNode*, ProgramDependenceNode*>(
            &node, newNode));
    }

    // Add the edges between MoveNodes in DDG
    for (int j = 0; j < ddg.edgeCount(); j++) {
        ProgramDependenceNode* pSource = NULL;
        ProgramDependenceNode* pTarget = NULL;
        DataDependenceEdge& edge = ddg.edge(j);
        if (!MapTools::containsKey(movePD, &ddg.tailNode(edge))) {
            continue;
        }
        pSource = MapTools::valueForKey<ProgramDependenceNode*>(
            movePD, &ddg.tailNode(edge));
        if (!MapTools::containsKey(movePD, &ddg.headNode(edge))) {
            continue;
        }
        pTarget = MapTools::valueForKey<ProgramDependenceNode*>(
            movePD, &ddg.headNode(edge));
        ProgramDependenceEdge* pEdge;
        pEdge = new  ProgramDependenceEdge(edge);
        connectNodes(*pSource, *pTarget, *pEdge);
    }

    // edges between region nodes and move nodes
    for (int i = 0; i < ddg.nodeCount(); i++) {
        // Some jumps were not copied so we skip them when adding edges
        MoveNode& node = ddg.node(i);
        if (!MapTools::containsKey(movePD, &node)) {
            continue;
        }

        BasicBlockNode* b = NULL;
        b = &ddg.getBasicBlockNode(node);
        ControlDependenceNode* cNode = NULL;
        if (!MapTools::containsKey(BBNodeCDNode, b)) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "No Control Dependence Node for Basic Block Node!");
        }
         try {
            cNode = MapTools::valueForKey<ControlDependenceNode*>(
                BBNodeCDNode, b);
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessageStack());
        }

        ProgramDependenceNode* pNode = NULL;
        try {
            pNode = MapTools::valueForKey<ProgramDependenceNode*>(
                movePD, &node);
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessageStack());
        }
        if (pNode->isMoveNode() && pNode->moveNode().isMove() == false) {
            /// Found dummy entry node of ddg, connect it to entry node of
            /// pdg as well.
            ControlDependenceEdge* cdgEdge = new ControlDependenceEdge();
            ProgramDependenceEdge* pEdge = new ProgramDependenceEdge(*cdgEdge);
            connectNodes(entryNode(), *pNode, *pEdge);
            continue;
        }
        if (!MapTools::containsKey(moveToCD, cNode)) {
            std::vector<Node*> tmp;
            tmp.push_back(pNode);
            moveToCD[cNode] = tmp;
        } else {
            std::vector<Node*> tmp = moveToCD[cNode];
            tmp.push_back(pNode);
            moveToCD[cNode] = tmp;
        }
        // For each MoveNode, find in which Basic Block it was
        // and all input edges that went into CDG for given Basic Block
        // are also added to the MoveNode
        for (int j = 0; j < cdg.inDegree(*cNode); j++) {
            ControlDependenceNode* source;
            source = &cdg.tailNode(cdg.inEdge(*cNode, j));
            if (source->isRegionNode()
                || source->isEntryNode()
                || source->isLoopEntryNode()
                || source->isLoopCloseNode()) {
                ProgramDependenceEdge* pEdge;
                pEdge = new
                    ProgramDependenceEdge(cdg.inEdge(*cNode, j));
                ProgramDependenceNode* pSource = NULL;
                try {
                    pSource = MapTools::valueForKey<ProgramDependenceNode*>(
                        cNodePNode, source);
                } catch (const Exception& e) {
                    throw InvalidData(
                        __FILE__, __LINE__, __func__, e.errorMessageStack());
                }
                connectNodes(*pSource, *pNode, *pEdge);
            } else {
                abortWithError("The source of control dependence is not\
                    region node!");
            }
        }
        if (pNode->isPredicateMoveNode() &&
            pNode->moveNode().move().isJump()){
            removeGuardedJump(cNodePNode, *pNode, *cNode);
            removeNode(*pNode);
        }
    }
    /// If CDG already has analysis of Region and EEC done, just copy
    /// results
    if (cdg.analyzed()) {
        copyRegionEECComponent(cNodePNode, BBNodeCDNode, movePD, moveToCD);
    }
}

/**
 * Remove guarded jumps from the graph, makes guard generating operation
 * a predicated node and fixes the true and false edges in case the jump
 * had inverted guard.
 * @param cToP mapping between Control Dependence nodes of original CDG and
 * newly created Program Dependence nodes in PDG.
 * @param pNode Program Dependence Node containing guarded jump
 * @param cNode Control Dependence node which included guarded jump in CDG
 */
void
ProgramDependenceGraph::removeGuardedJump(
    ControlToProgram& cToP,
    ProgramDependenceNode& pNode,
    ControlDependenceNode& cNode) {

    ProgramDependenceNode* guardSource = NULL;
    bool isInverted = pNode.moveNode().move().guard().isInverted();
    if (inDegree(pNode) != 2) {
        /// This used to be error, but now it possible
        if(Application::verboseLevel() > 0) {
            Application::logStream() << (boost::format(
            "Guarded jump (%s) has inDegree different from 2! Degree =%d.\n")
                % pNode.toString() % inDegree(pNode)).str();
            EdgeSet e = inEdges(pNode);
            for (EdgeSet::iterator ei = e.begin(); ei != e.end(); ei++) {
                Node* n = & tailNode(**ei);
                Application::logStream() << n->toString() << " "
                     << (*ei)->toString() << " ";
            }
        }
    }
    for (int i = 0; i < inDegree(pNode); i++) {
        if (inEdge(pNode,i).isDataDependence()) {
            guardSource = &tailNode(inEdge(pNode,i));
            break;
        }
    }
    if (guardSource == NULL) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Guarded jump did not have source of guard defined!");
    }
    guardSource->setPredicateMoveNode();
    /// All incoming control dependence edges into the predicate node
    /// are also incoming edges to the guard, since they were in same
    /// block in CDG
    for (int j = 0; j < cdg_->outDegree(cNode); j++) {
        ControlDependenceNode* target;
        target = &cdg_->headNode(cdg_->outEdge(cNode, j));
        if (target->isRegionNode()
            || target->isLoopEntryNode()
            || target->isLoopCloseNode()) {
            ProgramDependenceEdge* pEdge;
            ControlDependenceEdge* newEdge = &cdg_->outEdge(cNode, j);;
            if (isInverted) {
                newEdge->invertEdgePredicate();
            }
            pEdge = new ProgramDependenceEdge(*newEdge);
            ProgramDependenceNode* pTarget;
            try {
                pTarget = MapTools::valueForKey<ProgramDependenceNode*>(
                    cToP, target);
            } catch (const Exception& e) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__, e.errorMessageStack());
            }

            connectNodes(*guardSource, *pTarget, *pEdge);
        } else {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Basic block with guarded jump does not have"
                " Region nodes as control dependent!");
        }
    }
}
/**
 * Performs serialization of ProgramDependenceGraph, turning it into
 * Control Flow Graph.
 *
 * @return ControlFlowGraph representation of PDG
 * @throw InvalidData in case serialization of graph is not possible
 */
bool
ProgramDependenceGraph::serializePDG() {

    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\tStarting PDG serialization for %s with %d nodes and %d edges.\n")
            % name() % nodeCount() % edgeCount()).str();
    }

    /// Created filtered PDG graph containing only control dependence edges
    CDGFilter<Graph> filter(graph_);
    FilteredCDG  filteredCDG = FilteredCDG(graph_, filter);
    boost::timer timer;
    long elapsed = 0;
    /// Detect strong components if they were not detected in CDG and transferred
    if (!cdg_->analyzed()) {
        PDGOrderMap componentMap;
        DescriptorMap rootMap;
        /// Modifies graph_ with added close nodes and close edges
        int componentCount = detectStrongComponents(
            componentMap, rootMap, filteredCDG);
        elapsed = static_cast<long>(timer.elapsed());
        if (Application::verboseLevel() > DEBUG_LEVEL) {
            Application::logStream() << (boost::format(
            "\t\tStrong components:%d components, %d minutes and %d seconds.\n")
            % componentCount % (elapsed/60) % (elapsed%60)).str();
        }
    }

    /// map to store post order information for all the nodes of a graph
    PDGOrderMap lastMap;
    PDGOrder lastOrder(lastMap);
    /// map to store color information during dfs
    ColorMap colorMap;
    Color colorsDFS(colorMap);
    int fStamp(-1);
    /// boost::on_finish_vertex will give us post order numbering
    boost::time_stamper<PDGOrder, int, boost::on_finish_vertex>
        lastOrderStamper(lastOrder, fStamp);
    timer.restart();
    /// Computes post order of all the nodes in PDG
    boost::depth_first_visit(
        filteredCDG, descriptor(entryNode()),
        boost::make_dfs_visitor(lastOrderStamper), colorsDFS);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tPost order: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }
    /// If not computed on CDG and copied, computes 'region' information for
    /// all nodes of a graph
    if (!cdg_->analyzed()) {
        timer.restart();
        computeRegionInfo(lastMap, filteredCDG);
        elapsed = static_cast<long>(timer.elapsed());
        if (Application::verboseLevel() > DEBUG_LEVEL) {
            Application::logStream() << (boost::format(
                "\t\tRegion: %d minutes and %d seconds.\n")
                % (elapsed/60) % (elapsed%60)).str();
        }
        /// If not computed on CDG and copied, computes 'eec' information for
        /// all nodes of a graph
        timer.restart();
        computeEECInfo(lastMap, filteredCDG);
        elapsed = static_cast<long>(timer.elapsed());
        if (Application::verboseLevel() > DEBUG_LEVEL) {
            Application::logStream() << (boost::format(
                "\t\tEEC: %d minutes and %d seconds.\n")
                % (elapsed/60) % (elapsed%60)).str();
        }
    }

    timer.restart();
    /// If CDG comes analyzed we need to add region and eec info for
    /// dummy ENTRYNODE of DDG. By it's definition, ENTRYNODE is leaf
    /// in terms of control dependence, region == eec
    if(cdg_->analyzed() && ddgEntryNode_ != NULL) {
        Node::NodesInfo finalNodesInfo;
        regionHelper(ddgEntryNode_, filteredCDG, finalNodesInfo);
        for (Node::NodesInfo::iterator k = finalNodesInfo.begin();
            k != finalNodesInfo.end(); k++) {
            ddgEntryNode_->addToRegion(**k);
            ddgEntryNode_->addToEEC(**k);
        }
    }
    return true;
#if 0
    /// Compute actual relations between sibling nodes. Move Data Dependence
    /// edges to root's of subgraph if they connect subgraph with outside
    /// nodes
    ///computeRelations(lastMap, filteredCDG);
    elapsed = static_cast<long>(timer.elapsed());
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "\t\tRelations: %d minutes and %d seconds.\n")
            % (elapsed/60) % (elapsed%60)).str();
    }
    if (Application::verboseLevel() > DEBUG_LEVEL) {
        Application::logStream() << (boost::format(
            "Procedure %s has %d nodes and %d edges.\n")
            % name() % nodeCount() % edgeCount()).str();
    }
    //writeToDotFile(name() + "_pdgSerialized.dot");
    //disassemble();
    return true;
#endif
}

/**
 * Detects all strong components of a PDG graph (loops). Strong components are
 * maximal sets of nodes that are reachable from each other. Each node is member
 * of some component. If it is not in loop then node is components of it's own.
 * Augments graph with loop entry and close nodes.
 * Works iteratively, first detects maximal component, then proceeds to ones
 * that are "embedded".
 *
 * @param components After return contains for each node number of component
 * to which node belongs
 * @param roots After return contains for each node a node that is root of
 * component to which node belongs
 * @return returns number of components in a graph
 */
int
ProgramDependenceGraph::detectStrongComponents(
    PDGOrderMap& components,
    DescriptorMap& roots,
    FilteredCDG& cdg) {

    std::vector<std::pair<Node*, Node*> >  backEdges;
    int componentCount = 0;
    int currentComponents = 0;
    int numberOfNodes = 0;

    /// Will repeat until all the strong components will be found
    /// Including weirdly nested :-)
    do {
        PDGOrder componentOrder(components);
        Descriptors componentRoots(roots);
        currentComponents = 0;
        /// Node count will change with addition of close nodes
        numberOfNodes = boost::num_vertices(cdg);
        currentComponents =  boost::strong_components(
            cdg, componentOrder, boost::root_map(componentRoots));

        // for each component add vector of nodes that belongs to it
        std::vector<std::set<Node*> >  componentVector;
        componentVector.resize(componentCount + currentComponents);

        /// If the number of components is identical to number of nodes
        /// there are no loops in graph
        if (currentComponents == numberOfNodes) {
            componentCount = strongComponents_.size();
            break;
        }

        /// Add to strong components only those which are loops
        /// Store them as Node*, use of descriptors is not possible
        /// due to later addition of Nodes which will invalidate
        /// descriptors
        for (PDGOrderMap::iterator iterA = components.begin();
            iterA != components.end(); iterA ++) {
                Node* cNode = cdg[(*iterA).first];
                componentVector[(*iterA).second].insert(cNode);
        }
        for (unsigned int i = componentCount; i < componentVector.size(); i++){
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

                FilteredVertexDescriptor vec;
                vec = descriptor(**iterD);
                FilteredInEdgePair edges = boost::in_edges(vec, cdg);
                for (FilteredInEdgeIter ei = edges.first;
                    ei != edges.second;
                    ++ei) {

                    Node* testedNode = cdg[boost::source(*ei, cdg)];
                    if (nodes.find(testedNode) == nodes.end()) {
                        /// tail of the edge is not inside component
                        /// it is external edge making this node loop entry
                        if (!(*iterD)->isLoopEntryNode(i)) {
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
        /// Node and Edge descriptors are invalidated here!
        for (unsigned int j = 0; j < newEntry.size(); j++) {
            Node* loopNode = newEntry[j].first;
            std::set<Node*>& nodes =
                strongComponents_[newEntry[j].second];
            /// Create one "close" node for each loop entry
            Node* close = new Node(Node::PDG_NODE_LOOPCLOSE);
            close->setComponent(newEntry[j].second);
            addNode(*close);

            /// Close node is also part of component
            strongComponents_[newEntry[j].second].insert(close);

            FilteredVertexDescriptor vec;
            vec = descriptor(*loopNode);
            FilteredInEdgePair edges = boost::in_edges(vec, cdg);
            std::vector<Edge*> storeEdges;
            /// Redirect edges to loop entry from inside the loop
            /// to loop close node. Collect edges that needs redirecting
            for (FilteredInEdgeIter ei = edges.first;
                ei != edges.second;
                ++ei) {
                Node* sourceNode = cdg[boost::source(*ei, cdg)];
                if (nodes.find(sourceNode) != nodes.end()){
                    /// store edges that will have to be moved
                    Edge* edge = cdg[*ei];
                    storeEdges.push_back(edge);
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

            /// Close node is also added to unfiltered pdg, as well as edges
            /// so we can test it directly there and not on filtered graph
            if (inDegree(*close) == 0) {
                TCEString msg = "Close node for loop entry node ";
                msg += loopNode->toString() + " was not connected!";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
            }
            /// In case loop entry has multiple incoming edges
            /// outside loop, we add new region to group them together
            /// This can be also on original pdg, no need to use filtered
            /// version
            std::vector<Node*> tmp =
                MapTools::valueForKey<std::vector<Node*> >(
                    incomingToEntry, loopNode);
            if (tmp.size() > 1) {
                /// Creates new artificial region node without equivalent
                /// one in CDG
                Node* collect = new Node();
                addNode(*collect);
                for (unsigned int i = 0; i < tmp.size(); i++) {
                    Node* input = tmp[i];
                    EdgeDescriptor ed = connectingEdge(*input, *loopNode);
                    Edge* e = graph_[ed];
                    moveInEdge(*loopNode, *collect, *e);
                }
                ProgramDependenceEdge* pdgEdge =
                    new ProgramDependenceEdge();
                connectNodes(*collect, *loopNode, *pdgEdge);
            }
        }
        newEntry.clear();
        componentCount = strongComponents_.size();
   } while (true);

    /// Add edges from close nodes to their respective loop entries
    for (unsigned int i = 0; i < backEdges.size(); i++) {
        ProgramDependenceEdge* pdgEdge =
            new ProgramDependenceEdge(
                ProgramDependenceEdge::PDG_EDGE_LOOP_CLOSE);
        connectNodes(*backEdges[i].first, *backEdges[i].second, *pdgEdge);
    }

#if 0
    for (unsigned int i = 0; i < strongComponents_.size() ; i++) {
        std::cerr << "\tComponent: " << i << std::endl;
        for (std::set<Node*>::iterator iter = strongComponents_[i].begin();
             iter != strongComponents_[i].end(); iter++) {
            std::cerr << "\t\t" << (*iter)->toString() << std::endl;
        }
        std::cerr << std::endl;
    }
#endif
    return componentCount;
}

/**
 * Compute the "region" information for each node of graph. Region is used to
 * compute "eec" to determine order in which sibling subgraphs will be in
 * resulting cfg.
 *
 * "Region" of node X is set of nodes that will be executed in case is X
 * executed.
 *
 * @param orderMap post order map of PDG graph, nodes will be augmented with
 * "region" information.
 * @param cdg Program Dependence Graph filtered to us only control dependence
 * edges.
 */
void
ProgramDependenceGraph::computeRegionInfo(
    const PDGOrderMap& orderMap,
    FilteredCDG& cdg){

    int mapSize = orderMap.size();
    if (mapSize == 0) {
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
            "No nodes in CDG graph for " + name() + "!");
    }
    /// Compute "region" information using reverse post-order processing
    /// Node descriptors in filtered graph are identical to original graph
    /// we only care about edge filtering here
    for (int i = mapSize -1 ; i >= 0 ; i--) {
        NodeDescriptor des =
            MapTools::keyForValue<NodeDescriptor>(orderMap,i);
        Node* node = graph_[des];
        if (!node->isLoopEntryNode()) {
            /// For non loop entry nodes, simply compute region info
            /// and store it in the node
            Node::NodesInfo finalNodesInfo;
            regionHelper(node, cdg, finalNodesInfo);
            for (Node::NodesInfo::iterator k = finalNodesInfo.begin();
                k != finalNodesInfo.end(); k++) {
                node->addToRegion(**k);
            }

        } else if (node->region().size() == 0) {
            /// for loop entry node, find all other entry nodes
            /// of the same loop (component)
            /// final region info will be intersection of region info from
            /// all the entry nodes in the same loop
            /// it will be same for all the nodes too (they are reachable)
            std::vector<Node*> entries;
            std::set<Node*> component = strongComponents_[node->component()];
            for (std::set<Node*>::iterator si = component.begin();
                si != component.end(); si++) {
                /// Check if node is loop entry node of tested component
                if ((*si)->isLoopEntryNode(node->component())) {
                    entries.push_back(*si);
                }
            }
            Node::NodesInfo finalNodesInfo;
            for (unsigned int i = 0; i < entries.size(); i++) {
                /// for every entry node of the loop compute region info
                Node* entryNode = entries[i];
                regionHelper(entryNode, cdg, finalNodesInfo);
            }
            for (unsigned j = 0; j < entries.size(); j++) {
                Node* result = entries[j];
                for (Node::NodesInfo::iterator k = finalNodesInfo.begin();
                    k != finalNodesInfo.end();
                    k++) {
                    result->addToRegion(**k);
                }
            }
        }
    }
}

/**
 * Compute the "eec" information for each node of graph. EEC is used to
 * determine order in which sibling subgraphs needs to be scheduled in
 * resulting cfg.
 *
 * "eec" of node X is set of nodes that will be executed in case any of the
nodes
 * in subgraph of X will be executed.
 *
 * @param orderMap post order map of PDG graph, nodes augmented with
 * "region" information, "eec" information will be added.
 */
void
ProgramDependenceGraph::computeEECInfo(
    const PDGOrderMap& orderMap,
    FilteredCDG& filteredCDG) {

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
                    || (*si)->isPredicateMoveNode()) {
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
            // add intersection of all region info to each close node in loop
            for (unsigned j = 0; j < closeNodes.size(); j++) {
                Node* result = closeNodes[j];
                /// Close nodes eec must be empty before we get here
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

         } else if (boost::out_degree(descriptor(*node), filteredCDG) == 0) {
            /// Found leaf node, eec(node) == region(node)
            Node::NodesInfo regionX = node->region();
            for (Node::NodesInfo::iterator t = regionX.begin();
                 t != regionX.end(); t++) {
                     node->addToEEC( **t);
                 }
        } else {
            /// Not a leaf node,
            /// eec(x) = intersection(eec(children(x)) \ children(x)
            Node::NodesInfo childNodes;
            FilteredOutEdgePair edges =
                boost::out_edges(descriptor(*node), filteredCDG);
            for (FilteredOutEdgeIter ei = edges.first;
                ei != edges.second;
                ++ei) {
                Node* testedNode = filteredCDG[boost::target(*ei, filteredCDG)];
                childNodes.insert(testedNode);
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
            // compute set difference, returns iterator pointing to
            // .end() of the result
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
 * Compare sibling nodes and determines their ordering relation
 * based on eec information. In addition, nodes marked as lastNode
 * are always ordered later in case ANY_ORDER is available.
 *
 * @param a First node to compare
 * @param b Second node to compare
 * @return value determining relation
 */
ProgramDependenceGraph::CompareResult
ProgramDependenceGraph::compareSiblings(Node* a, Node* b) {
    bool AinA = false;
    bool BinB = false;
    bool BinA = false;
    bool AinB = false;

#ifdef AVOID_COMPILER_WARNINGS_REMOVE_THESE_LINES
    bool extra = false;
    if (a->isPredicateMoveNode() || b->isPredicateMoveNode()) {
        extra = true;
    }
#endif
    /// Both a and b are simple statements, order is given by data dependencies
    /// no need to test eec info
    if (a->isMoveNode() && b->isMoveNode()
        && !a->isPredicateMoveNode() && !b->isPredicateMoveNode()) {
        return ANY_ORDER;
    }
    /// Find nodes relations in eec
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
    //std::cerr << "AinA=" << AinA << ", BinB=" << BinB <<", AinB=" << AinB
      // << ", BinA=" << BinA << std::endl;
    if ((a->isLoopEntryNode() || a->isPredicateMoveNode())
        && (b->isMoveNode() && !b->isPredicateMoveNode())
        && AinA == true) {
        if (a->isLastNode()) {
            return B_BEFORE_A;
        }
        return ANY_ORDER;
    }
    if ((b->isLoopEntryNode() || b->isPredicateMoveNode())
        && (a->isMoveNode() && !a->isPredicateMoveNode())
        && BinB == true) {
        if (b->isLastNode()) {
            return A_BEFORE_B;
        }
        return ANY_ORDER;
    }
    if ((a->isLoopEntryNode() || a->isPredicateMoveNode())
        && (b->isLoopEntryNode() || b->isPredicateMoveNode())
        && AinA == true && BinB == true) {
        if (a->isLastNode()) {
            return B_BEFORE_A;
        }
        if (b->isLastNode()) {
            return A_BEFORE_B;
        }
        return ANY_ORDER;
    }
    if ((a->isLoopEntryNode() || a->isPredicateMoveNode())
        && (b->isMoveNode() && !b->isPredicateMoveNode())
        && AinA == false) {
        return B_BEFORE_A;
    }
    if ((b->isLoopEntryNode() || b->isPredicateMoveNode())
        && (a->isMoveNode() && !a->isPredicateMoveNode())
        && BinB == false) {
        return A_BEFORE_B;
    }
    if ((a->isLoopEntryNode() || a->isPredicateMoveNode())
        && (b->isLoopEntryNode() || b->isPredicateMoveNode())
        && AinA == false && BinB == true) {
        return B_BEFORE_A;
    }
    if ((b->isLoopEntryNode() || b->isPredicateMoveNode())
        && (a->isLoopEntryNode() || a->isPredicateMoveNode())
        && AinA == true && BinB == false) {
        return A_BEFORE_B;
    }
    if ((a->isLoopEntryNode() || a->isPredicateMoveNode())
        && (b->isLoopEntryNode() || b->isPredicateMoveNode())
        && AinA == false && BinB == false) {
        //std::cerr << "\tUnorderable 1" << std::endl;
        return UNORDERABLE;
    }
    if ((a->isRegionNode() && AinB == true)
        && (b->isMoveNode() ||
            b->isLoopEntryNode() ||
            b->isPredicateMoveNode())){
        return B_BEFORE_A;
    }
    if ((b->isRegionNode() && BinA == true)
        && (a->isMoveNode() ||
            a->isLoopEntryNode() ||
            a->isPredicateMoveNode())){
        return A_BEFORE_B;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isLoopEntryNode() || b->isPredicateMoveNode())
        && AinB == false) {
        //std::cerr << "\tUnorderable 2" << std::endl;
        return UNORDERABLE;
    }
    if ((b->isRegionNode() && BinA == false)
        && (a->isLoopEntryNode() || a->isPredicateMoveNode())
        && BinA == false) {
        //std::cerr << "\tUnorderable 3" << std::endl;
        return UNORDERABLE;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isRegionNode() && BinA == true)) {
        return B_BEFORE_A;
    }
    if ((b->isRegionNode() && BinA == false)
        && (a->isRegionNode() && AinB == true)) {
        return A_BEFORE_B;
    }
    if ((a->isRegionNode() && AinB == false)
        && (b->isRegionNode() && BinA == false)) {
        return UNORDERABLE;
    }
    if ((a->isLoopCloseNode() && AinB == true)
        && (b->isMoveNode() || b->isPredicateMoveNode() || b->isLoopEntryNode()
            || b->isRegionNode())) {
        return B_BEFORE_A;
    }
    if ((b->isLoopCloseNode() && BinA == true)
        && (a->isMoveNode() || a->isPredicateMoveNode() || a->isLoopEntryNode()
            || a->isRegionNode())) {
        return A_BEFORE_B;
    }
    if ((a->isLoopCloseNode() && AinB == false)
        && (b->isPredicateMoveNode() || b->isLoopEntryNode()
            || b->isRegionNode())) {
        //std::cerr << "\tUnorderable 5" << std::endl;
        return UNORDERABLE;
    }
    if ((b->isLoopCloseNode() && BinA == false)
        && (a->isPredicateMoveNode() || a->isLoopEntryNode()
            || a->isRegionNode())) {
        //std::cerr << "\tUnorderable 6" << std::endl;
        return UNORDERABLE;
    }
    /// FIXME:
    /// Unique region node rule is broken when creating loop
    /// entry nodes (removing loop back edge) and creating new region for
    /// incoming edges into loop entry - there can be another region
    /// which already has same set of dependencies and loop entry was
    /// supposed to be child of that, but was not. Problem with detection
    /// of subsets of dependencies when creating region nodes!
    if ((a->isRegionNode() && AinB == true)
        && (b->isRegionNode() && BinA == true)) {
        Application::logStream() << (boost::format(
            "Found two regions with identical control dependencies. "
            "Known issue with CDG detection not reusing regions.\n")).str();
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
 * Returns entry node of the graph.
 * @return Entry node
 */
ProgramDependenceNode&
ProgramDependenceGraph::entryNode() const {
    if (entryNode_ == NULL ) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            "Trying to get entry node before it was defined!");
    }
    return *entryNode_;
}

/**
 * Helper function to compute actual region information for a node.
 * Called several times for a case when there are loop entry nodes
 * detected.
 *
 * @param node Node for which to compute region info
 * @param cdg Filtered PDG graph with only control dependence edges
 * @param finalNodesInfo stores actual computed region info
 */
void
ProgramDependenceGraph::regionHelper(
    Node* node,
    FilteredCDG& cdg,
    Node::NodesInfo& finalNodesInfo){

    NodeDescriptor des = descriptor(*node);
    std::vector<Node::NodesInfo> tmpResult;
    /// Find all incoming control dependence edges
    FilteredInEdgePair edges = boost::in_edges(des, cdg);
    for (FilteredInEdgeIter ei = edges.first;
        ei != edges.second;
        ++ei) {
        Node* previous = cdg[boost::source(*ei, cdg)];
        /// There is no Region info for Entry node
        if (previous->isRegionNode()
            || previous->isLoopEntryNode()) {
            if (previous->region().size() == 0 &&
                ! (previous == entryNode_)) {
                /// If parent's region is not yet computed (should be btw)
                /// compute it before continuing.
                Node::NodesInfo addedNodesInfo;
                regionHelper(previous, cdg, addedNodesInfo);
                for (Node::NodesInfo::iterator k = addedNodesInfo.begin();
                    k != addedNodesInfo.end();
                    k++) {
                    previous->addToRegion(**k);
                }

/*                writeToDotFile(name() + "_broken.dot");
                TCEString msg = "Parent " + previous->toString();
                msg += " of node " + (*iter)->toString();
                msg += " has empty region information!";
                msg += " Procedure has " + Conversion::toString(nodeCount());
                msg += " nodes.";
                throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);*/
            }
            /// region(node,parent) == region(parent) U children(parent)
            Node::NodesInfo tmp = previous->region();
            FilteredOutEdgePair edges =
                boost::out_edges(descriptor(*previous), cdg);
            for (FilteredOutEdgeIter ei = edges.first;
                ei != edges.second;
                ++ei) {
                Node* testedNode = cdg[boost::target(*ei, cdg)];
                tmp.insert(testedNode);
            }
            tmpResult.push_back(tmp);
        } else if (previous->isPredicateMoveNode()){
        /// region(node,parent) == region(parent)
            Node::NodesInfo tmp = previous->region();
            tmpResult.push_back(tmp);
        } else {
            assert(previous->isLoopCloseNode());
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
 * "Sorts" all the child nodes of a region in topological order based on
 * their control and data dependencies.
 *
 * @param regionNode Region/Predicate node who's child nodes to process
 * @param filteredCDG Control Dependence subraph
 */
void
ProgramDependenceGraph::processRegion(
    Node* regionNode,
    FilteredCDG& filteredCDG) {

    /// Get all child nodes of region node
    NodeDescriptor nodeDes = descriptor(*regionNode);
    FilteredOutEdgePair edges = boost::out_edges(nodeDes, filteredCDG);
    /// Store all child nodes, we will need to collect all edges
    /// to and from subgraph rooted by region
    NodeSet subgraphNodes;
    subgraphNodes.insert(regionNode);
    std::vector<std::pair<Node*, Node*> > newEdges;
    std::vector<std::pair<Node*, Node*> > unorderable;
    for (FilteredOutEdgeIter ei1 = edges.first;
        ei1 != edges.second;
        ++ei1) {
        NodeDescriptor des = boost::target(*ei1, filteredCDG);
        Node* node1 = graph_[des];
        subgraphNodes.insert(node1);

        /// node1 will be compared against all the other previously untested
        /// nodes
        FilteredOutEdgeIter ei2 = ei1;
        ei2++;
        for (; ei2 != edges.second; ++ei2) {
            Node* node2 = graph_[boost::target(*ei2, filteredCDG)];
            /// Test relation between siblings, should never return ERROR!
            CompareResult result = compareSiblings(node1, node2);
            switch(result) {
                case ERROR:
                    if(Application::verboseLevel() > 0) {
                        writeToDotFile(name() + "_pdg_broken.dot");
                        node1->printRelations();
                        node2->printRelations();
                    }
                    throw ModuleRunTimeError(
                        __FILE__, __LINE__, __func__,
                        (boost::format(
                            "Ordering error for A='%s' and B='%s'")
                            % node1->toString() % node2->toString()).str());
                case A_BEFORE_B:
                    newEdges.push_back(std::pair<Node*, Node*>(node1, node2));
                    break;
                case B_BEFORE_A:
                    newEdges.push_back(std::pair<Node*, Node*>(node2, node1));
                    break;
                case UNORDERABLE:
                    if(Application::verboseLevel() >= 0) {
                        Application::logStream() << (boost::format(
                            "Nodes (%s) and (%s) can not "
                            "be put into any order.\n")
                            % node1->toString()% node2->toString()).str();
                    }
                    unorderable.push_back(
                        std::pair<Node*, Node*>(node1, node2));
                    break;
                case ANY_ORDER:
                    break;
            }

        }
    }
    /// Move/copy edges between nodes "inside" subgraph and nodes outside the
    /// subgraph
    moveDDGedges(*regionNode, subgraphNodes, filteredCDG);
    /// Add actual control dependence edges for serialization
    for (unsigned int i = 0; i < newEdges.size(); i++) {
        ProgramDependenceEdge* direction = new ProgramDependenceEdge();
        connectNodes(*newEdges[i].first, *newEdges[i].second, *direction);
    }
    newEdges.clear();
    //return;
    /// Nodes of subgraph without it's root
    SubgraphTypeTest<NodeSet, Graph> subgraph(
        regionNode, subgraphNodes, graph_);
    /// No loop close edges or DDG loop edges
    BackFilter<Graph> backFilter(graph_);
    /// Create filtered graph
    Subgraph  sub = Subgraph(graph_, backFilter, subgraph);
    /// Sort nodes of subgraph topologically
    std::vector<NodeDescriptor> sorted;
    try {
        boost::topological_sort(sub, std::back_inserter(sorted));
    } catch (boost::not_a_dag & x) {
        /// In case topological sort fails with graph not being DAG
        /// Write it out
        label_writer<Graph> lb(graph_);
        TCEString outName =
            name() + Conversion::toString(wrongCounter_) + "_notDAG.dot";
        wrongCounter_++;
        std::ofstream output(outName.c_str());
        boost::write_graphviz(output, sub,lb,lb);

        Application::logStream() << (boost::format(
            "Topological sort of %s(%s) failed.\n"
            "Most likely loop is present.\n")
            % name() % regionNode->toString()).str();
        return;
    } catch (...) {
       Application::logStream() << (boost::format(
            "Topological sort of %s(%s) failed.\n"
            "Most likely loop is present.\n")
            % name() % regionNode->toString()).str();
        return;
    }

    BasicBlockNode* lastBB = NULL;
    std::vector<BasicBlockNode*> leafBlocks;
    bool changeBB = false;

    for (std::vector<NodeDescriptor>::reverse_iterator iter = sorted.rbegin();
        iter != sorted.rend(); iter++) {
        Node* accessedNode = graph_[*iter];
        BasicBlockNode* bb = NULL;

        if(accessedNode->newFirstBB() != NULL) {
            // Child node is region or predicate and already has BBs
            // we will just link them
            bb = accessedNode->newFirstBB();
            if (regionNode->newFirstBB() == NULL) {
                assert(leafBlocks.size() == 0 && lastBB == NULL);
                /// It is first block so we add it as new first also to parent
                regionNode->setNewFirstBB(bb);
            }
            /// There was some previous node tested, we have to add edges
            if (lastBB != NULL) {
                assert(leafBlocks.size() == 0);
                /// Previously, statements created single basic block
                /// so we add edge from that to this one
                ControlFlowEdge* edge = NULL;
                /// If previous block ended with call we add CALL type of edge
                if (changeBB == true) {
                    changeBB = false;
                    edge = new
                        ControlFlowEdge(ControlFlowEdge::CFLOW_EDGE_NORMAL,
                        ControlFlowEdge::CFLOW_EDGE_CALL);
                } else {
                    edge = new ControlFlowEdge();
                }
/*                Application::logStream() << (boost::format(
                "Adding edge from lastBB %s to %s in %s\n")
                % lastBB->toString() % bb->toString() % name()).str();*/
                newCFG_->connectNodes(*lastBB, *bb, *edge);
                std::cerr << "Adding for lastBB != NULL" << std::endl;
                createJump(lastBB, bb);
                /// clear lastBB, the edge was already added
/*                Application::logStream() << (boost::format(
                "\tChanged lastbb from %s to NULL\n")
                % lastBB->toString()).str();*/
                lastBB = NULL;
                leafBlocks.clear();
            }
/*            Application::logStream() << (boost::format(
            "\tStart adding leaf blocks for %s in region %s with bb %s\n")
            % accessedNode->toString() % regionNode->toString()
            % bb->toString()).str();*/
            addLeafEdges(leafBlocks, bb);
            /// leafs were processed, this node should have some leafs as well
            leafBlocks.clear();
            leafBlocks = accessedNode->leafBlocks();
            /// if we got this far, lastBB should be NULL
            assert(lastBB == NULL);
            /// Accessed node is loop entry, we add edge from corresponding
            /// loop close node to it.
            if (accessedNode->isLoopEntryNode()) {
                processLoopEntry(accessedNode, bb);
            }
        } else {
            /// We found single statement, time to add it to basic block
            if (lastBB == NULL || changeBB == true) {
                /// Time to create new BB
                TTAProgram::BasicBlock* block = 
                    new TTAProgram::BasicBlock(insCount_);
                insCount_++;
                bb = new BasicBlockNode(*block);
/*                Application::logStream() << (boost::format(
                "Create new BB %s in %s for %s\n")
                % bb->toString() % regionNode->toString() %
                    accessedNode->toString()).str();*/
                newCFG_->addNode(*bb);
                if (regionNode->newFirstBB() == NULL) {
                    /// We just created first BB for children of region
                    /// so we set it as first BB
                    regionNode->setNewFirstBB(bb);
                }
                /// Previous BB ended with CALL, so we link it to new one
                /// with call edge
                if (changeBB == true && lastBB != NULL) {
                    changeBB = false;
                    ControlFlowEdge* edge = new ControlFlowEdge(
                        ControlFlowEdge::CFLOW_EDGE_NORMAL,
                        ControlFlowEdge::CFLOW_EDGE_CALL);
                    newCFG_->connectNodes(*lastBB, *bb, *edge);
                    assert(leafBlocks.size() == 0);
                }
                /// Block just created is current lastBB
/*                Application::logStream() << (boost::format(
                "\tChanged lastbb to %s\n")
                %bb->toString()).str();*/
                lastBB = bb;
                }
            }
            if(accessedNode->isMoveNode()
                && accessedNode->moveNode().isMove()) {
                TTAProgram::Instruction* newIns =
                    new TTAProgram::Instruction();
                newIns->addMove(accessedNode->moveNode().movePtr());
                lastBB->basicBlock().add(newIns);
                insCount_++;
                if (accessedNode->moveNode().move().isCall()) {
                    changeBB = true;
                }
                if (leafBlocks.size() > 0) {
/*                    Application::logStream() << (boost::format(
                    "\tStart adding leaf blocks: %s in region %s with bb %s\n")
                    % accessedNode->toString() % regionNode->toString()
                    % bb->toString()).str();*/
                    addLeafEdges(leafBlocks, bb);
                    /// Leaf blocks were added, we can clear them
                    leafBlocks.clear();
            } else {
                if (Application::verboseLevel() > 2) {
                    Application::logStream() <<
                    (boost::format(" Undetected case! %s inside %s\n")
                    % accessedNode->toString() % regionNode->toString()
                    ).str();
                }
            }
        }
    }
    if (leafBlocks.size() > 0) {
        regionNode->addLeafBlocks(leafBlocks);
        leafBlocks.clear();
    }
    if (lastBB != NULL) {
/*        Application::logStream() << (boost::format(
        "Add last bb %s as a leaf to %s\n")
        % lastBB->toString() % regionNode->toString()).str();*/
        regionNode->addLeafBlock(lastBB);
    }
    if(regionNode == entryNode_) {
        processEntry(regionNode->newFirstBB());
    }
    unorderable.clear();
}

/**
 * Computes relation between every pair of nodes in a graph that has
 * common parent.
 *
 * @param orderMap Post order map of a graph
 * @parem filteredCDG Filtered PDG with only control dependence edges
 */
void
ProgramDependenceGraph::computeRelations(
    const PDGOrderMap& orderMap,
    FilteredCDG& filteredCDG) {

    /// Process nodes in post order, guarantees child region/loopEntry/predicate
    /// nodes will be processed before their parent
    int mapSize = orderMap.size();

    cdg_->writeToDotFile(name() + "_originalCDG.dot");

    newCFG_ = new ControlFlowGraph(name(), program_);
    for (int i = 0; i < mapSize; i++) {
        NodeDescriptor des =
            MapTools::keyForValue<NodeDescriptor>(
            orderMap, i);
        Node* node = graph_[des];
        /// MoveNodes are skipped here, they are always children of region
        if (node->isRegionNode()
            || node->isLoopEntryNode()) {
            processRegion(node, filteredCDG);
            continue;
        }
        if (node->isPredicateMoveNode()){
            processPredicate(node, filteredCDG);
            continue;
        }
        if (node->isLoopCloseNode()) {
            processLoopClose(node);
            continue;
        }
    }
    newCFG_->writeToDotFile(name() + "_newCFG.dot");
    writeToDotFile(name() + "_newPDG.dot");

}

/**
 * Helper method to move/copy DDG edges that connected subraph nodes with rest
 * of the graph to the root node of subgraph
 *
 * @param rootNode Root node of subgraph
 * @param subgraphNodes Nodes of the subgraph to test
 * @param filteredCDG Filtered cdg graph, containing only control dependence
nodes
 */
void
ProgramDependenceGraph::moveDDGedges(
    Node& root,
    NodeSet& subgraphNodes,
    FilteredCDG& filteredCDG) {

    for (NodeSet::iterator iter = subgraphNodes.begin();
        iter != subgraphNodes.end();
        iter++) {
        /// Do not process root of a subtree, it is child of other node
        /// and will be processed later
        if ((*iter) == &root) {
            continue;
        }
        /// Test if target is region, predicate or loop node,
        /// those prefer copies
        bool copyInsteadOfMove = false;
        if (!(*iter)->isMoveNode()) {
            /// gets incoming control dependence edges of node
            /// more then one edge means node is reachable from several places
            int parentCount = boost::in_degree(descriptor(**iter), filteredCDG);
            if (parentCount > 1) {
                copyInsteadOfMove = true;
            }
        }
        /// Deal with incoming edges
        EdgeSet inputs = inEdges(**iter);
        for (EdgeSet::iterator ein = inputs.begin();
            ein != inputs.end();
            ein++) {
            /// No need to deal with control dependence edges or data dependence
            /// that had been "fixed" - meaning they are between nodes of same
            /// subtree (including edges between root of subtree and child node)
            if (!(*ein)->isDataDependence() || (*ein)->fixed()) {
                continue;
            }
            Node* tail = &tailNode(**ein);
            if (!AssocTools::containsKey(subgraphNodes, tail)) {
                Edge* testedEdge = *ein;
                EdgeSet inputEdges = connectingEdges(*tail, root);
                bool duplicateEdge = false;
                for (EdgeSet::iterator testIter = inputEdges.begin();
                    testIter != inputEdges.end();
                    testIter ++) {
                    if (!(*testIter)->isDataDependence()) {
                        continue;
                    }
                    if ((testedEdge->dataDependenceEdge().dependenceType() ==
                        (*testIter)->dataDependenceEdge().dependenceType())
                        &&
                        (testedEdge->dataDependenceEdge().edgeReason() ==
                        (*testIter)->dataDependenceEdge().edgeReason())) {
                        /// Edges with identical properties already exists
                        /// no need to copy or move, just remove edge
                        duplicateEdge = true;
                    }
                }
                if (copyInsteadOfMove && !duplicateEdge) {
                    /// Creates copy of edge from outside subgraph
                    /// to root of the graph
                    copyInEdge(root, **ein, tail);
                } else if (duplicateEdge) {
                    /// Edge is duplicate of already existing dependence
                    removeEdge(**ein);
                } else {
                    /// Moves edge from outside the subgraph to target root
                    moveInEdge(**iter, root, **ein);
                }
            } else {
                /// Edge between nodes in subtree
                (*ein)->setFixed();
            }
        }
        /// Deal with outgoing edges
        EdgeSet outputs = outEdges(**iter);
        for (EdgeSet::iterator eout = outputs.begin();
            eout != outputs.end();
            eout++) {
            /// No need to deal with control dependence edges or data dependence
            /// that had been "fixed" - meaning they are between nodes of same
            /// subtree (including edges between root of subtree and child node)
            if (!(*eout)->isDataDependence() || (*eout)->fixed()) {
                continue;
            }
            Node* head = &headNode(**eout);
            if (!AssocTools::containsKey(subgraphNodes, head)) {
                Edge* testedEdge = *eout;
                EdgeSet outputEdges = connectingEdges(root, *head);
                bool duplicateEdge = false;
                for (EdgeSet::iterator testIter = outputEdges.begin();
                    testIter != outputEdges.end();
                    testIter ++) {
                    if (!(*testIter)->isDataDependence()) {
                        continue;
                    }
                    if ((testedEdge->dataDependenceEdge().dependenceType() ==
                        (*testIter)->dataDependenceEdge().dependenceType())
                        &&
                        (testedEdge->dataDependenceEdge().edgeReason() ==
                        (*testIter)->dataDependenceEdge().edgeReason())) {
                        /// Edges with identical properties already exists
                        /// no need to copy or move, just remove edge
                        duplicateEdge = true;
                    }
                }
                if (copyInsteadOfMove && !duplicateEdge) {
                    /// Creates copy of edge from outside subgraph
                    /// to root of the graph if such edge does not exists
                    copyOutEdge(root, **eout, head);
                } else if (duplicateEdge) {
                    /// Edge is duplicate of already existing dependence
                    removeEdge(**eout);
                } else {
                    /// Moves edge from outside the subgraph to target root
                    moveOutEdge(**iter, root, **eout);
                }
            } else {
                /// Edge between nodes in subtree
                (*eout)->setFixed();
            }
        }
    }
}

/**
 * Copies Region and EEC information from CDG nodes to PDG. Only used if
 * "preprocessing" is done via CDG analysis. Copies also component members.
 *
 * @param cDNodeToPNode Map between CDG nodes and PDG nodes
 * @param bBlockToCDNode Map between basic blocks and CDG nodes
 * @param moveToPNode Map between move nodes and their PDG equivalents
 * @param moveToCD Map between CDG node and all PDG equivalents of it's content
 */
void
ProgramDependenceGraph::copyRegionEECComponent(
    ControlToProgram& cDNodeToPNode,
    BBToCD& bBlockToCDNode,
    MoveNodeToPDGNode& /*moveToPNode*/,
    MovesInCD& moveToCD) {

    int componentCount = cdg_->componentCount();
    strongComponents_.resize(componentCount);

    for (int i = 0; i < nodeCount(); i++) {
        ProgramDependenceNode* node = graph_[i];
        ControlDependenceNode* cNode = NULL;

        /// Find source of region and eec. In case node is move or predicate
        /// have to find parent CDG node
        if (node == entryNode_) {
            /// No region or eec info in entry
            continue;
        }
        if (node->isRegionNode()
            || node->isLoopEntryNode()
            || node->isLoopCloseNode()) {
            /// For non basic block/predicate nodes, we copied them so
            /// they will also be in 'region' and 'eec'.
            /// Set component info for nodes that are part of loop as well
            cNode = &node->cdgNode();
            if (cNode->component() != -1) {
                node->setComponent(cNode->component());
                strongComponents_[node->component()].insert(node);
            }
        } else {
            const BasicBlockNode* BB =
                &ddg_->getBasicBlockNode(node->moveNode());
            if (MapTools::containsKey(bBlockToCDNode, BB)) {
                cNode =
                    MapTools::valueForKey<ControlDependenceNode*>(
                        bBlockToCDNode, BB);
            } else {
                throw InvalidData(__FILE__, __LINE__, __func__,
                    "No CD node for basic block!" + BB->toString());
            }
            /// If cdg node is basic block or predicate, all moves inside
            /// will be part of same component
            if (cNode->component() != -1) {
                std::vector<Node*> nodesInBB = moveToCD[cNode];
                int currentComponent = cNode->component();
                for (unsigned int i = 0; i < nodesInBB.size(); i++) {
                    strongComponents_[currentComponent].insert(
                        nodesInBB[i]);
                    nodesInBB[i]->setComponent(currentComponent);
                }
            }
        }

        ControlDependenceNode::NodesInfo rInfo = cNode->region();
        for (ControlDependenceNode::NodesInfo::iterator iter = rInfo.begin();
            iter != rInfo.end();
            iter++) {
            /// If destination in 'region' is CDG node, add it
            if ((*iter)->isRegionNode()
                || (*iter)->isLoopCloseNode()
                || (*iter)->isLoopEntryNode()) {
                Node* target = cDNodeToPNode[*iter];
                node->addToRegion(*target);
            } else {
                /// If destination in 'region' is BB, add all the moves in it
                std::vector<Node*> nodesInBB = moveToCD[*iter];
                for (unsigned int i = 0; i < nodesInBB.size(); i++) {
                    node->addToRegion(*nodesInBB[i]);
                }
            }
        }
        ControlDependenceNode::NodesInfo eecInfo;
        if (cNode->isPredicateNode() && !node->isPredicateMoveNode()) {
            /// We have node which was in predicate basic block but is not
            /// the actual predicate move node.
            /// We have to copy "shadow" eec information
            eecInfo = cNode->pseudoPredicateEEC();
        } else {
            /// any other type of node should have same eec information as it's
            /// CDG counterpart or basic block it belonged to
            eecInfo = cNode->eec();
            /// If node was in predicate basic block and is predicate we also
            /// test if it is lastNode and set info
            if (cNode->isPredicateNode()
                && node->isPredicateMoveNode()
                && cNode->isLastNode()) {
                node->setLastNode();
            }
        }
        for (ControlDependenceNode::NodesInfo::iterator iter =
            eecInfo.begin(); iter != eecInfo.end(); iter++) {
            if ((*iter)->isRegionNode()
                || (*iter)->isLoopCloseNode()
                || (*iter)->isLoopEntryNode()) {
                Node* target = cDNodeToPNode[*iter];
                node->addToEEC(*target);
            } else {
                std::vector<Node*> nodesInBB = moveToCD[*iter];
                for (unsigned int i = 0; i < nodesInBB.size(); i++) {
                    node->addToEEC(*nodesInBB[i]);
                }
            }
        }
    }
}

/**
 * When entry node is found during serialization, the artificial Entry and Exit
 * nodes are added to CFG. There is added edge from Entry to first BB and
 * All leaf nodes will have edge to the exit.
 */
void
ProgramDependenceGraph::processEntry(BasicBlockNode* firstBB){
    BasicBlockNode* newEntry = new BasicBlockNode(0, 0, true, false);
    newCFG_->addNode(*newEntry);
    ControlFlowEdge* edge = new ControlFlowEdge();
    newCFG_->connectNodes(*newEntry, *firstBB, *edge);
    newCFG_->addExit();
//     Application::logStream() << (boost::format(
//     "Create new Entry %s for %s\n")
//     % newEntry->toString() % firstBB->toString()).str();
}

/**
 * Process predicate node of the graph.
 *
 * Add edges from predicate to one or two child region nodes and fill in
 * next basic block with last BB of first child region and fall through
 * BB with last BB of second child (if exists), otherwise predicate BB
 * itself. Also moves DDG edges between child nodes and out of subtree
 * to point to predicate for topological sorting of region where predicate
 * itself belongs.
 * @param predicate Predicate node to process
 * @param filteredCDG Filtered graph from where we get child nodes of predicate
 */
void
ProgramDependenceGraph::processPredicate(
    Node* predicate,
    FilteredCDG& filteredCDG) {

    /// Get all child nodes of predicate node
    NodeDescriptor nodeDes = descriptor(*predicate);
    FilteredOutEdgePair edges = boost::out_edges(nodeDes, filteredCDG);
    /// Store all child nodes, we will need to collect all edges
    /// to and from subgraph rooted by predicate
    NodeSet subgraphNodes;
    subgraphNodes.insert(predicate);
    /// Create basic block with predicate, it will be also first to
    /// execute
    TTAProgram::BasicBlock* block = new TTAProgram::BasicBlock(insCount_);
    BasicBlockNode* bb = new BasicBlockNode(*block);
//     Application::logStream() << (boost::format(
//     "Create new Predicate BB %s for %s\n")
//     % bb->toString() % predicate->toString()).str();

    newCFG_->addNode(*bb);
    TTAProgram::Instruction* newIns = new TTAProgram::Instruction();
    newIns->addMove(predicate->moveNode().movePtr());
    TTAProgram::Terminal& guardReg =
        predicate->moveNode().move().destination();

    bb->basicBlock().add(newIns);
    insCount_++;
    predicate->setNewFirstBB(bb);

    bool hasTrue = false;
    bool hasFalse = false;
    for (FilteredOutEdgeIter ei1 = edges.first;
        ei1 != edges.second;
        ++ei1) {
        Edge* edge = graph_[*ei1];
        if (edge->isArtificialControlDependence()) {
            continue;
        }
        NodeDescriptor des = boost::target(*ei1, filteredCDG);
        Node* node = graph_[des];
        subgraphNodes.insert(node);

        /// In case one of branches had only single
        /// absolute jump to "exit" of procedure, there
        /// is no child basic block
        if(node->newFirstBB() != NULL) {
            ControlFlowEdge::CFGEdgePredicate predicateValue =
                ControlFlowEdge::CFLOW_EDGE_NORMAL;
            if (edge->controlDependenceEdge().isTrueEdge()) {
                predicateValue = ControlFlowEdge::CFLOW_EDGE_TRUE;
                hasTrue = true;
            } else {
                predicateValue = ControlFlowEdge::CFLOW_EDGE_FALSE;
                hasFalse = true;
            }
            ControlFlowEdge *newEdge = new ControlFlowEdge(predicateValue);
/*            Application::logStream() << (boost::format(
            "Adding edge for predicate %s to %s in %s\n")
            % bb->toString() % node->newFirstBB()->toString() % name()).str();*/
            newCFG_->connectNodes(
                *bb, *node->newFirstBB(), *newEdge);
            std::cerr << "Adding for predicate" << std::endl;
            createJump(bb, node->newFirstBB(), &guardReg, predicateValue);
            //predicate->addLeafBlocks(node->leafBlocks());
        } else {
            if(boost::out_degree(des, filteredCDG) != 0) {
                TCEString msg = (boost::format(
                    "Found a node without first BB set. %s for predicate %s"
                    " in procedure %s\n")
                    % node->toString() % predicate->toString() % name()).str();
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
        }
        if (node->isLoopCloseNode()) {
            assert(node->newFirstBB() != NULL);
            //predicate->setLastNode();
        }
        if (node->isLoopEntryNode()) {
            processLoopEntry(node, node->newFirstBB());
        }
    }
    /// One of the outgoing edges is missing, we will have to add edge
    /// from predicate itself to next BB
    if (!(hasTrue && hasFalse)) {
        predicate->addLeafBlock(bb);
    }
    /// Move/copy edges between nodes "inside" subgraph and nodes outside the
    /// subgraph
    moveDDGedges(*predicate, subgraphNodes, filteredCDG);
}

/**
 * Add edges from 'leaf' blocks of a subgraph to the basic block
 *
 * @param leafBlocks vector of basic blocks to add
 * @param bb Basic block to which the edges will point to
 */
void
ProgramDependenceGraph::addLeafEdges(
    std::vector<BasicBlockNode*> leafBlocks,
    BasicBlockNode* bb) {
    for (unsigned int i = 0; i < leafBlocks.size(); i++) {
        /// One previously tested was predicate or region
        /// we have to add edges from all of their leafs to this block
        ControlFlowGraph::EdgeSet edges =
            newCFG_->outEdges(*leafBlocks[i]);
        ControlFlowEdge::CFGEdgePredicate predicate =
            ControlFlowEdge::CFLOW_EDGE_NORMAL;
        /// Predicate had only one outgoing edge, we add second with
        /// inverse value
        for(ControlFlowGraph::EdgeSet::iterator cfe = edges.begin();
            cfe != edges.end(); cfe++) {
            if ((*cfe)->isTrueEdge()) {
                predicate = ControlFlowEdge::CFLOW_EDGE_FALSE;
                break;
            }
            if ((*cfe)->isFalseEdge()) {
                predicate = ControlFlowEdge::CFLOW_EDGE_TRUE;
                break;
            }
        }
        if (newCFG_->connectingEdges(*leafBlocks[i], *bb).size() == 0) {
            ControlFlowEdge* edge = new ControlFlowEdge(predicate);
            Application::logStream() << (boost::format(
            "Adding leaf edge from %s to %s in %s\n")
            % leafBlocks[i]->toString() % bb->toString() % name()).str();
            newCFG_->connectNodes(*leafBlocks[i], *bb, *edge);
            std::cerr << "Adding for leaf edges" << std::endl;
            createJump(leafBlocks[i], bb);
        }
    }
    leafBlocks.clear();
}

void
ProgramDependenceGraph::processLoopClose(Node* node) {
    TTAProgram::BasicBlock* block = new TTAProgram::BasicBlock(insCount_);
    BasicBlockNode* bb = new BasicBlockNode(*block);
    newCFG_->addNode(*bb);
    TTAProgram::Instruction* newIns = new TTAProgram::Instruction();
    /// Add empty instruction, actual jump move will be created when
    /// we process corresponding loop entry
    bb->basicBlock().add(newIns);
    insCount_++;
    node->setNewFirstBB(bb);
    /*NodeSet predecessorsNodes = predecessors(*node);
    for (NodeSet::iterator iter = predecessorsNodes.begin();
        iter != predecessorsNodes.end(); iter++) {
        (*iter)->setLastNode();
    }*/
/*    Application::logStream() << (boost::format(
        "Creating BB %s for loop close %s")
        % bb->toString() % node->toString()).str();*/
}

void
ProgramDependenceGraph::processLoopEntry(Node* node, BasicBlockNode* bb) {
    /// Add edge from loop close node to corresponding loop entry
    if (bb == NULL) {
        return;
    }
    NodeSet inputs = predecessors(*node);
    for(NodeSet::iterator ni = inputs.begin(); ni != inputs.end();
        ni++) {
        if((*ni)->isLoopCloseNode()
            && (*ni)->newFirstBB() != NULL
            && (*ni)->component() == node->component()) {
/*            Application::logStream() << "Adding loop edge "
            << (*ni)->newFirstBB()->toString() << " to "
            << bb->toString() << std::endl;*/
            ControlFlowEdge* edge = new ControlFlowEdge();
            edge->setBackEdge();
            newCFG_->connectNodes(*(*ni)->newFirstBB(), *bb, *edge);
            std::cerr << "Adding for loop entry" << std::endl;
            createJump((*ni)->newFirstBB(), bb);
        }
    }
}

void
ProgramDependenceGraph::disassemble() const {
#if 0
    BackCFGFilter<ControlFlowGraph> backCFGFilter(*newCFG_);
    /// Create filtered graph
    CFGSubgraph  sub = CFGSubgraph(*newCFG_, backCFGFilter);
    /// Sort nodes of subgraph topologically
    std::vector<ControlFlowGraph::NodeDescriptor> sorted;
    try {
        boost::topological_sort(*newCFG_, std::back_inserter(sorted));
    } catch (...) {
        Application::logStream() << (boost::format(
            "CFG %s can not be topologically sorted\n")
            % name()).str();
    }
    for (std::vector<ControlFlowGraph::NodeDescriptor>::reverse_iterator iter =
        sorted.rbegin();
        iter != sorted.rend(); iter++) {
        BasicBlockNode* node = newCFG_[*iter];
        if (node->isNormalBB()) {
            Application::logStream() <<
                POMDIsassembler::disassemble(node->basicBlock());
        }
    }

    Application::logStream() << (boost::format(
        "Trying out %s with node count %d\n")
        % name() % newCFG_->nodeCount()).str();
    TTAMachine::AddressSpace& space =
        cdg_->program()->universalMachine().instructionAddressSpace();
    TTAProgram::Procedure newProc(name(), space, 0);
    Application::logStream() << "Is in program " << newProc.isInProgram() <<
            std::endl;
    cdg_->program()->addProcedure(&newProc);
    newCFG_->copyToProcedure(newProc);
    //newCFG_->updateReferencesFromProcToCfg();
    Application::logStream() << "Procedure copied" <<
            std::endl;
    Application::logStream() << " start " << newProc.startAddress().location()
            << " end " << newProc.endAddress().location() << std::endl;
    //Application::logStream() <<
      //  POMDisassembler::disassemble(newProc);
    cdg_->program()->removeProcedure(newProc);
    int count = newProc.instructionCount();
    for (int i = 0; i < count; i++) {
        Application::logStream() << (boost::format(
        "%s\n")
        % POMDisassembler::disassemble(newProc.instructionAtIndex(i),1)).str();
    }
    //ControlFlowGraph testCFG(newProc);
    //testCFG.writeToDotFile(name() + "_testCFG.dot");
    //delete newCFG_;
#endif
}

void
ProgramDependenceGraph::createJump(
    BasicBlockNode* from,
    BasicBlockNode* to,
    TTAProgram::Terminal* guardReg,
    ControlFlowEdge::CFGEdgePredicate predicate){
    return;
    if (from->isNormalBB() && to->isNormalBB()
       //&& !from->basicBlock().lastInstruction().hasControlFlowMove()
       ) {
        if (from->basicBlock().lastInstruction().hasControlFlowMove()) {
            Application::logStream() << "\tMove already has CF " <<
                from->basicBlock().lastInstruction().toString() << std::endl;
        }
        Application::logStream() << (boost::format(
            "Creating jump from %s to %s\n")
            % from->toString() % to->toString()).str();
        TTAProgram::InstructionReferenceManager& manager =
            program_->instructionReferenceManager();
        /// If reference already exists, just return it
        TTAProgram::InstructionReference ref =
            manager.createReference(to->basicBlock().firstInstruction());

        TTAMachine::HWOperation* hwOp =
            program_->universalMachine().controlUnit()->operation("jump");

        TTAProgram::TerminalFUPort* jump1Terminal =
            new TTAProgram::TerminalFUPort(*hwOp, 1);

        TTAProgram::Terminal* jump0Terminal =
            new TTAProgram::TerminalInstructionReference(ref);

        auto jumpMove = std::make_shared<TTAProgram::Move>(
                    jump0Terminal, jump1Terminal,
                    program_->universalMachine().universalBus());
    
        if (predicate == ControlFlowEdge::CFLOW_EDGE_TRUE) {
            TTAMachine::Machine::BusNavigator busNav =
                program_->universalMachine().busNavigator();
            for (int i = 0; i < busNav.count(); i++) {
                TTAMachine::Bus* bus = busNav.item(i);
                for (int i = 0; i < bus->guardCount(); i++) {
                    TTAMachine::RegisterGuard* regGuard =
                        dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(i));
                    if (regGuard != NULL &&
                        regGuard->registerFile() == &guardReg->registerFile() &&
                        regGuard->registerIndex() == (int)guardReg->index()) {
                        jumpMove->setGuard(
                            new TTAProgram::MoveGuard(*regGuard));
                        break;
                    }
                }
            }
        } else if (predicate == ControlFlowEdge::CFLOW_EDGE_FALSE) {
            TTAMachine::Machine::BusNavigator busNav =
                program_->universalMachine().busNavigator();
            for (int i = 0; i < busNav.count(); i++) {
                TTAMachine::Bus* bus = busNav.item(i);
                for (int i = 0; i < bus->guardCount(); i++) {
                    TTAMachine::RegisterGuard* regGuard =
                        dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(i));
                    if (regGuard != NULL &&
                        regGuard->registerFile() == &guardReg->registerFile() &&
                        regGuard->registerIndex() == (int)guardReg->index() &&
                        regGuard->isInverted() == true) {
                        jumpMove->setGuard(
                            new TTAProgram::MoveGuard(*regGuard));
                        break;
                    }
                }
            }
        }

        TTAProgram::Instruction* inst = &from->basicBlock().lastInstruction();
        if (inst->moveCount() > 0) {
            inst = new TTAProgram::Instruction();
            from->basicBlock().add(inst);
        }
        std::cerr << " before result " << inst->toString() << std::endl;
        inst->addMove(jumpMove);
        std::cerr << " after result " << inst->toString() << std::endl;
    } else {
        Application::logStream() << (boost::format(
            "Failed to add jump for %s %s, %s, %s\n")
            % from->toString() % to->toString() %
            from->basicBlock().lastInstruction().toString() %
            to->basicBlock().firstInstruction().toString()).str();
    }
}
