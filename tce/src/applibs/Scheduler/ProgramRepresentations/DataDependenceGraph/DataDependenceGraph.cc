/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file DataGraph.cc
 *
 * Implementation of data dependence graph class
 *
 * @author Heikki Kultala 2006-2008 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "StringTools.hh"
#include "AssocTools.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "ProgramOperation.hh"
#include "Application.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "HWOperation.hh"
#include "CodeSnippet.hh"
#include "Instruction.hh"
#include "POMDisassembler.hh"
#include "BasicBlockNode.hh"
#include "TCEString.hh"
#include "Guard.hh"

/**
 * Sets bookkeeping that the given movende belongs to the given basic block.
 * 
 * @param mn MoveNode given
 * @param bblock Basic Block node where the move node belongs
 * @param modifier modifier graph on the subgraph tree
 */
void 
DataDependenceGraph::setNodeBB(
    MoveNode& mn, BasicBlockNode& bblock, DataDependenceGraph* modifier) {
    moveNodeBlocks_[&mn] = &bblock;
    
    if (parentGraph_ != NULL && parentGraph_ != modifier) {
        static_cast<DataDependenceGraph*>(parentGraph_)->
            setNodeBB(mn, bblock, this);
    }

    for (unsigned int i = 0; i < childGraphs_.size(); i++) {
        if ( childGraphs_[i] != modifier ) {
            static_cast<DataDependenceGraph*>(childGraphs_[i])->
                setNodeBB(mn,bblock,this);
        }
    }
}

/**
 * Adds a node into the graph.
 *
 * This method should not be called by the user, used internally
 * 
 * @param moveNode moveNode being added.
 */
void
DataDependenceGraph::addNode(MoveNode& moveNode) throw (ObjectAlreadyExists) {
    BoostGraph<MoveNode, DataDependenceEdge>::addNode(moveNode);
    if (moveNode.isMove()) {
        nodesOfMoves_[&moveNode.move()] = &moveNode;
    }
}


/**
 * Adds a node into the graph.
 * 
 * @param moveNode moveNode being added.
 * @param bblock Basic block where the move logically belongs.
 */
void
DataDependenceGraph::addNode(MoveNode& moveNode, BasicBlockNode& bblock) {
    addNode(moveNode);
    setNodeBB(moveNode, bblock, NULL);
}

/**
 * Adds a node into the graph.
 * 
 * @param moveNode moveNode being added.
 * @param relatedNode another node already existing in the graph
 * where this movenode relates to. , for example is created by
 * splitting that 
 */
void
DataDependenceGraph::addNode(MoveNode& moveNode, MoveNode& relatedNode) {
    addNode(moveNode);
    setNodeBB(moveNode, getBasicBlockNode(relatedNode), NULL);
    ///  @todo: also add to subgrapsh which have the related node?
}


/**
 * Constructor.
 *
 * @param name The graph can be named for debugging purposes.
 * @param containsProcedure whether the DDG contains complete procedure
 */
DataDependenceGraph::DataDependenceGraph(
    const std::string& name, bool containsProcedure) : 
    BoostGraph<MoveNode, DataDependenceEdge>(name), cycleGrouping_(true), 
    machine_(NULL), delaySlots_(0), procedureDDG_(containsProcedure) {
}

/**
 * Destructor.
 *
 * Deletes all MoveNodes and ProgramOperations.
 */
DataDependenceGraph::~DataDependenceGraph() {

    // only delete removed node data, not the node pointers themselves
    // they should already have been deleted by the one who removed them
    AssocTools::deleteAllValues(removedNodes_);

    if (parentGraph_ == NULL) {

        //delete nodes.
        int nc = nodeCount();
        for (int i = 0; i < nc; i++) {
            delete &node(i);
        }

        // delete program operations.
        for (POLIter i = programOperations_.begin();
             i != programOperations_.end(); i++) {
            delete *i;
        }
    }
}

/**
 * Gives the basic block node where the node belongs to.
 *
 * @param mn MoveNode whose basic block we are asking
 * @return BasicBlockNode of the move
 */
BasicBlockNode&
DataDependenceGraph::getBasicBlockNode(MoveNode& mn) {
    return *moveNodeBlocks_[&mn];
}

ProgramOperation& 
DataDependenceGraph::programOperation(int index) {
    return *programOperations_.at(index);
}

int 
DataDependenceGraph::programOperationCount() {
    return programOperations_.size();
}

DataDependenceEdge* 
DataDependenceGraph::onlyRegisterEdgeIn(MoveNode& mn) {
    
    DataDependenceEdge* result = NULL;
    for (int i = 0; i < inDegree(mn); i++) {
        DataDependenceEdge &edge = inEdge(mn, i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {
            if (result == NULL) {
                result = &edge;
            } else {
                return NULL;
            }
        }
    }
    return result;
}

DataDependenceEdge* 
DataDependenceGraph::onlyRegisterEdgeOut(MoveNode& mn) {

    DataDependenceEdge* result = NULL;
    for (int i = 0; i < outDegree(mn); i++) {
        DataDependenceEdge &edge = outEdge(mn, i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {
            if (result == NULL) {
                result = &edge;
            } else {
                return NULL;
            }
        }
    }
    return result;
}

/**
 * Adds a program operation to the graph, and it's parent graphs.
 *
 * The graph then owns this programOperation.
 *
 * @param po ProgramOperation being added.
 */
void 
DataDependenceGraph::addProgramOperation(ProgramOperation* po) {
    programOperations_.push_back(po);
    if (parentGraph_ != NULL) {
        dynamic_cast<DataDependenceGraph*>(parentGraph_)
            ->addProgramOperation(po);
    }
}

/**
 * Returns the earliest cycle this move can be scheduled at given the
 * cycles of the dependencies.
 *
 * Checks all the parent nodes of the move in the DDG and finds their max
 * cycle if they are scheduled, if none found, 0 is returned, if at least one
 * of the preceeding nodes is unscheduled, returns INT_MAX.
 *
 * @param moveNode The move node for which to find the earliest cycle.
 * @return The earliest cycle the move can be scheduled to according to
 * data dependencies, INT_MAX if unknown.
 */
int
DataDependenceGraph::earliestCycle(const MoveNode& moveNode) const {

    const EdgeSet edges = inEdges(moveNode);
    int minCycle = 0;
    for (EdgeSet::const_iterator i = edges.begin(); 
         i != edges.end(); ++i) {
        DataDependenceEdge& edge = **i;
        MoveNode& tail = tailNode(edge);

        /// @todo Consider the latency for result read move!
        if (tail.isScheduled()) {
            int latency = 1;
            int effTailCycle = tail.cycle();
            
            // If call, make sure all incoming deps fit into delay slots,
            // can still be later than the call itself
            // dependence type does not matter.
            if (edge.headPseudo()) {
                effTailCycle -= delaySlots_;
            } else {
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
                    
                    // latency does not matter with WAW. always +1.
                    effTailCycle += 1;
                } else {
                    if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                        // WAR allows writing at same cycle than reading.
                        // in WAR also the latency goes backwards, 
                        // new value can
                        // be written before old is read is latency is big.
                        if (edge.guardUse()) {
                            latency = tail.guardLatency();
                        } 
                        effTailCycle = effTailCycle - latency + 1;
                    } else {
                        // RAW
                        if (edge.guardUse()) {
                            latency = moveNode.guardLatency();
                        }
                        // in case of RAW, we have to wait latency cycles 
                        // before we can use the written value. 
                        effTailCycle += latency;
                    }
                }
            }
            minCycle = std::max(effTailCycle, minCycle);
        } else {
            return INT_MAX;
        }
    }
    return minCycle;
}

/**
 * Returns the latest cycle this move can be scheduled at given the
 * cycles of the dependencies.
 *
 * This is assumed to be used with bottom-up scheduling.
 * Checks all successor nodes and checks their min cycle if they are scheduled.
 * If none found, INT_MAX is returned, if at least one of successors is
 * unscheduled, returns 0.
 *
 * @param moveNode The move node for which to find the latest cycle.
 * @return The latest cycle the move can be scheduled to according to
 * data dependencies, 0 if unknown.
 */
int
DataDependenceGraph::latestCycle(const MoveNode& moveNode) const {
    
    const EdgeSet edges = outEdges(moveNode);
    int maxCycle = INT_MAX;
    for (EdgeSet::const_iterator i = edges.begin(); 
         i != edges.end(); ++i) {
        DataDependenceEdge& edge = **i;
        MoveNode& head = headNode(edge);
        
        /// @todo Consider the latency for result read move!
        if (head.isScheduled()) {
            int latency = 1;
            int effHeadCycle = head.cycle();
            
            // If call, make sure all incoming deps fit into delay slots,
            // can still be later than the call itself
            // dependence type does not matter.
            if (edge.tailPseudo()) {
                effHeadCycle += delaySlots_;
            } else {
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
                    
                    // latency does not matter with WAW. always +1.
                    effHeadCycle -= 1;
                } else {
                    if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                        // WAR allows writing at same cycle than reading.
                        // in WAR also the latency goes backwards, 
                        // new value can
                        // be written before old is read is latency is big.
                        if (edge.guardUse()) {
                            latency = moveNode.guardLatency();
                        } 
                        effHeadCycle = effHeadCycle + latency - 1;
                    } else {
                        // RAW
                        if (edge.guardUse()) {
                            latency = moveNode.guardLatency();
                        }
                        // in case of RAW, value must be written latency
                        // cycles before it is used
                        effHeadCycle -= latency;
                    }
                }
            }
            maxCycle = std::min(effHeadCycle, maxCycle);
        } else {
            return 0;
        }
    }
    return maxCycle;
}

/**
 * Returns the moves that are scheduled at the given cycle.
 *
 * @param cycle The cycle.
 * @return Move that are scheduled at the given cycle.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::movesAtCycle(int cycle) const {

    NodeSet moves;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isScheduled() && n.cycle() == cycle)
            moves.insert(&n);
    }

    return moves;
}

/**
 * Returns the MoveNode that defines (writes the value of) the guard
 * the given move node is predicated with.
 *
 * @param moveNode The move node of which guard defining move to find.
 * @return The MoveNode that produces the guard value. 
 * If not found, returns NULL
 */
MoveNode*
DataDependenceGraph::guardDefMove(MoveNode& moveNode) {

    const int inEdges = inDegree(moveNode);
    for (int i = 0; i < inEdges; i++) {
        DataDependenceEdge &edge = inEdge(moveNode, i);
        if (edge.guardUse()) {
            return &tailNode(edge);
        }
    }
/*
    writeToDotFile("broken_ddg.dot");
    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, 
        "Cannot find the MoveNode which defines the guard for " +
        moveNode.toString() + ".");
*/
    return NULL;
}

/**
 * Returns the MoveNode with highest cycle that reads the given register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The MoveNode, NULL, if not found.
 */
MoveNode*
DataDependenceGraph::lastScheduledRegisterRead(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int lastCycle = -1;
    MoveNode* lastFound = NULL;
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& source = n.move().source();
        if (!n.isScheduled() || 
            !(source.isImmediateRegister() || source.isGPR())) 
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
        if (source.isImmediateRegister())
            currentRF = &source.immediateUnit();
        else
            currentRF = &source.registerFile();

        if (&rf == currentRF && 
            source.index() == registerIndex &&
            n.cycle() > lastCycle) {
            lastCycle = n.cycle();
            lastFound = &n;
        }
    }
    return lastFound;
}

/**
 * Returns all unscheduled moves.
 *
 * @param cycle The cycle.
 * @return Unscheduled moves.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::unscheduledMoves() const {

    NodeSet moves;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (!n.isScheduled())
            moves.insert(&n);
    }

    return moves;
}

/**
 * Checks that the DDG is sane.
 *
 * Goes through all edges in the DDG and ensures they make sense, for example,
 * in case of R_RAW, the head should really read the register written by 
 * the tail.
 *
 * @exception In case the graph contains failures. Exception message contains
 *            the reason.
 */
void
DataDependenceGraph::sanityCheck() const 
    throw (Exception) {
    
    for (int i = 0; i < edgeCount(); ++i) {
        DataDependenceEdge& e = edge(i);
        MoveNode& tail = tailNode(e);
        const TTAProgram::Terminal& tailSource = tail.move().source();
        const TTAProgram::Terminal& tailDestination = tail.move().destination();

        MoveNode& head = headNode(e);
        const TTAProgram::Terminal& headSource = head.move().source();
        const TTAProgram::Terminal& headDestination = head.move().destination();

        switch (e.dependenceType()) {
        case DataDependenceEdge::DEP_UNKNOWN:
            if (tailDestination.isFUPort() && headSource.isFUPort())
                break; // operation dependency is marked with DEP_UNKNOWN
            throw Exception(
                __FILE__, __LINE__, __func__,
                ((boost::format(
                      "DEP_UNKNOWN in edge between %s and %s."))
                 % tail.toString() % head.toString()).str());
            break;
        case DataDependenceEdge::DEP_RAW:
            // the normal case
            if (tailDestination.equals(headSource))
                break;

            // memory RAW
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                tailDestination.hintOperation().writesMemory() &&
                headDestination.isFUPort() &&
                headDestination.hintOperation().readsMemory())
                break;

            // W:gcu.ra R:ra
            if (tailDestination.isFUPort() &&
                &tailDestination.functionUnit() == 
                tailDestination.functionUnit().machine()->controlUnit() && 
                headSource.isFUPort())
                break;

            // tail writes a reg the head is guarded with 
            if (!head.move().isUnconditional() && tailDestination.isGPR() && 
                dynamic_cast<TTAMachine::RegisterGuard*>(
                    &head.move().guard().guard()) != NULL) {
                TTAMachine::RegisterGuard& g = 
                    dynamic_cast<TTAMachine::RegisterGuard&>(
                        head.move().guard().guard());
                if (g.registerFile() == &tailDestination.registerFile())
                    break; // TODO: check also the register index
            }

            // return value register on procedure return
            if (tailDestination.isGPR() &&
                headDestination.isFUPort() && 
                head.move().isControlFlowMove())
                break;

            writeToDotFile("faulty_raw_ddg.dot");
            throw Exception(
                __FILE__, __LINE__, __func__,
                ((boost::format(
                      "DEP_RAW in edge between %s and %s."))
                 % tail.toString() % head.toString()).str());
            break;
        case DataDependenceEdge::DEP_WAR:
            if (headDestination.equals(tailSource))
                break;

            // memory WAR
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                tailDestination.hintOperation().readsMemory() &&
                headDestination.isFUPort() &&
                headDestination.hintOperation().writesMemory())
                break;
            
            // call parameter register
            if (tailDestination.isFUPort() &&
                tailSource.isGPR() &&
                head.move().isCall())
                break;

            // return value register has WaR to 'call'
            // no better heuristics yet as we don't know the register RV is
            // mapped to
            if (tailSource.isGPR() && head.move().isCall())
                break; 

            // a use (probably a store) of RA has a WaR to 'call'
            if (tailSource.isFUPort() && head.move().isCall())
                break;

            writeToDotFile("faulty_war_ddg.dot");
            throw Exception(
                __FILE__, __LINE__, __func__,
                ((boost::format(
                      "DEP_WAR in edge between %s and %s."))
                 % tail.toString() % head.toString()).str());
            break;
        case DataDependenceEdge::DEP_WAW:
            if (headDestination.equals(tailDestination))
                break;

            // memory WAW
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                tailDestination.hintOperation().writesMemory() &&
                headDestination.isFUPort() &&
                headDestination.hintOperation().writesMemory())
                break;

            // function parameter registers
            if (tailDestination.isGPR() &&
                headDestination.isFUPort() && 
                head.move().isCall())
                break;

            writeToDotFile("faulty_waw_ddg.dot");
            throw Exception(
                __FILE__, __LINE__, __func__,
                ((boost::format(
                      "DEP_WAW in edge between %s and %s."))
                 % tail.toString() % head.toString()).str());
            break;
        default:
            throw Exception(
                __FILE__, __LINE__, __func__,
                ((boost::format(
                      "Unknown edge type: %d in edge between %s and %s."))
                 % e.dependenceType() % tail.toString() % head.toString()).
                str());
        }
    }
}

/**
 * Returns the graph as a string formatted in GraphViz Dot format.
 *
 * This version is able to order the nodes according to their cycles to
 * make the output more readable.
 *
 * @return Graph represented as a Dot string.
 */
std::string
DataDependenceGraph::dotString() const {

    // TODO group based on both BB and cycle
    std::ostringstream s;
    s << "digraph G {" << std::endl;
    
    if (cycleGrouping_&& !procedureDDG_) {
        // print the "time line"
        s << "\t{" << std::endl
          << "\t\tnode [shape=plaintext];" << std::endl
          << "\t\t";
        const int smallest = smallestCycle();
        const int largest = largestCycle();
        for (int c = smallest; c <= largest; ++c) {
            s << "\"cycle " << c << "\" -> ";
        }
        s << "\"cycle " << largest + 1 << "\"; " 
          << std::endl << "\t}" << std::endl;
    
        // print the nodes that have cycles
        for (int c = smallest; c <= largest; ++c) {
            NodeSet moves = movesAtCycle(c);
            if (moves.size() > 0) {
                s << "\t{ rank = same; \"cycle " << c << "\"; ";
                for (NodeSet::iterator i = moves.begin(); 
                     i != moves.end(); ++i) {
                    Node& n = **i;        
                    s << "n" << n.nodeID() << "; ";
                }
                s << "}" << std::endl;
            }        
        }
    }

    // print all the nodes and their properties
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        s << "\tn" << n.nodeID() << " [" 
          << n.dotString() << "]; " 
          << std::endl;
    }

    // edges
    for (int i = 0, count = edgeCount(); i < count; ++i) {
        Edge& e = edge(i);
        Node& tail = tailNode(e);
        Node& head = headNode(e);

        s << "\tn" << tail.nodeID() 
          << " -> n" << head.nodeID() << "[" 
          << e.dotString() << "];" << std::endl;    }
    s << "}" << std::endl;   
    return s.str();    
}

/**
 * Sets the "cycle grouping" mode of the Dot printout.
 *
 * If set, moves are grouped according to their scheduled cycles (if any).
 */
void
DataDependenceGraph::setCycleGrouping(bool flag) {
    cycleGrouping_ = flag;
}

/**
 * Returns the smallest cycle of a move in the DDG.
 *
 * Current implementation is very slow, so don't call in critical places.
 *
 * @return The smallest cycle of a move.
 */
int
DataDependenceGraph::smallestCycle() const {

    int minCycle = INT_MAX;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isScheduled())
            minCycle = std::min(minCycle, n.cycle());
    }

    return minCycle;
}

/**
 * Returns the largest cycle of a move in the DDG.
 *
 * Current implementation is very slow, so don't call in critical places.
 *
 * @return The largest cycle of a move.
 */
int
DataDependenceGraph::largestCycle() const {

    int maxCycle = 0;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isScheduled()) {
            maxCycle = std::max(maxCycle, n.cycle());
        }
    }

    return maxCycle;
}

/**
 * Returns the count of nodes in the graph that have been scheduled.
 *
 * Current implementation is very slow, so don't call in critical places.
 *
 * @return The count of scheduled nodes.
 */
int
DataDependenceGraph::scheduledNodeCount() const {

    int scheduledCount = 0;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isScheduled()) 
            ++scheduledCount;
    }

    return scheduledCount;
}

/**
 * Implements software bypassing by copying the input nodes of a node into
 * second node and removing the edge between them.
 * 
 * Also updates the source of the second move.
 *
 * @param resultNode node creating a result being bypassed.
 * @param userNode node whose source will be updated.
 */
void
DataDependenceGraph::mergeAndKeep(MoveNode& resultNode, MoveNode& userNode) {

    if (!hasEdge(resultNode, userNode)) {
        throw Exception(
            __FILE__,__LINE__,__func__,"No edge between nodes being merged");
    }

    if (!resultNode.isMove() || !userNode.isMove()) {
        throw Exception(
            __FILE__,__LINE__,__func__,"Cannot merge entry/exit node!");
    }

    // update the move
    userNode.move().setSource(resultNode.move().source().copy());

    // If source is an operation, set programOperation
    if (resultNode.isSourceOperation()) {
        ProgramOperation& srcOp = resultNode.sourceOperation();
        srcOp.addOutputNode(userNode);
        userNode.setSourceOperation(srcOp);
    }
    
    // remove RAW deps

    EdgeSet edges = connectingEdges(
        resultNode, userNode);

    for (EdgeSet::iterator i = edges.begin();
         i != edges.end(); i++ ) {
         DataDependenceEdge* edge = *i;
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW) {
            removeEdge(*edge);
        }
    }

    for (int i = 0; i < inDegree(resultNode); i++) {
        DataDependenceEdge& edge = inEdge(resultNode,i);

        // skip antidependencies due bypassed register.. these are no more
        if (edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            (edge.dependenceType() != DataDependenceEdge::DEP_WAW &&
             edge.dependenceType() != DataDependenceEdge::DEP_WAR)) {

            // copy other edges
            DataDependenceEdge* newEdge = new DataDependenceEdge(edge);
            MoveNode& source = tailNode(edge);
            connectNodes(source, userNode, *newEdge);
        }
    }
    
    // fix WAR antidependencies to WaW
    for (int i = 0; i < outDegree(userNode); i++) {
        DataDependenceEdge& edge = outEdge(userNode,i);
    
        // create new WaW in place of old WaR
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAR) {

            MoveNode& dest = headNode(edge);

            // do not create duplicate WaW edges
            if (!hasEdge(resultNode, dest)) {
                DataDependenceEdge* newEdge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW);

                connectNodes(resultNode, dest, *newEdge);
            }
            // and remove the old WaR
            removeEdge(edge);
        }
    }
 }

/**
 * Reverses work done by mergeAndKeep routine
 *
 * changes node to read it's data from register and returns the original
 * edges.
 *
 * @param resultNode node which writes to the register mergedNode should read.
 * @param mergedNode node being changed
 */
void
DataDependenceGraph::unMerge(MoveNode &resultNode, MoveNode& mergedNode) {

    // unset programoperation from bypassed
    if (mergedNode.isSourceOperation()) {
        ProgramOperation& srcOp = mergedNode.sourceOperation();
        srcOp.removeOutputNode(mergedNode);
        mergedNode.unsetSourceOperation();
    }

    // remove unnecessary edges from merged node
    // these can be operation edges or ordinary register RaWs.
    for (int i = 0; i < inDegree(mergedNode); i++) {
        DataDependenceEdge& edge = inEdge(mergedNode,i);
        // removes operation edges and 
        if (edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION ||
            (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
             edge.dependenceType() == DataDependenceEdge::DEP_RAW &&
             !edge.headPseudo() && !edge.guardUse())) {
            removeEdge(edge);
            i--; // do not skip next edge which now has same index
        } 
    }

    mergedNode.move().setSource(resultNode.move().destination().copy());

    // create register edge between nodes
    DataDependenceEdge* dde = new DataDependenceEdge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_RAW);
    connectNodes(resultNode, mergedNode, *dde);

    // If we changed WaR from second move to WaW of first move 
    // during bypassign, reverse this.
    for (int i = 0; i < outDegree(resultNode); i++) {
        DataDependenceEdge& edge = outEdge(resultNode,i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
            DataDependenceEdge* newEdge = new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER,
                DataDependenceEdge::DEP_WAR);

            MoveNode& dest = headNode(edge);
            connectNodes(mergedNode, dest, *newEdge);
            
            // leave the extra WaW there. it's existence does not
            // make schedule worse neither cause any bugs. It's just
            // unnecessary edge on some cases.
            // but removing it would break situations where we have
            // multiple bypasses from same source and we unmerge those.
        }
    }
}


/**
 * Checks whether a result is used later or if the result move can be 
 * dropped.
 *
 * @param resultNode node being checked for nonused results.
 */ 
bool
DataDependenceGraph::resultUsed(MoveNode& resultNode) {

    //naming of this variabl si reverse logic, ok if not used
    bool wawOK = true;
    if (!isRootGraphProcedureDDG()) {
        wawOK = false;
    }
    bool hasRAW = false;
    bool hasOtherEdge = false;
    
    EdgeSet edges = rootGraphOutEdges(resultNode);
    EdgeSet::iterator edgeIter = edges.begin();
    while (edgeIter != edges.end()) {

        DataDependenceEdge& edge = *(*edgeIter);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
            edgeIter++;
            continue;
        }
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {
            if (edge.dependenceType() == DataDependenceEdge::DEP_RAW) {
                // result is still going to be used
                hasRAW = true;
                break;
            }

            if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
                // result is overwritten
                wawOK = true;
            }

        } else {
            // there are some other outgoing edges
            hasOtherEdge = true;
        }
        edgeIter++;
    }
    return (!wawOK || hasRAW || hasOtherEdge );
}

/**
 * Removes MoveNode from graph and ProgramOperations.
 *
 * Does not delete the movenode.
 *
 * @param node MoveNode being removed.
 */
void
DataDependenceGraph::removeNode(MoveNode& node) throw (InstanceNotFound) {

    // bookkeeping about the removed node, in order to be able to restore it
    RemovedNodeData* rmn = new RemovedNodeData;
    removedNodes_[&node] = rmn;

    // remove move -> movenode mapping.
    if (node.isMove()) {
        TTAProgram::Move* move = &node.move();
        std::map<TTAProgram::Move*, MoveNode*>::iterator i = 
            nodesOfMoves_.find(move);
        if (i != nodesOfMoves_.end()) {
            nodesOfMoves_.erase(i);
        } 
    }

    DataDependenceGraph::NodeDescriptor nd = descriptor(node);

    EdgeSet iEdges = inEdges(node);
    
    for (EdgeSet::iterator i = iEdges.begin(); i != iEdges.end(); i++) {
        DataDependenceEdge& iEdge = **i;
        DataDependenceEdge* newIEdge = new DataDependenceEdge(iEdge);
        rmn->inEdges.push_back(
            std::pair<DataDependenceEdge*,MoveNode*>(
                newIEdge, &tailNode(iEdge,nd)));
    }

    EdgeSet oEdges = outEdges(node);

    for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
        DataDependenceEdge& oEdge = **i;
        DataDependenceEdge* newOEdge = new DataDependenceEdge(oEdge);
        rmn->outEdges.push_back(
            std::pair<DataDependenceEdge*,MoveNode*>(
                newOEdge, &headNode(oEdge,nd)));
    }

    // end of bookkeeping

    // remove node from program operations
    if (node.isMove()) {
        if (node.isSourceOperation()) {
            ProgramOperation& srcOp = node.sourceOperation();
            srcOp.removeOutputNode(node);
            node.unsetSourceOperation();
        }

        if (node.isDestinationOperation()) {
            ProgramOperation& dstOp = node.destinationOperation();
            dstOp.removeInputNode(node);
            node.unsetDestinationOperation();
        }

        // fix WaW and WaR antidependencies that go over this node

        for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
            DataDependenceEdge& oEdge = **i;
        
            if (oEdge.dependenceType() == DataDependenceEdge::DEP_WAW
                && oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {

                for (EdgeSet::iterator j = iEdges.begin(); 
                     j != iEdges.end(); j++) {
                    DataDependenceEdge& iEdge = **j;
                    
                    if (iEdge.dependenceType() == 
                        DataDependenceEdge::DEP_WAW &&
                        iEdge.edgeReason() == 
                        DataDependenceEdge::EDGE_REGISTER) {
                        
                        MoveNode& tail = tailNode(iEdge,nd);
                        MoveNode& head = headNode(oEdge,nd);
                        connectNodes(tail, head,*(new DataDependenceEdge(
                                         DataDependenceEdge::EDGE_REGISTER,
                                         DataDependenceEdge::DEP_WAW)));
                    } else if (iEdge.dependenceType() == 
                               DataDependenceEdge::DEP_WAR &&
                               iEdge.edgeReason() == 
                               DataDependenceEdge::EDGE_REGISTER) {
                        
                        MoveNode& tail = tailNode(iEdge,nd);
                        MoveNode& head = headNode(oEdge,nd);
                        connectNodes(tail, head, *(new DataDependenceEdge(
                                         DataDependenceEdge::EDGE_REGISTER,
                                         DataDependenceEdge::DEP_WAR)));
                    }
                }
            }
        }
    }
    
    // remove node from graph
    BoostGraph<MoveNode,DataDependenceEdge>::removeNode(node);
}

/** 
 * Removes a MoveNode from a graph and deletes it.
 *
 * @param node MoveNode being deleted.
 */
void
DataDependenceGraph::deleteNode(MoveNode& node) {
    removeNode(node);
    delete &node;
}

/*
 * Restores a removed node into graph.
 *
 * Restores node into graph. Restores also all edges the node had when
 * it was removed from the graph. All the nodes the node had egdes when
 * if was removed have to exist in the graph.
 *
 * @param node node being restored into the graph.
 */
void DataDependenceGraph::restoreNode(MoveNode& node) {
    
    addNode(node);
    
    std::map<MoveNode*,RemovedNodeData*>::iterator i = 
        removedNodes_.find(&node);
    if (i != removedNodes_.end()) {
        RemovedNodeData* rnd = i->second;
        // restore incoming edges
        
        for (int iec = rnd->inEdges.size() -1 ; iec >= 0 ; iec--) {
            std::pair<DataDependenceEdge*,MoveNode*>& p = rnd->inEdges[iec];
            if (hasNode(*p.second)) {
                connectNodes(*p.second, node, *p.first);
            } else {
                std::string msg = "Cannot restore edges to nodes not in graph."
                    " Trying to restore multiple nodes in non-stack-order?";
                throw NotAvailable(__FILE__,__LINE__,__func__,msg);
            }
            rnd->inEdges.pop_back();
        }
            
        // restore outgoing edges
        for (int oec = rnd->outEdges.size() -1; oec >= 0; oec--) {
            std::pair<DataDependenceEdge*,MoveNode*>& p = rnd->outEdges[oec];

            if (hasNode(*p.second)) {
                connectNodes(node, *p.second, *p.first);
            } else {
                std::string msg = "Cannot restore edges to nodes not in graph."
                    " Trying to restore multiple nodes in non-stack-order?";
                throw NotAvailable(__FILE__,__LINE__,__func__,msg);
            }
            rnd->outEdges.pop_back();
        }
        removedNodes_.erase(i);
    }
    
    // lets not care about removing overgoing antideps now, they are just
    // extra edges that only cost some memory but don't change the schedule
}

/**
 * Calculates a weight value for edges, to be used for 
 * path weight calculation for selector
 *
 * Current implementation quite simple to have something better than 
 * fixed weight without having to analyze the machine.
 * 3 equals about one cycle.
 *
 * @param e edge whose weight is being measured
 * @param n head node of the edge.
 * @return weigth of the edge.
 */
int
DataDependenceGraph::edgeWeight(DataDependenceEdge& e, const MoveNode& n) const {
    
    switch (e.edgeReason()) {
    case DataDependenceEdge::EDGE_OPERATION: {
        ProgramOperation& po = n.sourceOperation();
        Operation& op = po.operation();
        return getOperationLatency(op) * 3;
    }
    case DataDependenceEdge::EDGE_MEMORY: {
        return 5;
    }
    case DataDependenceEdge::EDGE_REGISTER: {
        switch (e.dependenceType()) {
            // TODO: some connectivity heuristics
        case DataDependenceEdge::DEP_RAW: {
            return 3; // Sw bypass puts this down, reduced connectivity up
        }
        case DataDependenceEdge::DEP_WAR: {
            return 1; // can be scheduled to same cycle
        }
        case DataDependenceEdge::DEP_WAW: {
            return 3; // correct one has to be the last one
        }
        default: {
            return 3;
        }
        }
    }
    case DataDependenceEdge::EDGE_RA: {
        return delaySlots_ != 0 ? (delaySlots_+1)*3 : 3; //6 better default?
    }
    default:
        return 3; 
    }
}

/**
 * Sets a machine into DDG. 
 *
 * This machine is used to some heuristics and helper functions that 
 * selector uses, for example path lenght calculation and earliestCycle.
 *
 * If no machine is set, these functions will still work but will
 * give non-optimal results
 *
 * @param machine machine to be used for heristics
 */
void
DataDependenceGraph::setMachine(const TTAMachine::Machine& machine) {
    machine_ = &machine;
    delaySlots_ = machine.controlUnit()->delaySlots();

    const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
        machine.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int j = 0; j < fu->operationCount(); j++) {
            TTAMachine::HWOperation* hwop = fu->operation(j);
            int latency = hwop->latency();
            std::string name = StringTools::stringToLower(hwop->name());
            // if does not exist or is existing is bigger update
            if (!AssocTools::containsKey(operationLatencies_, name)
                || latency < operationLatencies_[name]) {
                operationLatencies_[name] = latency;
            }
        }
    }
    height_ = -1; // force path recalculation
    sourceDistances_.clear();
    sinkDistances_.clear();
}

/**
 * Checks whether the given node has all its predcessors scheduled.
 *
 * Ignores intra operation edges, thus if the predecessors belongs to the
 * same operation, it need not be scheduled for the node to be considered
 * ready.
 *
 * @return True in case all predecessors are scheduled.
 */
bool
DataDependenceGraph::predecessorsReady(MoveNode& node) const {
    
    NodeSet pred = predecessors(node);
    for (NodeSet::iterator i = pred.begin(); i != pred.end();
         ++i) {

        MoveNode& m = **i;

        const bool operandMoveOfSameOperation =
            (node.isSourceOperation() && m.isDestinationOperation() && 
             &node.sourceOperation() == &m.destinationOperation());
        const bool resultMoveOfSameOperation = 
            (node.isSourceOperation() && m.isSourceOperation() &&
             &node.sourceOperation() == &m.sourceOperation());
        const bool operandsOfSameOperation =
            (node.isDestinationOperation() && m.isDestinationOperation() &&
             &node.destinationOperation() == &m.destinationOperation());
        
        if (operandMoveOfSameOperation || resultMoveOfSameOperation ||
            operandsOfSameOperation) {
            continue;
        } 
        if (!m.isScheduled()) {
            return false;
        }
    }
    return true;

}

/**
 * Gets the lowest instruction latency for given operation.
 *
 * If latency is know known ( no machine is given ) does some simple
 * heuristics.
 *
 * This function should propably be on somewhere else.
 *
 * @param op operation whose minimum latency is being searched
 * @return minimum latency of given operation. 
 */
int 
DataDependenceGraph::getOperationLatency(Operation& op) const {
    string name = StringTools::stringToLower(op.name());
    std::map<std::string, int>::const_iterator iter =
        operationLatencies_.find(name);
    if (iter != operationLatencies_.end()) {
        return iter->second;
    } else {
        return 1;
    }
}

/**
 * Checks if the graph already has an edge with same properties from same
 * node to same node.
 *
 * @param tailNode tail node of edge
 * @param headNode head node of edge
 * @param edge edge which for to test equality
 * @return true if equal edge exists, false if not exist
 */
bool
DataDependenceGraph::hasEqualEdge(
    const MoveNode& tailNode, const MoveNode& headNode, const DataDependenceEdge& edge) 
    const {
    
    typedef GraphTraits::out_edge_iterator outEdgeIter;
    std::pair<outEdgeIter, outEdgeIter> edges = boost::out_edges(
        descriptor(tailNode), graph_);
    NodeDescriptor hnd = descriptor(headNode);

    for (outEdgeIter ei = edges.first; ei != edges.second; ei++) {
        if (boost::target(*ei, graph_) == hnd) {
            DataDependenceEdge* dde = graph_[(*ei)];
            if (*dde == edge) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Connects nodes with an edge if equal edge between nodes not exists.
 * If equal edge already exists, deletes the given edge.
 *
 * @param tailNode tail node of edge
 * @param headNode head node of edge
 * @param edge edge which is added to graph or deleted.
 * @return true if connected, false if already existed.
 */
bool
DataDependenceGraph::connectOrDeleteEdge(        
    const MoveNode& tailNode, const MoveNode& headNode, 
    DataDependenceEdge* edge) {
    if (hasEqualEdge(tailNode, headNode, *edge)) {
        delete edge;
        return false;
    } else {
        connectNodes(tailNode, headNode, *edge);
        return true;
    }
}
/**
 * Destructor.
 * 
 * Removes the bookkeeping about nodes that are removed.
 */
DataDependenceGraph::RemovedNodeData::~RemovedNodeData() {

    // delete incoming edges
    for (int iec = inEdges.size() -1 ; iec >= 0 ; iec--) {
        std::pair<DataDependenceEdge*,MoveNode*>& p = inEdges[iec];

        delete p.first;
        inEdges.pop_back();
    }
            
    // delete outgoing edges
    for (int oec = outEdges.size() -1; oec >= 0; oec--) {
        std::pair<DataDependenceEdge*,MoveNode*>& p = outEdges[oec];
        delete p.first;
        
        outEdges.pop_back();
    }
}

/**
 * Creates a subgraph of a ddg from set of cede snippets
 * which contains instructions which contains moves
 * in this graph.
 *
 * @param nodes code being included in the subgraph
 * @param includeLoops whether to include loop-carried dependencies
 * @return the created subgraph
 */
DataDependenceGraph*
DataDependenceGraph::createSubgraph(
    NodeSet& nodes, bool includeLoops) 
    throw (InstanceNotFound) {
    DataDependenceGraph *subGraph = new DataDependenceGraph;
    constructSubGraph(*subGraph, nodes);

    subGraph->moveNodeBlocks_ = moveNodeBlocks_;

    // add all PO's which have some nodes to here
    for (POList::iterator iter = programOperations_.begin();
         iter != programOperations_.end(); iter++) {
        ProgramOperation* po = *iter;
        // check input moves
        for (int i = 0; i < po->inputMoveCount(); i++ ) {
            MoveNode* mn = &po->inputMove(i);
            if( AssocTools::containsKey(nodes,mn)) {
                subGraph->programOperations_.push_back(po);
                break;
            }
        }

        // check output moves
        for (int i = 0; i < po->outputMoveCount(); i++ ) {
            MoveNode* mn = &po->outputMove(i);
            if( AssocTools::containsKey(nodes,mn)) {
                subGraph->programOperations_.push_back(po);
                break;
            }
        }
    }

    if ( !includeLoops ) {
        subGraph->dropBackEdges();
    }
    return subGraph;
}

/**
 * Creates a subgraph of a ddg from set of cede snippets
 * which contains instructions which contains moves
 * in this graph.
 *
 * @param cs code being included in the subgraph
 * @param includeLoops whether to include loop-carried dependencies
 * @return the created subgraph
 */
DataDependenceGraph*
DataDependenceGraph::createSubgraph(
    TTAProgram::CodeSnippet& cs, bool includeLoops) 
    throw (InstanceNotFound) {
    NodeSet moveNodes;
    for (int i = 0; i < cs.instructionCount(); i++) {
        TTAProgram::Instruction& ins = cs.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++ ) {
            MoveNode& node = nodeOfMove(ins.move(j));
            moveNodes.insert(&node);
        }
    }
    return createSubgraph(moveNodes, includeLoops);
}

/**
 * Creates a subgraph of a ddg from set of cede snippets
 * which contains instructions which contains moves
 * in this graph.
 *
 * @param codeSnippets code being included in the subgraph
 * @param includeLoops whether to include loop-carried dependencies
 * @return the created subgraph
 */
DataDependenceGraph*
DataDependenceGraph::createSubgraph(
    std::list<TTAProgram::CodeSnippet*>& codeSnippets, bool includeLoops) 
    throw (InstanceNotFound) {
    NodeSet moveNodes;
    for( std::list<TTAProgram::CodeSnippet*>::iterator iter =
             codeSnippets.begin(); iter != codeSnippets.end(); iter++ ) {
        TTAProgram::CodeSnippet& cs = **iter;
        for (int i = 0; i < cs.instructionCount(); i++) {
            TTAProgram::Instruction& ins = cs.instructionAtIndex(i);
            for (int j = 0; j < ins.moveCount(); j++ ) {
                MoveNode& node = nodeOfMove(ins.move(j));
                moveNodes.insert(&node);
            }
        }
    }
    return createSubgraph(moveNodes, includeLoops);
}


/**
 *
 * Gets a node of a move.
 * 
 * Warning: this operations is currently O(n).
 * Smarter data structure needed for faster operation,
 * propably should be implemented.
 *
 * @param move move.
 * @return MoveNode of the given move
 */
MoveNode& 
DataDependenceGraph::nodeOfMove(TTAProgram::Move& move) 
    throw (InstanceNotFound) {

    std::map<TTAProgram::Move*, MoveNode*>::iterator i = 
        nodesOfMoves_.find(&move);
    if (i != nodesOfMoves_.end()) {
        return *(i->second);
    } 

    std::string msg = "move not in ddg: " + 
            Conversion::toString(reinterpret_cast<long>(&move)) + " " + 
            POMDisassembler::disassemble(move);
    throw InstanceNotFound(__FILE__,__LINE__,__func__, msg);

}

/**
 * Drops back edges from a sub-DDG.
 *
 * This works only with simple control structures. 
 */
void DataDependenceGraph::dropBackEdges() {

    const int nc = nodeCount();

    // first loop thru all nodes.
    for (int n = 0; n < nc; n++) {
        NodeDescriptor nd = boost::vertex(n, graph_);

        Node* tn = graph_[nd];

        // the thru all output edges of the node.
        std::pair<OutEdgeIter, OutEdgeIter> edges = 
            boost::out_edges(nd, graph_);

        for (OutEdgeIter ei = edges.first; ei != edges.second;) {
            DataDependenceEdge* e = graph_[(*ei)];

            MoveNode& hn = headNode(*e,nd);
            if (hn.nodeID() <= tn->nodeID() || 
                (e->edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                 e->dependenceType() == DataDependenceEdge::DEP_WAW &&
                 hn.isDestinationOperation() &&
                 tn->isDestinationOperation() &&
                 &hn.destinationOperation() == 
                 &tn->destinationOperation())) {
                
                // remove from internal bookkeeping
                boost::remove_edge(*ei, graph_);

                // iterators must be resetted when deleted something.
                edges = boost::out_edges(nd, graph_);
                ei = edges.first;

                // remove from edge descriptor cache
                DataDependenceGraph::EdgeDescMap::iterator
                    edIter = edgeDescriptors_.find(e);
                if (edIter != edgeDescriptors_.end()) {
                    edgeDescriptors_.erase(edIter);
                }

                for (unsigned int i = 0; i < childGraphs_.size(); i++) {
                    childGraphs_.at(i)->dropEdge(*e);
                }    
            } else {
                ei++;
            }
        }
    }
}

/**
 * Tells whether the root graph is a procedure-wide ddg or created from
 * a smaller piece of code. 
 *
 * This is needed for dead result elimination.
 *
 * @return if root graph of the subgraph tree contains whole procedure.
 */
bool 
DataDependenceGraph::isRootGraphProcedureDDG() {
    if (parentGraph_ == NULL) {
        return procedureDDG_;
    } else {
        return dynamic_cast<DataDependenceGraph*>(parentGraph_)
            ->isRootGraphProcedureDDG();
    }
}

/**
 * Addds inter-BB-Anti edges between the the basic blocks. 
 *
 * currently quite a heavy routine
 * 
 * @param bbn1 first basic block, executed first, sources of antidependence edges
 * @param bbn2 second basic block, executed later, targets of antidependence edges
 */
void
DataDependenceGraph::fixInterBBAntiEdges(
    BasicBlockNode& bbn1, BasicBlockNode& bbn2) throw (Exception) {

    // TODO: creating heave, remembering fast
    DataDependenceGraph* sg1 = createSubgraph(bbn1.basicBlock());
    DataDependenceGraph* sg2 = createSubgraph(bbn2.basicBlock());

    for (int n2 = 0; n2 < sg2->nodeCount(); n2++) {
        MoveNode& mn2 = sg2->node(n2);
        if (mn2.isMove() && mn2.move().destination().isGPR()) {
//            if (sg2->rAntiEdgesIn(mn2) == 0 ) {
                for (int n1 = sg1->nodeCount() -1 ; n1 >= 0; n1--) {
                    MoveNode& mn1 = sg1->node(n1); 
                    if (mn1.isMove()) {
                        if (mn1.move().source().isGPR() &&  // WAR?
                            mn1.move().source().equals(
                                mn2.move().destination())) {
                            DataDependenceEdge* edge = 
                                new DataDependenceEdge(
                                    DataDependenceEdge::EDGE_REGISTER,
                                    DataDependenceEdge::DEP_WAR);
                            connectNodes(mn1, mn2, *edge);
                        }
                        if (mn1.move().destination().isGPR() && // WAW?
                            mn1.move().destination().equals(
                                mn2.move().destination())) {
                            DataDependenceEdge* edge = 
                                new DataDependenceEdge(
                                    DataDependenceEdge::EDGE_REGISTER,
                                    DataDependenceEdge::DEP_WAW);
                            connectNodes(mn1, mn2, *edge);
                        }
                        // antideps also for guards
                        if (!mn1.move().isUnconditional()) {
                            TTAMachine::Guard& g = mn1.move().guard().guard();
                            TTAMachine::RegisterGuard* rg = 
                                dynamic_cast<TTAMachine::RegisterGuard*>(&g);
                            if (rg != NULL && 
                                mn2.move().destination().isGPR()) {
                                TTAProgram::TerminalRegister& tr = 
                                    dynamic_cast<
                                    TTAProgram::TerminalRegister&>(
                                        mn2.move().destination());
                                if (rg->registerFile() == &tr.registerFile() &&
                                    rg->registerIndex() == tr.index()) {
                                    
                                    DataDependenceEdge* edge = 
                                        new DataDependenceEdge(
                                            DataDependenceEdge::EDGE_REGISTER,
                                            DataDependenceEdge::DEP_WAR,true);
                                    connectNodes(mn1, mn2, *edge);
                                }
                            }
                        }
                    }
                }
//            }
        }
    }
    delete sg1;
    delete sg2;
 }

/**
 * Copies all dependencies going to and from a movenode to another
 * 
 * Creates copy of all edges going to and from an movenode and 
 * make them to point to and fro another movenode.
 * 
 * @param src movenode to copy dependencies from
 * @param dst movenode to copy dependencies to
 * @todo should this method be in base class?  would require graphedge.clone()
 */
void 
DataDependenceGraph::copyDependencies(
    MoveNode& src, MoveNode& dst) throw (InstanceNotFound) {

    // performance optimization 
    NodeDescriptor nd = descriptor(src);

    EdgeSet iEdges = inEdges(src);
    for (EdgeSet::iterator iter = iEdges.begin(); 
         iter != iEdges.end(); iter++) {
        DataDependenceEdge* edge = *iter;
        DataDependenceEdge* newEdge = new DataDependenceEdge(*edge);
        MoveNode& tail = tailNode(*edge,nd);
        connectNodes(tail, dst, *newEdge);
    }

    EdgeSet oEdges = outEdges(src);
    for (EdgeSet::iterator iter = oEdges.begin(); iter != oEdges.end(); 
         iter++) {
        DataDependenceEdge* edge = *iter;
        DataDependenceEdge* newEdge = new DataDependenceEdge(*edge);
        MoveNode& head = headNode(*edge,nd);
        connectNodes(dst, head, *newEdge);
    }
}

/**
 * Calculates number of register WAR antidependecies originating from 
 * given node
 *
 * @param mn Movenodes whose dependencies to calculate
 * @return number of register WAR antidependencies originating
 * from given node
 */
int DataDependenceGraph::rWarEdgesOut(MoveNode& mn) {
    EdgeSet oEdges = outEdges(mn);
    int count = 0;
    for (EdgeSet::iterator iter = 
             oEdges.begin(); iter != oEdges.end(); iter++) {
        if ((*iter)->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (*iter)->dependenceType() == DataDependenceEdge::DEP_WAR) {
            count++;
        }
    }
    return count;
}

/**
 * Calculates number of register antidependecies originating from 
 * given node
 *
 * @param mn Movenodes whose dependencies to calculate
 * @return number of register antidependencies originating
 * from given node
 */

int DataDependenceGraph::rWawRawEdgesOut(MoveNode& mn) {
    EdgeSet oEdges = outEdges(mn);
    int count = 0;
    for (EdgeSet::iterator iter = 
             oEdges.begin(); iter != oEdges.end(); iter++) {
        if ((*iter)->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (((*iter)->dependenceType() == DataDependenceEdge::DEP_RAW) ||
            ((*iter)->dependenceType() == DataDependenceEdge::DEP_WAW))) {
            count++;
        }
    }
    return count;
}

/**
 * Counts all incoming antidependence edges to a movenode.
 *
 * @param mn MoveNode whose edges we are counting
 * @return number of incoming antidependence edges.
 */
int DataDependenceGraph::rAntiEdgesIn(MoveNode& mn) {
    EdgeSet iEdges = inEdges(mn);
    int count = 0;
    for (EdgeSet::iterator iter = 
             iEdges.begin(); iter != iEdges.end(); iter++) {
        if ((*iter)->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (((*iter)->dependenceType() == DataDependenceEdge::DEP_WAR) ||
            ((*iter)->dependenceType() == DataDependenceEdge::DEP_WAW))) {
            count++;
        }
    }
    return count;
}
 
/**
 * Returns the only incoming guard edge to given node
 *
 * if there are multiple incoming guard edges, return NULL.
 * 
 * @param mn MoveNode whose incoming edges we are searching.
 * @return only guard edge to mn or NULL if no or multiple.
 */
DataDependenceEdge* 
DataDependenceGraph::onlyIncomingGuard(
    const MoveNode& mn) {
    DataDependenceEdge* guard = NULL;
    DataDependenceGraph::EdgeSet iEdges = inEdges(mn);

    for (DataDependenceGraph::EdgeSet::iterator i = iEdges.begin();
         i != iEdges.end(); i++) {
        DataDependenceEdge* edge = *i;
        if (edge->guardUse() && edge->dependenceType() == 
            DataDependenceEdge::DEP_RAW) {
            if (guard == NULL) {
                guard = edge;
            } else {
                return NULL; // too complicated guard
            }
        }
    }
    return guard;
}
