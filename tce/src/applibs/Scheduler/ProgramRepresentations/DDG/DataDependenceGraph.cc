/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file DataDependenceGraph.cc
 *
 * Implementation of data dependence graph class
 *
 * @author Heikki Kultala 2006-2010 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2010-2012
 * @author Fabio Garzia 2010 (fabio.garzia-no.spam-tut.fi)

 * @note rating: red
 */

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
#include "DisassemblyRegister.hh"
#include "ObjectState.hh"
#include "XMLSerializer.hh"
#include "MoveNodeSet.hh"
#include "LiveRangeData.hh"
#include "LiveRange.hh"
#include "Terminal.hh"
#include "BasicBlock.hh"
#include "Operation.hh"
#include "Move.hh"

/**
 * Constructor.
 *
 * @param name The graph can be named for debugging purposes.
 * @param containsProcedure whether the DDG contains complete procedure.
 * @param ownedBBN if the DDG should delete a BBn at destructor.
 * @param containsProcedure if the ddg contains a whole procedure.
 * @param if the ddg should not accept loop edges.
 * @param antidependenceLevel level of register antidependencies which this 
 *        ddg should contain.
 */
DataDependenceGraph::DataDependenceGraph(
    std::set<TCEString> allParamRegs,
    const TCEString& name, AntidependenceLevel antidependenceLevel,
    BasicBlockNode* ownedBBN, bool containsProcedure, 
    bool noLoopEdges) :
    BoostGraph<MoveNode, DataDependenceEdge>(name, !noLoopEdges), 
    allParamRegs_(allParamRegs), cycleGrouping_(true), 
    dotProgramOperationNodes_(false),
    machine_(NULL), delaySlots_(0), ownedBBN_(ownedBBN),
    procedureDDG_(containsProcedure), 
    registerAntidependenceLevel_(antidependenceLevel),
    edgeWeightHeuristics_(EWH_HEURISTIC) {
}

/**
 * Deletes all MoveNodes and ProgramOperations.
 */
DataDependenceGraph::~DataDependenceGraph() {

    if (parentGraph_ == NULL) {

        //delete nodes.
        int nc = nodeCount();
        for (int i = 0; i < nc; i++) {
            delete &node(i, false);
        }
        programOperations_.clear();
    }
    if (ownedBBN_ != NULL) {
        delete ownedBBN_;
    }
}

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
DataDependenceGraph::addNode(MoveNode& moveNode) {
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
 * Gives the basic block node where the node belongs to.
 *
 * @param mn MoveNode whose basic block we are asking
 * @return BasicBlockNode of the move
 */
const BasicBlockNode&
DataDependenceGraph::getBasicBlockNode(const MoveNode& mn) const {
    std::map<const MoveNode*, BasicBlockNode*>::const_iterator iter = 
        moveNodeBlocks_.find(&mn);
    if (iter == moveNodeBlocks_.end()) {
        throw InvalidData(__FILE__,__LINE__,__func__,"MoveNode not in DDG!");
    }
    return *iter->second;
}

/**
 * Gives the basic block node where the node belongs to.
 *
 * @param mn MoveNode whose basic block we are asking
 * @return BasicBlockNode of the move
 */
BasicBlockNode&
DataDependenceGraph::getBasicBlockNode(MoveNode& mn)  {
    std::map<const MoveNode*, BasicBlockNode*>::iterator iter = 
        moveNodeBlocks_.find(&mn);
    if (iter == moveNodeBlocks_.end()) {
        throw InvalidData(__FILE__,__LINE__,__func__,"MoveNode not in DDG!");
    }
    return *iter->second;
}

void
DataDependenceGraph::setBasicBlockNode(
    const MoveNode& mn, BasicBlockNode& bbn) {
    moveNodeBlocks_[&mn] = &bbn;
}

/** 
 * Returs programoperation which is in this graph.
 * 
 * @param index index of the programoperation.
 */
ProgramOperation& 
DataDependenceGraph::programOperation(int index) {
    return *programOperations_.at(index);
}

const ProgramOperation& 
DataDependenceGraph::programOperationConst(int index) const {
    return *programOperations_.at(index);
}


/**
 * Returns the number of programoperations in this ddg.
 */
int 
DataDependenceGraph::programOperationCount() const {
    return programOperations_.size();
}

/**
 * Returns the only incoming register edge to a node. 
 * If none or multiple, returns NULL.
 */
DataDependenceEdge* 
DataDependenceGraph::onlyRegisterEdgeIn(MoveNode& mn) const {
    
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

/**
 * Returns the only outgoing register edge from a node. 
 * If none or multiple, returns NULL.
 */
DataDependenceEdge* 
DataDependenceGraph::onlyRegisterEdgeOut(MoveNode& mn) const {

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
 * Adds a program operation to the graph, and its parent graphs.
 *
 * The graph then owns this programOperation.
 *
 * @param po ProgramOperation being added.
 */
void 
DataDependenceGraph::addProgramOperation(ProgramOperationPtr po) {
    programOperations_.push_back(po);
    if (parentGraph_ != NULL) {
        dynamic_cast<DataDependenceGraph*>(parentGraph_)
            ->addProgramOperation(po);
    }
}

int DataDependenceGraph::edgeLatency(const DataDependenceEdge& edge,
                                     int ii,
                                     const MoveNode* tail,
                                     const MoveNode* head) const {

    int latency = 1;
    if (edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
        if (head == NULL) {
            head = &headNode(edge);
        }
        if (head->isSourceOperation()) {
            if (tail == NULL) {
                tail = &tailNode(edge);
            }
            if (tail->inSameOperation(*head)) {
                latency = getOperationLatency(edge.data());
            }
        }
    } else if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
        latency = 0;
    }

    if (edge.headPseudo()) {
        latency -= delaySlots_;
    }

    if (edge.tailPseudo()) {
        latency += delaySlots_;
    }

    if (edge.guardUse()) {
        if (edge.dependenceType() == DataDependenceEdge::DEP_RAW) {
            latency += (head->guardLatency()-1);
        } else {
            if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                latency -= (tail->guardLatency()-1);
            } else {
                std::cerr << "invalid guard edge: " << edge.toString();
            }
        }
    }
    return latency - ii*edge.loopDepth();
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
DataDependenceGraph::earliestCycle(
    const MoveNode& moveNode, unsigned int ii, bool ignoreRegWaRs,
    bool ignoreRegWaWs, bool ignoreGuards, bool ignoreFuDeps,
    bool ignoreSameOperationEdges) const {

    if (machine_ == NULL) {
        throw InvalidData(__FILE__,__LINE__,__func__,
                          " setMachine() must be called before this");
    }
    const EdgeSet edges = inEdges(moveNode);
    int minCycle = 0;
    for (EdgeSet::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        DataDependenceEdge& edge = **i;

        if (ignoreGuards && edge.guardUse()) {
            continue;
        }

        if (ignoreFuDeps && 
            (edge.edgeReason() == DataDependenceEdge::EDGE_FUSTATE ||
             edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY)) {
            continue;
        }
        
        if (ignoreSameOperationEdges && 
            edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
            continue;
        }

        MoveNode& tail = tailNode(edge);
        if (ignoreSameOperationEdges && !edge.isBackEdge() &&
            moveNode.isSourceOperation() &&
            tail.isDestinationOperation() &&
            &moveNode.sourceOperation() == &tail.destinationOperation()) {
            continue;
        }
            

        if (tail.isScheduled()) {
            int latency = 1;
            int effTailCycle = tail.cycle();
            
            // If call, make sure all incoming deps fit into delay slots,
            // can still be later than the call itself
            // dependence type does not matter.
            if (edge.headPseudo()) {
                effTailCycle -= delaySlots_;
            } else {
                /// @todo clean this up: should be the same code as the edgeWeight 
                /// when EWH_REAL is used.
                if (edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION &&
                    moveNode.isSourceOperation() && tail.inSameOperation(moveNode)) {
                    effTailCycle += getOperationLatency(edge.data());
                } else if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {

                    // ignore reg antidep? then skip over this edge.
                    if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
                        && !edge.headPseudo() && 
                        ignoreRegWaWs) {
                        continue;
                    }
                    // latency does not matter with WAW. always +1.
                    effTailCycle += 1;
                } else {
                    if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                        // ignore reg antidep? then skip over this edge.
                        if (edge.edgeReason() == 
                            DataDependenceEdge::EDGE_REGISTER && 
                            !edge.headPseudo() && ignoreRegWaRs) {
                            continue;
                        }

                        // WAR allows writing at same cycle than reading.
                        // in WAR also the latency goes backwards, 
                        // new value can
                        // be written before old is read is latency is big.
                        if (edge.guardUse()) {
                            latency = tail.guardLatency();
                        } 

                        // TODO: generate some hasAutomaticBundling()
                        // property to ADF
                        if (machine_->triggerInvalidatesResults()) {
                            latency = 0;
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

            assert ((ii != 0 || edge.loopDepth() ==0) && 
                    "ii should not be 0 if we have an loop edge");

            effTailCycle -= (ii * edge.loopDepth());
            minCycle = std::max(effTailCycle, minCycle);
        } 
    }

    // on architectures with hw data hazard detection this is needed.
    // TODO: now does this for all input moves, not just trigger
    if (machine_->triggerInvalidatesResults() &&
        moveNode.isDestinationOperation()) {
        ProgramOperation& po = moveNode.destinationOperation();
        for (int i = 0; i < po.outputMoveCount(); i++) {
            MoveNode& outMove = po.outputMove(i);
            minCycle = 
                std::max(minCycle, 
                         earliestCycle(outMove, ii, 
                                       ignoreRegWaRs,
                                       ignoreRegWaWs, 
                                       ignoreGuards,
                                       true, // ignoreFuDeps
                                       true)); // operation edges ignored
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
DataDependenceGraph::latestCycle(
    const MoveNode& moveNode, unsigned int ii, bool ignoreRegAntideps,
    bool ignoreUnscheduledSuccessors, bool ignoreGuards, bool ignoreFuDeps,
    bool ignoreSameOperationEdges) const {

    if (machine_ == NULL) {
        throw InvalidData(__FILE__,__LINE__,__func__,
                          " setMachine() must be called before this");
    }
    
    const EdgeSet edges = outEdges(moveNode);
    int maxCycle = INT_MAX;
    for (EdgeSet::const_iterator i = edges.begin(); 
         i != edges.end(); ++i) {
        DataDependenceEdge& edge = **i;

        if (ignoreGuards && edge.guardUse()) {
            continue;
        }
        if (ignoreFuDeps &&
            (edge.edgeReason() == DataDependenceEdge::EDGE_FUSTATE ||
             edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY)) {
            continue;
        }

        if (ignoreSameOperationEdges &&
            edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
            continue;
        }

        MoveNode& head = headNode(edge);
        if (&head == &moveNode) {
            continue;
        }
        
        /// @todo Consider the latency for result read move!
        if (head.isPlaced()) {
            int latency = 1;
            int effHeadCycle = head.cycle();
            
            // If call, make sure all incoming deps fit into delay slots,
            // can still be later than the call itself
            // dependence type does not matter.
            if (edge.headPseudo()) {
                effHeadCycle += delaySlots_;
            } else {
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
                    
                    // ignore deg antidep? then skip over this edge.
                    if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
                        && !edge.tailPseudo() && ignoreRegAntideps) {
                        continue;
                    }

                    // latency does not matter with WAW. always +1.
                    effHeadCycle -= 1;
                } else {
                    if (edge.dependenceType() == DataDependenceEdge::DEP_WAR) {

                        // ignore deg antidep? then skip over this edge.
                        if (edge.edgeReason() == 
                            DataDependenceEdge::EDGE_REGISTER && 
                            !edge.tailPseudo() && ignoreRegAntideps) {
                            continue;
                        }

                        // WAR allows writing at same cycle than reading.
                        // in WAR also the latency goes backwards, 
                        // new value can
                        // be written before old is read is latency is big.
                        if (edge.guardUse()) {
                            latency = moveNode.guardLatency();
                        } 
                        // TODO: generate some hasAutomaticBundling()
                        // property to ADF
                        if (machine_->triggerInvalidatesResults()) {
                            latency = 0;
                        }
                        effHeadCycle = effHeadCycle + latency - 1;
                    } else {
                        // RAW
                        if (edge.guardUse()) {
                            latency = head.guardLatency();
                        } else {
                            if (edge.edgeReason() ==
                                DataDependenceEdge::EDGE_OPERATION &&
                                moveNode.isDestinationOperation() &&
                                head.inSameOperation(moveNode)) {
                                latency = getOperationLatency(edge.data());
                            }
                        }
                        // in case of RAW, value must be written latency
                        // cycles before it is used
                        effHeadCycle -= latency;
                    }
                }
            }

            assert ((ii != 0 || edge.loopDepth() ==0) && 
                    "ii should not be 0 if we have an loop edge");
            effHeadCycle += (ii * edge.loopDepth());

            maxCycle = std::min(effHeadCycle, maxCycle);
        } else {
            if (!edge.isBackEdge()) {
                if (!ignoreUnscheduledSuccessors) {
                    return -1;
                }
            }
        }

        // TODO: now does this for all input moves, not just trigger
        if (machine_->triggerInvalidatesResults() &&
            head.isSourceOperation()) {
            ProgramOperation& headPO = head.sourceOperation();
            for (int i = 0; i < headPO.inputMoveCount(); i++) {
                MoveNode& inputMove = headPO.inputMove(i);
                if (!inputMove.isPlaced()) {
                    continue;
                }

                if (edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                    (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     edge.edgeReason() == DataDependenceEdge::EDGE_RA)) {

                    // same operation. 0 cycle war.
                    // TODO: also this if bundle ordering correct.
                    if (moveNode.isDestinationOperation() &&
                        &moveNode.destinationOperation() ==
                        &headPO && !edge.isBackEdge()) {
                        maxCycle = std::min(maxCycle, inputMove.cycle());
                    } else {
                        // different operation.1 cycle war,to be sure,for now
                        maxCycle = std::min(maxCycle, inputMove.cycle()-1);
                    }
                }

                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW &&
                    (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     edge.edgeReason() == DataDependenceEdge::EDGE_RA)) {
                    maxCycle = std::min(maxCycle, inputMove.cycle()-1);
                }
            }
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
 * the given move node is predicated with. If there are multiple,
 * returns the last.
 *
 * @param moveNode The move node of which guard defining move to find.
 * @return The MoveNode that produces the guard value. 
 * If not found, returns NULL
 */
MoveNode*
DataDependenceGraph::lastGuardDefMove(MoveNode& moveNode) {
    NodeSet guardDefs = guardDefMoves(moveNode);
    MoveNode* last = NULL;
    for (NodeSet::iterator i = guardDefs.begin(); 
         i != guardDefs.end(); i++) {
        if (last == NULL || last->cycle() < (*i)->cycle()) {
            last = *i;
        }
    }
    return last;
}

/**
 * Returns all movenodes that define (write the value of) the guard
 * the given move node is predicated with. 
 *
 * @param moveNode The move node of which guard defining move to find.
 * @return The MoveNodes that produces the guard value. 
 *         Can be multiple if the writes are predicated or in different BB.
 * If not found, returns NULL
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::guardDefMoves(const MoveNode& moveNode) {

    NodeSet guardDefMoves;
    EdgeSet inputEdges = inEdges(moveNode);
    for (EdgeSet::iterator i = inputEdges.begin();
         i != inputEdges.end(); i++) {
        DataDependenceEdge &edge = **i;
        if (edge.guardUse() && 
            edge.dependenceType() == DataDependenceEdge::DEP_RAW) {
            guardDefMoves.insert(&tailNode(edge));
        }
    }
    return guardDefMoves;
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
    const TTAMachine::BaseRegisterFile& rf,
    int registerIndex,
    int lastCycleToTest) const {

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
            n.cycle() > lastCycle &&
            n.cycle() <= lastCycleToTest) {
            lastCycle = n.cycle();
            lastFound = &n;
        }
    }
    return lastFound;
}

/**
 * Returns the MoveNode with lowest cycle that reads the given register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @param firstCycleToTest optional argument from which cycle to start
 *        search.
 * @return The MoveNode, NULL, if not found.
 */
MoveNode*
DataDependenceGraph::firstScheduledRegisterRead(
    const TTAMachine::BaseRegisterFile& rf, 
    int registerIndex, int firstCycleToTest) const {

    int firstCycle = INT_MAX;
    MoveNode* firstFound = NULL;
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
            n.cycle() < firstCycle &&
            n.cycle() >= firstCycleToTest) {
            firstCycle = n.cycle();
            firstFound = &n;
        }
    }
    return firstFound;
}

/**
 * Returns the MoveNode with lowest cycle that writes the given register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The MoveNode, NULL, if not found.
 */
MoveNode*
DataDependenceGraph::firstScheduledRegisterWrite(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int firstCycle = INT_MAX;
    MoveNode* firstFound = NULL;
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& destination = n.move().destination();
        if (!n.isScheduled() || !destination.isGPR()) 
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
        currentRF = &destination.registerFile();

        if (&rf == currentRF && 
            destination.index() == registerIndex &&
            n.cycle() < firstCycle) {
            firstCycle = n.cycle();
            firstFound = &n;
        }
    }
    return firstFound;
}

/**
 * Returns the highest cycle where accesses the given register.
 * If unscheudled moves accessing the register, returns INT_MAX;
 * If none found, returns -1.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return cycle, or INT_MAX if some unscheduled found.
 */
int
DataDependenceGraph::lastRegisterCycle(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int lastCycle = -1;
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);
        TTAProgram::Move& move = n.move();
        const TTAMachine::BaseRegisterFile* currentRF = NULL;
        
        // check source
        TTAProgram::Terminal& source = move.source();
        bool sourceImmReg = source.isImmediateRegister();
        bool sourceGPR = source.isGPR();
        
        if (sourceImmReg || sourceGPR) {
            
            if (sourceImmReg)
                currentRF = &source.immediateUnit();
            else
                currentRF = &source.registerFile();
            
            if (&rf == currentRF && source.index() == registerIndex) {
                if (!n.isScheduled()) {
                    return INT_MAX;
                }
                if (n.cycle() > lastCycle) {
                    lastCycle = n.cycle();
                }
            }
        }
        
        // check destination
        TTAProgram::Terminal& destination = move.destination();
        if (destination.isGPR()) {
            currentRF = &destination.registerFile();
            
            if (&rf == currentRF && destination.index() == registerIndex) {
                if (!n.isScheduled()) {
                    return INT_MAX;
                }
                if (n.cycle() > lastCycle) {
                    lastCycle = n.cycle();
                }

                if (move.isUnconditional()) {
                    if (outDegree(n) == 0) {
                        assert(lastCycle == n.cycle());
                        return lastCycle;
                    }
                }
            }
        }
        
        // check guard.
        if (!move.isUnconditional()) {
            const TTAMachine::Guard& guard = move.guard().guard();
            const TTAMachine::RegisterGuard* rg =
                dynamic_cast<const TTAMachine::RegisterGuard*>(&guard);
            if (rg != NULL) {
                if (rg->registerFile() == &rf && 
                    rg->registerIndex() == registerIndex) {
                    if (!n.isScheduled()) {
                        return INT_MAX;
                    }
                    if (n.cycle() > lastCycle) {
                        lastCycle = n.cycle();
                    }
                }
            }
        }
        if (move.isCall()) {
            const TTAMachine::RegisterFile* rrf = 
                dynamic_cast<const TTAMachine::RegisterFile*>(&rf);
            assert(rrf != NULL);
            TCEString reg = 
                DisassemblyRegister::registerName(*rrf, registerIndex);
            if (allParamRegs_.find(reg) != allParamRegs_.end()) {
                if (!n.isScheduled()) {
                    return INT_MAX;
                }
                if (n.cycle() + delaySlots_> lastCycle) {
                    lastCycle = n.cycle() + delaySlots_ ;
                }
            }                    
        }
    }
    return lastCycle;
}

/**
 * Returns the lowest cycle where accesses the given register.
 * If unscheudled moves accessing the register, returns -1.
 * If none found, return INT_MAX
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return cycle, or -1 if some unscheduled found, or INT_MAX if none found.
 */
int
DataDependenceGraph::firstRegisterCycle(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int firstCycle = INT_MAX;
    for (int i = nodeCount()-1; i >= 0; --i) {
        MoveNode& n = node(i);
        TTAProgram::Move& move = n.move();
        const TTAMachine::BaseRegisterFile* currentRF = NULL;
        
        // check source
        TTAProgram::Terminal& source = move.source();
        bool sourceImmReg = source.isImmediateRegister();
        bool sourceGPR = source.isGPR();
        
        if (sourceImmReg || sourceGPR) {
            
            if (sourceImmReg)
                currentRF = &source.immediateUnit();
            else
                currentRF = &source.registerFile();
            
            if (&rf == currentRF && source.index() == registerIndex) {
                if (!n.isScheduled()) {
                    return -1;
                }
                if (n.cycle() < firstCycle) {
                    firstCycle = n.cycle();
                }
            }
        }
        
        // check destination
        TTAProgram::Terminal& destination = move.destination();
        if (destination.isGPR()) {
            currentRF = &destination.registerFile();
            
            if (&rf == currentRF && destination.index() == registerIndex) {
                if (!n.isScheduled()) {
                    return -1;
                }
                if (n.cycle() < firstCycle) {
                    firstCycle = n.cycle();
                }
                // this is a write of (constant) into reg, and no antideps in?
                if (move.isUnconditional()) {
                    if (inDegree(n) == 0) {
                        assert(firstCycle == n.cycle());
                        return firstCycle;
                    }
                }
            }
        }
        
        // check guard.
        if (!move.isUnconditional()) {
            const TTAMachine::Guard& guard = move.guard().guard();
            const TTAMachine::RegisterGuard* rg =
                dynamic_cast<const TTAMachine::RegisterGuard*>(&guard);
            if (rg != NULL) {
                if (rg->registerFile() == &rf && 
                    rg->registerIndex() == registerIndex) {
                    if (!n.isScheduled()) {
                        return -1;
                    }
                    if (n.cycle() < firstCycle) {
                        firstCycle = n.cycle();
                    }
                }
            }
        }
        if (move.isCall()) {
            const TTAMachine::RegisterFile* rrf = 
                dynamic_cast<const TTAMachine::RegisterFile*>(&rf);
            assert(rrf != NULL);
            TCEString reg = 
                DisassemblyRegister::registerName(*rrf, registerIndex);
            if (allParamRegs_.find(reg) != allParamRegs_.end()) {
                if (!n.isScheduled()) {
                    return -1;
                }
                if (n.cycle() + delaySlots_ < firstCycle) {
                    firstCycle = n.cycle() + delaySlots_ ;
                }
            }                    
        }
    }
    return firstCycle;
}


/**
 * Returns the set of MoveNodes which reads given register after
 * last unconditional scheduled write to the register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The set of movenodes.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::lastScheduledRegisterReads(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    MoveNode* lastKill = lastScheduledRegisterKill(rf, registerIndex);
    int killCycle = lastKill == NULL ? -1 : lastKill->cycle();
    NodeSet lastReads;

    // first search last kill.
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
            source.index() == registerIndex) {
            if (n.cycle() > killCycle) {
                lastReads.insert(&n);
            }
        }
    }
    return lastReads;
}


/**
 * Returns the set of MoveNodes which reads given register as guard after
 * last unconditional scheduled write to the register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The set of movenodes.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::lastScheduledRegisterGuardReads(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    MoveNode* lastKill = lastScheduledRegisterKill(rf, registerIndex);
    int killCycle = lastKill == NULL ? -1 : lastKill->cycle();
    NodeSet lastGuards;

    // first search last kill.
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);
        TTAProgram::Move& move = n.move();
        if (move.isUnconditional() || !n.isScheduled()) {
            continue;
        }

        const TTAMachine::Guard* guard = &move.guard().guard();
        const TTAMachine::RegisterGuard* rg =
            dynamic_cast<const TTAMachine::RegisterGuard*>(guard);
        if (rg == NULL) {
            continue;
        }
        const TTAMachine::BaseRegisterFile* currentRF = rg->registerFile();
        if (&rf == currentRF && 
            rg->registerIndex() == registerIndex) {
            if (n.cycle() > killCycle) {
                lastGuards.insert(&n);
            }
        }
    }
    return lastGuards;
}

/**
 * Returns the set of MoveNodes which writes given register after
 * last unconditional scheduled write to the register,
 * and the last unconditional write.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The set of movenodes.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::lastScheduledRegisterWrites(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    MoveNode* lastKill = lastScheduledRegisterKill(rf, registerIndex);
    int killCycle = lastKill == NULL ? -1 : lastKill->cycle();
    NodeSet lastReads;

    // first search last kill.
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& dest = n.move().destination();
        if (!n.isScheduled() || !dest.isGPR())
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
            currentRF = &dest.registerFile();

        if (&rf == currentRF && 
            dest.index() == registerIndex) {
            if (n.cycle() >= killCycle) {
                lastReads.insert(&n);
            }
        }
    }
    return lastReads;
}

/**
 * Returns the set of MoveNodes which writes given register after
 * last unconditional scheduled write to the register,
 * and the last unconditional write.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The set of movenodes.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::firstScheduledRegisterWrites(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    // TODO: should this be able to be calculated from LR bookkeeping?

    MoveNode* firstKill = firstScheduledRegisterKill(rf, registerIndex);
    int killCycle = firstKill == NULL ? INT_MAX : firstKill->cycle();
    NodeSet firstWrites;

    // first search last kill.
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& dest = n.move().destination();
        if (!n.isScheduled() || !dest.isGPR())
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
            currentRF = &dest.registerFile();

        if (&rf == currentRF && 
            dest.index() == registerIndex) {
            if (n.cycle() <= killCycle) {
                firstWrites.insert(&n);
            }
        }
    }
    return firstWrites;
}


/**
 * Returns the MoveNode of unconditional move with highest cycle that writes the given register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The MoveNode, NULL, if not found.
 */
MoveNode*
DataDependenceGraph::lastScheduledRegisterKill(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int lastCycle = -1;
    MoveNode* lastFound = NULL;
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& dest = n.move().destination();
        if (!n.isScheduled() || !dest.isGPR())
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
            currentRF = &dest.registerFile();

        if (&rf == currentRF && 
            dest.index() == registerIndex &&
            n.cycle() > lastCycle &&
            n.move().isUnconditional()) {
            lastCycle = n.cycle();
            lastFound = &n;
        }
    }
    return lastFound;
}

/**
 * Returns the MoveNode of unconditional move with highest cycle that writes the given register.
 *
 * @param rf The register file.
 * @param registerIndex Index of the register.
 * @return The MoveNode, NULL, if not found.
 */
MoveNode*
DataDependenceGraph::firstScheduledRegisterKill(
    const TTAMachine::BaseRegisterFile& rf, int registerIndex) const {

    int firstCycle = INT_MAX;
    MoveNode* firstFound = NULL;
    for (int i = 0; i < nodeCount(); ++i) {
        MoveNode& n = node(i);

        TTAProgram::Terminal& dest = n.move().destination();
        if (!n.isScheduled() || !dest.isGPR())
            continue;

        const TTAMachine::BaseRegisterFile* currentRF = NULL;
            currentRF = &dest.registerFile();

        if (&rf == currentRF && 
            dest.index() == registerIndex &&
            n.cycle() < firstCycle &&
            n.move().isUnconditional()) {
            firstCycle = n.cycle();
            firstFound = &n;
        }
    }
    return firstFound;
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
 * Returns all scheduled moves.
 *
 * @param cycle The cycle.
 * @return Scheduled moves.
 */
DataDependenceGraph::NodeSet
DataDependenceGraph::scheduledMoves() const {

    NodeSet moves;
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isScheduled())
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
DataDependenceGraph::sanityCheck() const {
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
                dynamic_cast<const TTAMachine::RegisterGuard*>(
                    &head.move().guard().guard()) != NULL) {
                const TTAMachine::RegisterGuard& g =
                    dynamic_cast<const TTAMachine::RegisterGuard&>(
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
            
            // memory WAR between memory op and call
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                tailDestination.hintOperation().readsMemory() &&
                headDestination.isFUPort() &&
                head.move().isCall())
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

            // memory WAW - can also point to call?
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                (tailDestination.hintOperation().writesMemory() &&
                ((headDestination.isFUPort() &&
                  headDestination.hintOperation().writesMemory()) ||
                 (headDestination.isFUPort() &&
                  head.move().isCall()))))
                break;

            // memory WAW between memory op and call
            if (e.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                tailDestination.isFUPort() && 
                tailDestination.hintOperation().writesMemory() &&
                headDestination.isFUPort() &&
                head.move().isCall())
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
TCEString
DataDependenceGraph::dotString() const {

    // TODO group based on both BB and cycle
    std::ostringstream s;
    s << "digraph " << name() << " {" << std::endl;
    
    if (cycleGrouping_ && !procedureDDG_ && !dotProgramOperationNodes_) {
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

        // in PONode mode, print the node for a single move only if
        // it doesn't belong to an operation
        if (dotProgramOperationNodes_ && n.isOperationMove()) 
            continue;

        TCEString nodeStr(n.dotString());
        if (false && isInCriticalPath(n)) {
            // convert critical path node shapes to invtriangle to make
            // the path stand out in the graph, this slows down the
            // printout probably quite a bit, TODO: optimize by caching
            // critical path data
            nodeStr.replaceString("shape=box", "shape=invtriangle");
            nodeStr.replaceString("shape=ellipse", "shape=invtriangle");
        }
        s << "\tn" << n.nodeID() << " [" << nodeStr << "]; " << std::endl;
    }

    typedef std::set<ProgramOperation*> POSet;
    POSet programOps;

    // edges. optimized low-level routines.
    typedef std::pair<EdgeIter, EdgeIter> EdgeIterPair;
    EdgeIterPair edges = boost::edges(graph_);
    for (EdgeIter i = edges.first; i != edges.second; i++) {
        EdgeDescriptor ed = *i;
        Edge& e = *graph_[ed];
        Node& tail = *graph_[boost::source(ed, graph_)];
        Node& head = *graph_[boost::target(ed, graph_)];

        
        if (dotProgramOperationNodes_ && 
            e.edgeReason() == DataDependenceEdge::EDGE_OPERATION)
            continue;
        TCEString tailNodeId;
        TCEString headNodeId;

        if (dotProgramOperationNodes_ && tail.isOperationMove()) {
            if (tail.isSourceOperation()) {
                tailNodeId << "po" << tail.sourceOperation().poId();
                programOps.insert(&tail.sourceOperation());
            } else {
                tailNodeId << "po" << tail.destinationOperation().poId();
                programOps.insert(&tail.destinationOperation());
            }
        } else {
            tailNodeId << "n" << tail.nodeID();
        }

        if (dotProgramOperationNodes_ && head.isOperationMove()) {
            if (head.isSourceOperation()) {
                headNodeId << "po" << head.sourceOperation().poId();
                programOps.insert(&head.sourceOperation());
            } else {
                headNodeId << "po" << head.destinationOperation().poId();
                programOps.insert(&head.destinationOperation());
            }
        } else {
            headNodeId << "n" << head.nodeID();
        }
        
        s << "\t" << tailNodeId
          << " -> " << headNodeId << "[";

        if (e.isFalseDep()) {
            s << "color=\"red\", ";
        }

        s << "label=\""
          << e.toString(tail) 
          << "\"];" << std::endl;    
    }

    // implicit operand to trigger edges
    for (int i = 0; i < nodeCount() && !dotProgramOperationNodes_; ++i) {
        Node& n = node(i);
        if (n.isMove() && n.move().isTriggering() &&
            n.isDestinationOperation()) {
            ProgramOperation &dst = n.destinationOperation();
            for (int j = 0; j < dst.inputMoveCount(); ++j) {
                if (dst.inputMove(j).nodeID() != n.nodeID()) {
                    s << "\tn" << dst.inputMove(j).nodeID()
                    << " -> n" << n.nodeID() << "["
                    << "label=\"T\"" << "];" << std::endl;
                }
            }
        }
    }

    if (dotProgramOperationNodes_) {
        for (POSet::iterator i = programOps.begin(); 
             i != programOps.end(); ++i) {
            const ProgramOperation& po = **i;
            TCEString label;

            int lineNo = -1;

            for (int i = 0; i < po.inputMoveCount(); ++i) {
                label += po.inputMove(i).toString() + "\\n";
                if (po.inputMove(i).move().hasSourceLineNumber())
                    lineNo = po.inputMove(i).move().sourceLineNumber();
            }
            label += "\\n";
            for (int i = 0; i < po.outputMoveCount(); ++i) {
                label += po.outputMove(i).toString() + "\\n";
            }

            if (lineNo != -1)
                label << "src line: " << lineNo << "\\n";

            s << "\tpo" << po.poId() << " [label=\"" 
              << label << "\",shape=box];" << std::endl;
        }
    }

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
 * Writes the graph into an XML file.
 *
 */
void
DataDependenceGraph::writeToXMLFile(std::string fileName) const {

    XMLSerializer serializer;
    ObjectState topOS = ObjectState("dependenceinfo");
    ObjectState* labelOS = new ObjectState("label", &topOS);
    ObjectState* nodesOS = new ObjectState("nodes", &topOS);
    ObjectState* edgesOS = new ObjectState("edges", &topOS);

    // Name of the unit
    labelOS->setValue(name());

    // Populate the nodes element
    for (int i = 0; i < nodeCount(); ++i) {
        ObjectState* nodeOS = new ObjectState("node", nodesOS);
        ObjectState* idOS = new ObjectState("id", nodeOS);
        ObjectState* labelOS = new ObjectState("label", nodeOS);

        Node& n = node(i);

        idOS->setValue(n.nodeID());
        labelOS->setValue(n.toString());

        if (n.isPlaced()) {
            ObjectState* cycleOS = new ObjectState("cycle", nodeOS);
            ObjectState* busOS = new ObjectState("slot", nodeOS);

            cycleOS->setValue(Conversion::toString(n.cycle()));
            busOS->setValue(n.move().bus().name());
        }
    }

    // Populate the edges element
    typedef std::pair<EdgeIter, EdgeIter> EdgeIterPair;
    EdgeIterPair edges = boost::edges(graph_);
    for (EdgeIter i = edges.first; i != edges.second; i++) {
        Edge& e = *graph_[*i];
        Node& tail = *graph_[boost::source(*i, graph_)];
        Node& head = *graph_[boost::target(*i, graph_)];
        edgesOS->addChild(e.saveState(tail, head));
    }

    // Implicit operand to trigger edges
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        if (n.isPlaced() && n.isMove() && n.move().isTriggering() &&
            n.isDestinationOperation()) {
            ProgramOperation &dst = n.destinationOperation();
            for (int j = 0; j < dst.inputMoveCount(); ++j) {
                if (dst.inputMove(j).nodeID() != n.nodeID()) {
                    
                    // Create a dummy edge and dump it
                    DataDependenceEdge e(DataDependenceEdge::EDGE_OPERATION,
                                         DataDependenceEdge::DEP_TRIGGER);
                    edgesOS->addChild(e.saveState(dst.inputMove(j), n));
                }
            }
        }
    }

    serializer.setDestinationFile(fileName);
    serializer.writeState(&topOS);
}

/**
 * Returns the smallest cycle of a move in the DDG.
 *
 * Current implementation is quite slow, so don't call in critical places.
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
 * Current implementation is quite slow, so don't call in critical places.
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
 * Current implementation is quite slow, so don't call in critical places.
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
 * Checks if software bypassing is allowed without causing a
 * deadlock through some circlar ddg dependence.
 *
 * @param sourceNode node writing the value which is bypassed
 * @param userNode node using the value, source of this node will be updated.
 *
*/

bool
DataDependenceGraph::mergeAndKeepAllowed(
    MoveNode& sourceNode, MoveNode& userNode) {

    // check against to WAR's to each others caused by
    // A->B ; B-A trying to be bypassed to same cycle. don't allow this.
    // TODO: this does not handle/detect: A -> B ; C -> A; D -> C  where
    // B -> D is being bypassed from A. creates a loop.

    int od = outDegree(sourceNode);
    for (int i = 0; i < od; i++) {
        DataDependenceEdge& edge = outEdge(sourceNode,i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
            !edge.tailPseudo()) {
            MoveNode& target = headNode(edge);
            if (!exclusingGuards(userNode, target)) {
                if (&target != &userNode && hasPath(target, userNode)) {
                    return false;
                }
            }
        }
    }

    if (!hasEdge(sourceNode, userNode)) {
        writeToDotFile("no_edge_on_merge.dot");
        throw Exception(
            __FILE__,__LINE__,__func__,"No edge between nodes being merged "
            + sourceNode.toString() + " " + userNode.toString());
    }

    if (!sourceNode.isMove() || !userNode.isMove()) {
        throw Exception(
            __FILE__,__LINE__,__func__,"Cannot merge entry/exit node!");
    }

    return true;
}

bool DataDependenceGraph::isLoopBypass(
    MoveNode& sourceNode, MoveNode& userNode) {
    EdgeSet edges = connectingEdges(
        sourceNode, userNode);

    for (auto edge: edges) {
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW &&
            edge->isBackEdge()) {
            return true;
        }
    }
    return false;
}

/**
 * Implements software bypassing by copying the input nodes of a node into
 * second node and removing the edge between them.
 * 
 * Also updates the source of the second move.
 *
 * @param sourceNode node writing the value which is bypassed
 * @param userNode node using the value, source of this node will be updated.
 *
*/

bool
DataDependenceGraph::mergeAndKeep(MoveNode& sourceNode, MoveNode& userNode) {

    // check against to WAR's to each others caused by
    // A->B ; B-A trying to be bypassed to same cycle. don't allow this.
    // TODO: this does not handle/detect: A -> B ; C -> A; D -> C  where
    // B -> D is being bypassed from A. creates a loop.

    for (int i = 0; i < outDegree(sourceNode); i++) {
        DataDependenceEdge& edge = outEdge(sourceNode,i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
            !edge.tailPseudo()) {
            MoveNode& target = headNode(edge);
            if (!exclusingGuards(userNode, target)) {
                if (&target != &userNode && hasPath(target, userNode)) {
                    return false;
                }
            }
        }
    }

//    writeToDotFile("before_merge.dot");

    assert(sourceNode.isMove());

    if (!hasEdge(sourceNode, userNode)) {
        writeToDotFile("no_edge_on_merge.dot");
        throw Exception(
            __FILE__,__LINE__,__func__,"No edge between nodes being merged "
            + sourceNode.toString() + " " + userNode.toString());
    }

    if (!sourceNode.isMove() || !userNode.isMove()) {
        throw Exception(
            __FILE__,__LINE__,__func__,"Cannot merge entry/exit node!");
    }

    // update the move
    userNode.move().setSource(sourceNode.move().source().copy());

    bool sourceIsRegToItselfCopy = false;
    // If source is an operation, set programOperation
    if (sourceNode.isSourceOperation()) {
        ProgramOperationPtr srcOp = sourceNode.sourceOperationPtr();
        srcOp->addOutputNode(userNode);
        userNode.setSourceOperationPtr(srcOp);

        // set fu annotations
        for (int j = 0; j < sourceNode.move().annotationCount(); j++) {
            TTAProgram::ProgramAnnotation anno = sourceNode.move().annotation(j);
            if (anno.id() == TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC) {
                userNode.move().setAnnotation(
                    TTAProgram::ProgramAnnotation(
                        TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC, anno.payload()));
            }
        }
    } else {
        // bypassing from stupid reg-to-itself needs extra handling.
        if (sourceNode.move().source().equals(
                sourceNode.move().destination())) {
            sourceIsRegToItselfCopy = true;
        }
    }

    // remove RAW deps between source and user.

    TCEString regName;

    EdgeSet edges = connectingEdges(
        sourceNode, userNode);

    for (EdgeSet::iterator i = edges.begin();
         i != edges.end(); i++ ) {
         DataDependenceEdge* edge = *i;
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW) {
            regName = edge->data();
            removeEdge(*edge, &sourceNode, &userNode);
        }
    }
    
    // there must have been a register raw edge between source and user node
    assert(regName != "");

    // if we are bypassign from a register-to-register copy, we'll have to
    // copy incoming raw edges also in rootgraph level to preserve inter-bb
    // -dependencies. 
    for (int i = 0; i < rootGraphInDegree(sourceNode); i++) {
        DataDependenceEdge& edge = rootGraphInEdge(sourceNode,i);

        // skip antidependencies due bypassed register.. these are no more
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.data() == regName) {
            if (edge.dependenceType() == DataDependenceEdge::DEP_WAW ||
                edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                continue;
            }
        }

        // copy other edges.
        MoveNode& source = rootGraph()->tailNode(edge);
        DataDependenceEdge* newEdge = new DataDependenceEdge(edge);
        
        // it the edge is a loop edge, put it only in root/parent graph.
        if (parentGraph_ != NULL) {
            static_cast<DataDependenceGraph*>(rootGraph())->
                connectNodes(source, userNode, *newEdge, 
                             (edge.isBackEdge()?this:NULL));
        } else {
            connectNodes(source, userNode, *newEdge);
        }
    }

    if (sourceNode.isSourceVariable()) {
        // if bypassing reg-to-reg this copy anti edges resulting from the
        // read of the other register.
        for (int i = 0; i < outDegree(sourceNode); i++) {
            DataDependenceEdge& edge = outEdge(sourceNode,i);
            if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                !edge.tailPseudo()) {
                MoveNode& target = headNode(edge);
                if (!exclusingGuards(userNode, target) && 
                    &userNode != &target) {
                    
                    DataDependenceEdge* newEdge = new DataDependenceEdge(edge);
                    // TODO: loop here!
                    connectNodes(userNode, target, *newEdge);
                }
            }
        }
    }

    // fix WAR antidependencies to WaW
    for (int i = 0; i < outDegree(userNode); i++) {
        DataDependenceEdge& edge = outEdge(userNode,i);
    
        // create new WaW in place of old WaR
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAR && 
            edge.data() == regName) {

            // if stupid reg to itself copy, keep to in edges..
            if (!sourceIsRegToItselfCopy) {
            // and remove the old WaR
                removeEdge(edge, &userNode, NULL);
                i--; // don't skip one edge here!
            }
        }
    }
    return true;
}

/**
 * Reverses work done by mergeAndKeep routine
 *
 * changes node to read it's data from register and returns the original
 * edges.
 *
 * @param sourceNode node which writes to the register mergedNode should read.
 * @param mergedNode node being changed
 */
void
DataDependenceGraph::unMerge(MoveNode &sourceNode, MoveNode& mergedNode) {

    // if this is not rootgraph, do it there. allows this to work even if
    // source node being deleted from this this sub-ddg.

    if (rootGraph() != this) {
        static_cast<DataDependenceGraph*>(rootGraph())->
            unMerge(sourceNode, mergedNode);
        return;
    } 

    // name of the bypass reg.
    TTAProgram::Terminal& dest = sourceNode.move().destination();
    assert(dest.isGPR());
    TCEString regName = DisassemblyRegister::registerName(dest);

    // unset programoperation from bypassed
    if (mergedNode.isSourceOperation()) {
        ProgramOperation& srcOp = mergedNode.sourceOperation();
        srcOp.removeOutputNode(mergedNode);
        mergedNode.unsetSourceOperation();

	// unset fu annotations
	mergedNode.move().removeAnnotations(TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC);
    }

    // All incoming RAW and operation dependencies were created by
    // the bypassing, originally there was just one RAW edge which was removed.
    // so remove the incoming edges merge routine copied to here.
    // these can be operation edges or ordinary register RaWs.
    // these should only go to source node.
    for (int i = 0; i < inDegree(mergedNode); i++) {
        DataDependenceEdge& edge = inEdge(mergedNode,i);
        // removes operation edges and 
        if (edge.edgeReason() == DataDependenceEdge::EDGE_OPERATION ||
            (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
             edge.dependenceType() == DataDependenceEdge::DEP_RAW &&
             !edge.headPseudo() && !edge.guardUse())) {
            removeEdge(edge, NULL, &mergedNode);
            i--; // do not skip next edge which now has same index
        } 
    }

    // do the actual unmerge by returning source to original register.
    mergedNode.move().setSource(sourceNode.move().destination().copy());

    bool nodeRegToItselfCopy = false;
    if (mergedNode.move().source().equals(mergedNode.move().destination())) {
        nodeRegToItselfCopy = true;
    }

    // create register edge between nodes.
    // this was removed by the merge.
    DataDependenceEdge* dde = new DataDependenceEdge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_RAW, regName);
    connectNodes(sourceNode, mergedNode, *dde);

    // remove war antidependence edges that should 
    // come from source. these should only come from the
    if (sourceNode.isSourceVariable()) {
        for( int i = 0; i < outDegree(mergedNode); i++) {
            DataDependenceEdge& edge = outEdge(mergedNode,i);
            if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                edge.data() != regName && !edge.tailPseudo() &&
                !edge.guardUse()) {
                removeEdge(edge, &mergedNode, NULL);
                i--; // do not skip next edge which now has same index
            }
        }
    }

    // All all outgoing WAR's to merged node. The war's go to 
    // all same places where WAW's fo from source node.
    for (int i = 0; i < outDegree(sourceNode); i++) {
        DataDependenceEdge& edge = outEdge(sourceNode,i);
        if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge.dependenceType() == DataDependenceEdge::DEP_WAW &&
            edge.data() == regName) {
            MoveNode& dest = headNode(edge);
            // skip nodes with exclusive guard.
            if (!exclusingGuards(dest, mergedNode)) {
                DataDependenceEdge* newEdge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, regName, false, false,
                    false, edge.headPseudo(), edge.loopDepth() + 
                    nodeRegToItselfCopy);
                connectNodes(mergedNode, dest, *newEdge);
            }
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
    bool killingWrite = true;
    if (!isRootGraphProcedureDDG()) {
        killingWrite = false;
    }
    bool hasRAW = false;
    bool hasOtherEdge = false;
    
    EdgeSet edges = rootGraphOutEdges(resultNode);
    EdgeSet::iterator edgeIter = edges.begin();
    while (edgeIter != edges.end()) {

        DataDependenceEdge& edge = *(*edgeIter);
        // don't case about operation edges.
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

            if (killingWrite == false) {
                // TODO: does this break if the waw dest unconditional?
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW &&
                    !edge.headPseudo()) {
                    MoveNode& head = 
                    (static_cast<const DataDependenceGraph*>
                     (rootGraph()))->headNode(edge);
                    if (head.isMove() && head.move().isUnconditional()) {
                        killingWrite = true;
                    }
                }
            }

        } else {
            // there are some other outgoing edges
            hasOtherEdge = true;
        }
        edgeIter++;
    }
    return (!killingWrite || hasRAW || hasOtherEdge );
}

/**
 * Copies dependencies over the node, like when the node is being deleted.
 *
 * Converts WaR + WaW to WaR and WaW + WaW to WaW.
 * @param node node whose in- and outgoing antideps are being combined/copied.
 */
DataDependenceGraph::EdgeSet
DataDependenceGraph::copyDepsOver(MoveNode& node, bool anti, bool raw) {

    EdgeSet createdEdges;

    DataDependenceGraph::NodeDescriptor nd = descriptor(node);

    EdgeSet iEdges = inEdges(node);
    EdgeSet oEdges = outEdges(node);

    // Loop thru all outedges.
    for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
        DataDependenceEdge& oEdge = **i;
        
        // Care about WaW edges. (WaW+Waw -> Waw, WaR+WaW->War)
        // Also raw edges (raw+raw -> raw)
        if ((oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
             || oEdge.edgeReason() == DataDependenceEdge::EDGE_RA) && 
            (((oEdge.dependenceType() == DataDependenceEdge::DEP_WAW && anti)||
              (oEdge.dependenceType() == DataDependenceEdge::DEP_RAW && raw)))){

            // Then loop all incoming edges.
            for (EdgeSet::iterator j = iEdges.begin(); 
                 j != iEdges.end(); j++) {
                DataDependenceEdge& iEdge = **j;
                
                // if WAW and same register, copy edge
                if (iEdge.dependenceType() == oEdge.dependenceType() &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {
                    
                    MoveNode& tail = tailNode(iEdge,nd);
                    MoveNode& head = headNode(oEdge,nd);
                    
                    if (!exclusingGuards(tail, head) ||
                        (oEdge.dependenceType() == 
                         DataDependenceEdge::DEP_RAW &&
                         oEdge.guardUse())) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        if (connectOrDeleteEdge(tail, head, edge)) {
                            createdEdges.insert(edge);
                        }
                    }
                }

                // if WAR and same register, and oedge was waw, copy edge.
                if (iEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                    oEdge.dependenceType() == DataDependenceEdge::DEP_WAW &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {
                    
                    MoveNode& tail = tailNode(iEdge,nd);
                    MoveNode& head = headNode(oEdge,nd);
                    
                    if (!exclusingGuards(tail, head) || iEdge.guardUse()) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        if (connectOrDeleteEdge(tail, head, edge)) {
                            createdEdges.insert(edge);
                        }
                    }
                }
            }
        }
    }
    return createdEdges;
}

void
DataDependenceGraph::copyDepsOver(
    MoveNode& node1, MoveNode& node2, bool anti, bool raw) {

    DataDependenceGraph::NodeDescriptor nd1 = descriptor(node1);
    DataDependenceGraph::NodeDescriptor nd2 = descriptor(node2);

    EdgeSet iEdges1 = inEdges(node1);
    EdgeSet oEdges1 = outEdges(node1);
    EdgeSet iEdges2 = inEdges(node2);
    EdgeSet oEdges2 = outEdges(node2);
        
    // Loop through all outedges of last one
    for (EdgeSet::iterator i = oEdges2.begin(); i != oEdges2.end(); i++) {
        DataDependenceEdge& oEdge = **i;

        // only care about register edges
        if (!(oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
              || oEdge.edgeReason() == DataDependenceEdge::EDGE_RA)) {
            continue;
        }

        // are we copying this type edges?
        if (!(((oEdge.dependenceType() == DataDependenceEdge::DEP_WAW||
                oEdge.dependenceType() == DataDependenceEdge::DEP_WAR) && anti) ||
              (oEdge.dependenceType() == DataDependenceEdge::DEP_RAW && raw))) {
            continue;
        }

        MoveNode& head = headNode(oEdge, nd2);

        if (oEdge.dependenceType() == DataDependenceEdge::DEP_RAW && raw) {
            // Then loop all incoming edges for raw deps
            for (EdgeSet::iterator j = iEdges1.begin(); 
                 j != iEdges1.end(); j++) {
                DataDependenceEdge& iEdge = **j;
                
                // RAW going over node.
                if (iEdge.dependenceType() == oEdge.dependenceType() &&
                    iEdge.dependenceType() == DataDependenceEdge::DEP_RAW &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {

                    MoveNode& tail = tailNode(iEdge, nd1);

                    if (!exclusingGuards(tail, head) ||
                        (oEdge.dependenceType() == oEdge.guardUse())) {

                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse() || oEdge.guardUse(),
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }
            }
        }
         
        if (!anti) {
            continue;
        }

        if (oEdge.dependenceType() == DataDependenceEdge::DEP_WAW) {
            // Then loop all incoming edges for waw deps
            for (EdgeSet::iterator j = iEdges2.begin(); 
                 j != iEdges2.end(); j++) {
                DataDependenceEdge& iEdge = **j;

                // WAW going over node.
                if (iEdge.dependenceType() == oEdge.dependenceType() &&
                    iEdge.dependenceType() == DataDependenceEdge::DEP_WAW &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {

                    MoveNode& tail = tailNode(iEdge);

                    if (!exclusingGuards(tail, head) && &tail != &node1) {

                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }

                // if WAR and same register, and oedge was waw, copy edge.
                if (iEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                    oEdge.dependenceType() == DataDependenceEdge::DEP_WAW &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {
                    
                    MoveNode& tail = tailNode(iEdge,nd2);
                    
                    if (!exclusingGuards(tail, head)) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }
            }
        }

        // last one reads interesting register
        if (oEdge.dependenceType() == DataDependenceEdge::DEP_WAR) {
            for (EdgeSet::iterator j = iEdges1.begin(); 
                 j != iEdges1.end(); j++) {
                DataDependenceEdge& iEdge = **j;
                if ((iEdge.dependenceType() == DataDependenceEdge::DEP_WAR ||
                     iEdge.dependenceType() == DataDependenceEdge::DEP_WAW) &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {
                    MoveNode& tail = tailNode(iEdge,nd2);
                    
                    if (!exclusingGuards(tail, head)) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }
            }
        }
    }

    if (!anti) {
        return;
    }

    // copy WARs over.
    for (EdgeSet::iterator i = oEdges1.begin(); i != oEdges1.end(); i++) {
        DataDependenceEdge& oEdge = **i;

        // only care about register edges
        if (!(oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
              || oEdge.edgeReason() == DataDependenceEdge::EDGE_RA)) {
            continue;
        }

        if (oEdge.dependenceType() == DataDependenceEdge::DEP_WAR) {

            MoveNode& head = headNode(oEdge,nd1);
            if (&head == &node2) {
                continue;
            }

            // Then loop all incoming edges for waw deps
            for (EdgeSet::iterator j = iEdges2.begin(); 
                 j != iEdges2.end(); j++) {
                DataDependenceEdge& iEdge = **j;

                // WAW going over node.
                if (iEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {

                    MoveNode& tail = tailNode(iEdge,nd2);
                    if (!exclusingGuards(tail, head) && &tail != &node1) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }
            }
        }
        if (oEdge.dependenceType() == DataDependenceEdge::DEP_WAW) {
            MoveNode& head = headNode(oEdge,nd1);
            if (&head == &node2) {
                continue;
            }

            for (EdgeSet::iterator j = iEdges1.begin(); 
                 j != iEdges1.end(); j++) {
                DataDependenceEdge& iEdge = **j;
                if ((iEdge.dependenceType() == DataDependenceEdge::DEP_WAR ||
                     iEdge.dependenceType() == DataDependenceEdge::DEP_WAW) &&
                    (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
                     iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    iEdge.data() == oEdge.data()) {
                    MoveNode& tail = tailNode(iEdge,nd2);
                    
                    if (!exclusingGuards(tail, head)) {
                        // create new edge
                        // same other properties but sum loop depth
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                iEdge.edgeReason(),
                                iEdge.dependenceType(),
                                iEdge.data(),
                                iEdge.guardUse(), 
                                false, 
                                iEdge.tailPseudo(),
                                oEdge.headPseudo(),
                                iEdge.loopDepth() + oEdge.loopDepth());
                        
                        connectOrDeleteEdge(tail, head, edge);
                    }
                }
            }
        }
    }
}

void
DataDependenceGraph::combineNodes(
    MoveNode& node1, MoveNode& node2, MoveNode& destination) {

    DataDependenceGraph::NodeDescriptor nd1 = descriptor(node1);
    DataDependenceGraph::NodeDescriptor nd2 = descriptor(node2);

    moveOutEdges(node2, destination);
    moveInEdges(node1, destination);

    EdgeSet oEdges1 = outEdges(node1);
    EdgeSet iEdges2 = inEdges(node2);

    // copy WARs over.
    for (EdgeSet::iterator i = oEdges1.begin(); i != oEdges1.end(); i++) {
        DataDependenceEdge& oEdge = **i;

        if ((oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
             || oEdge.edgeReason() == DataDependenceEdge::EDGE_RA) && 
            oEdge.dependenceType() == DataDependenceEdge::DEP_WAR) {

            MoveNode& head = headNode(oEdge,nd1);
            if (&head == &node2) {
                continue;
            }
            moveOutEdge(node1, destination, oEdge);
        }
    }

    for (EdgeSet::iterator i = iEdges2.begin(); i != iEdges2.end(); i++) {
        DataDependenceEdge& iEdge = **i;
        
        if ((iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER
             || iEdge.edgeReason() == DataDependenceEdge::EDGE_RA) && 
            (iEdge.dependenceType() == DataDependenceEdge::DEP_WAR 
             || iEdge.dependenceType() == DataDependenceEdge::DEP_WAW)) {

            MoveNode& tail = tailNode(iEdge,nd2);
            if (&tail == &node1) {
                continue;
            }
            moveInEdge(node2, destination, iEdge);
        }
    }
}





/**
 * Removes MoveNode from graph and ProgramOperations.
 *
 * Does not delete the movenode.
 *
 * This does NOT update overgoing antideps - consider calling
 *    copyAntidepsOver(node);
 * before calling this.
 *
 * @param node MoveNode being removed.
 */
void
DataDependenceGraph::removeNode(MoveNode& node) {
    // remove move -> movenode mapping.
    if (node.isMove()) {
        TTAProgram::Move* move = &node.move();
        auto i = nodesOfMoves_.find(move);
        if (i != nodesOfMoves_.end()) {
            nodesOfMoves_.erase(i);
        } 
    }

    // remove node from program operations
    if (node.isMove()) {
        if (node.isSourceOperation()) {
            ProgramOperation& srcOp = node.sourceOperation();
            srcOp.removeOutputNode(node);
            node.unsetSourceOperation();
        }

        if (node.isDestinationOperation()) {
            for (int i = node.destinationOperationCount() - 1; i >= 0; i--) {
                ProgramOperation& dstOp = node.destinationOperation(i);
                dstOp.removeInputNode(node);
            }
            node.clearDestinationOperation();
        }
    }
    
//    copyAntidepsOver(node);
    
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

/**
 * Calculates a weight value for edges, to be used for 
 * path weight calculation for selector
 *
 * Current implementation quite simple to have something better than 
 * fixed weight without having to analyze the machine.
 * 3 equals about one cycle.
 *
 * In order to use a version that computes the weight strictly using only
 * the operation latencies, thus gives the minimum achievable schedule
 * length given enough resources (with operation latencies as in the
 * machine), call setEdgeWeightHeuristics(EWH_REAL) before calling this.
 * To get back to the default one, call setEdgeWeightHeuristics(EWH_DEFAULT).
 *
 * @todo EWH_HEURISTIC is incomplete. It should take in account the number
 * of resources in the current target machine and not assume a "generic
 * machine".
 *
 * @param e edge whose weight is being measured
 * @param n head node of the edge.
 * @return weigth of the edge.
 */
int
DataDependenceGraph::edgeWeight(
    DataDependenceEdge& e, const MoveNode& n) const {

    if (e.headPseudo()) {
        // pseudo deps do not really limit the scheduling.
        return 0;
    }
    
    switch (e.edgeReason()) {
    case DataDependenceEdge::EDGE_OPERATION: {
        return getOperationLatency(e.data());
    }
    case DataDependenceEdge::EDGE_MEMORY: {
        if (e.dependenceType() == DataDependenceEdge::DEP_RAW) {
            // allowed to write a new value at the same cycle
            // another reads it (the load gets the old value)
            return 0; 
        } else {
            return 1;
        }
    }
    case DataDependenceEdge::EDGE_RA: 
    case DataDependenceEdge::EDGE_REGISTER: {
        switch (e.dependenceType()) {
            // TODO: some connectivity heuristics
        case DataDependenceEdge::DEP_RAW: {
            if (e.guardUse()) {
                int glat = n.guardLatency();
                // jump guard?
                
                // for predicated control flow ops the edge weight is
                // guard latency + delay slots, as the predicated cflow
                // ins has to be scehduled delay slots cycles before end.
                if (n.isMove() && n.move().isControlFlowMove()) {
                    return std::max(1, glat + delaySlots_);
                }
                // some other predicated operation
                return std::max(1, glat);
            } else {
                // jump address of indirect branch
                // indirect control flow ops the delay slots is added
                // to the edgeweight, as the indirect control flow ins
                // has to be scehduled delay slots cycles before end.
                if (n.isMove() && n.move().isControlFlowMove()) {
                    return delaySlots_ + 1;
                }
            }
            return 1;
        }
        case DataDependenceEdge::DEP_WAR: {
            return 0; // can be scheduled to same cycle
        }
        default: {
            return 1;
        }
        }
    }
    default:
        return 1; // can be scheduled to the next cycle (default)
    }
}

/**
 * Sets a machine into DDG. 
 *
 * This machine is used to some heuristics and helper functions that 
 * selector uses, for example path length calculation and earliestCycle.
 *
 * If no machine is set, these functions will still work but will
 * give non-optimal results.
 *
 * @param machine machine to be used for heristics
 */
void
DataDependenceGraph::setMachine(const TTAMachine::Machine& machine) {

    if (machine_ == &machine) {
        return;
    }

    machine_ = &machine;
    delaySlots_ = machine.controlUnit()->delaySlots();

    const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
        machine.functionUnitNavigator();

    operationLatencies_.clear();
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int j = 0; j < fu->operationCount(); j++) {
            TTAMachine::HWOperation* hwop = fu->operation(j);
            int latency = hwop->latency();
            TCEString name = StringTools::stringToUpper(hwop->name());
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
    loopingSinkDistances_.clear();
    loopingSourceDistances_.clear();
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

    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    NodeDescriptor nd = descriptor(node);
    std::pair<InEdgeIter, InEdgeIter> edges =
        boost::in_edges(nd, graph_);

    bool srcOp = node.isSourceOperation();
    bool dstOp = node.isDestinationOperation();

    for (InEdgeIter ei = edges.first; ei != edges.second; ei++) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge& edge = *graph_[ed];
        if (edge.isBackEdge()) {
            continue;
        }
        const MoveNode& m = *graph_[boost::source(ed, graph_)];

        if (srcOp) {
            // operand move of same operation
            if ((m.isDestinationOperation() && 
                 &node.sourceOperation() == &m.destinationOperation()) ||
                (m.isSourceOperation() &&
                 &node.sourceOperation() == &m.sourceOperation())) {
                continue;
            }
        }

        if (dstOp && m.isDestinationOperation() &&
            &node.destinationOperation() == &m.destinationOperation()) {
            continue;
        } 
        if (!m.isScheduled()) {
            return false;
        }
    }
    return true;

}

/**
 * Checks whether the given node has all its successors scheduled.
 *
 * Ignores intra operation edges, thus if the successor belongs to the
 * same operation, it need not be scheduled for the node to be considered
 * ready.
 *
 * @return True in case all successors are scheduled.
 */
bool
DataDependenceGraph::successorsReady(MoveNode& node) const {
    
    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    NodeDescriptor nd = descriptor(node);
    std::pair<OutEdgeIter, OutEdgeIter> edges =
    boost::out_edges(nd, graph_);
    
    for (OutEdgeIter ei = edges.first; ei != edges.second; ei++) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge& edge = *graph_[ed];
        if (edge.isBackEdge()) {
            continue;
        }
        const MoveNode& m = *graph_[boost::target(ed, graph_)];
        
        const bool operandMoveOfSameOperation =
        (node.isDestinationOperation() && m.isSourceOperation() && 
         &node.destinationOperation() == &m.sourceOperation());
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
 * If latency is not known (no machine is given) does some simple
 * heuristics.
 *
 * This function should propably be on somewhere else.
 *
 * @param op operation whose minimum latency is being searched
 * @return minimum latency of given operation. 
 */
int 
DataDependenceGraph::getOperationLatency(const TCEString& name) const {
    std::map<TCEString, int>::const_iterator iter =
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
    const MoveNode& tailNode, const MoveNode& headNode, 
    const DataDependenceEdge& edge) 
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
    NodeSet& nodes, bool includeLoops) {
    DataDependenceGraph* subGraph = 
        new DataDependenceGraph(
            allParamRegs_, "", registerAntidependenceLevel_, NULL, false,
            !includeLoops);

    if (machine_ != NULL) {
        subGraph->setMachine(*machine_);
    }

    constructSubGraph(*subGraph, nodes);

    typedef std::set<ProgramOperationPtr, ProgramOperationPtrComparator> POSet;
    POSet subgraphPOs;

    // copy the node -> bbn mapping.
    // also find POs to copy.
    for (int i = 0, nc = subGraph->nodeCount(); i < nc; i++) {
        MoveNode& mn = subGraph->node(i);
        BasicBlockNode* bbn = moveNodeBlocks_[&mn];
        subGraph->moveNodeBlocks_[&mn] = bbn;
        if (mn.isSourceOperation()) {
            subgraphPOs.insert(mn.sourceOperationPtr());
        }
        
        if (mn.isDestinationOperation()) {
            subgraphPOs.insert(mn.destinationOperationPtr());
        }
    }
    for (POSet::iterator i = subgraphPOs.begin(); 
         i != subgraphPOs.end();i++) {
        subGraph->programOperations_.push_back(*i);
    }
    return subGraph;
}

/**
 * Returns a version of the graph with only true dependence edges.
 */
DataDependenceGraph*
DataDependenceGraph::trueDependenceGraph(
    bool removeMemAntideps, bool ignoreMemDeps) {
    DataDependenceGraph* subGraph = 
        new DataDependenceGraph(
            allParamRegs_, "true DDG", registerAntidependenceLevel_,
            NULL, false, false);
    NodeSet nodes;
    for (int i = 0; i < nodeCount(); ++i) {
        nodes.insert(&node(i));
    }
    if (machine_ != NULL)
        subGraph->setMachine(*machine_);

    constructSubGraph(*subGraph, nodes);
    
    for (int i = 0; i < nodeCount(); i++) {
        MoveNode& tail = node(i);
        for (int e = 0; e < subGraph->outDegree(tail);) {
            DataDependenceEdge& edge = subGraph->outEdge(tail,e);
            if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                isNotAvoidable(edge)) {
                e++;
                continue;
            }
            // consider the memory dependencies
            if (edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY &&
                (ignoreMemDeps || (edge.isFalseDep() && removeMemAntideps))) {
                subGraph->dropEdge(edge);
            } else {
                e++;
            }
        }
    }
    return subGraph;
}

/**
 * Returns a version of the graph with only nodes that are in the
 * critical path.
 */
DataDependenceGraph*
DataDependenceGraph::criticalPathGraph() {
    DataDependenceGraph* subGraph = 
        new DataDependenceGraph(
            allParamRegs_, "critical path DDG",
            registerAntidependenceLevel_, NULL, false, false);

    if (machine_ != NULL)
        subGraph->setMachine(*machine_);
    
    NodeSet nodes;
    for (int i = 0; i < nodeCount(); ++i) {
        if (isInCriticalPath(node(i)))
            nodes.insert(&node(i));
    }
    constructSubGraph(*subGraph, nodes);

    return subGraph;
}

/**
 * Returns a version of the graph with only nodes and edges that 
 * present memory dependencies.
 */
DataDependenceGraph*
DataDependenceGraph::memoryDependenceGraph() {
    DataDependenceGraph* subGraph = 
        new DataDependenceGraph(
            allParamRegs_, "memory DDG",
            registerAntidependenceLevel_, NULL, false, false);

    if (machine_ != NULL)
        subGraph->setMachine(*machine_);
    
    NodeSet nodes;
    for (int i = 0; i < nodeCount(); ++i) {

        MoveNode& mn = node(i);
        EdgeSet outEdg = outEdges(mn);
        bool found = false;
        for (EdgeSet::iterator ei = outEdg.begin(); ei != outEdg.end(); ei++) {
            Edge& edge = **ei;
            if (edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY) {
                nodes.insert(&node(i));
                found = true;
                break;
            }
        }

        if (found) continue;
        EdgeSet inEdg = inEdges(mn);
        for (EdgeSet::iterator ei = inEdg.begin(); ei != inEdg.end(); ei++) {
            Edge& edge = **ei;
            if (edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY) {
                nodes.insert(&node(i));
                break;
            }
        }

    }
    constructSubGraph(*subGraph, nodes);

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
    TTAProgram::CodeSnippet& cs, bool includeLoops) {
    NodeSet moveNodes;
    int nc = nodeCount();
    for (int i = 0; i < nc; i++) {
        MoveNode& mn = node(i,false);
        if (mn.isMove()) {
            TTAProgram::Move& move = mn.move();
            if (move.isInInstruction()) {
                TTAProgram::Instruction& parentIns = move.parent();
                if (parentIns.isInProcedure()) { // is in code snippet
                    if (&parentIns.parent() == &cs) {
                        moveNodes.insert(&mn);
                    }
                }
            } 
        }
    }
    return createSubgraph(moveNodes, includeLoops);
}

/**
 * Creates a subgraph of a ddg from set of code snippets
 * which contains instructions which contains moves
 * in this graph.
 *
 * @param codeSnippets code being included in the subgraph
 * @param includeLoops whether to include loop-carried dependencies
 * @return the created subgraph
 */
DataDependenceGraph*
DataDependenceGraph::createSubgraph(
    std::list<TTAProgram::CodeSnippet*>& codeSnippets, bool includeLoops) {
    NodeSet moveNodes;

    int nc = nodeCount();
    for (int i = 0; i < nc; i++) {
        MoveNode& mn = node(i,false);
        if (mn.isMove()) {
            TTAProgram::Move& move = mn.move();
            TTAProgram::Instruction& parentIns = move.parent();
            if (parentIns.isInProcedure()) { // is in code snippet
                for (std::list<TTAProgram::CodeSnippet*>::iterator iter = 
                         codeSnippets.begin(); 
                     iter != codeSnippets.end(); iter++) {
                    TTAProgram::CodeSnippet& cs = **iter;
                    if (&move.parent().parent() == &cs) {
                        moveNodes.insert(&mn);
                    }
                }
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
DataDependenceGraph::nodeOfMove(const TTAProgram::Move& move) {

    auto i = nodesOfMoves_.find(&move);
    if (i != nodesOfMoves_.end()) {
        return *(i->second);
    } 

    TCEString msg = "move not in ddg: " + 
        Conversion::toString(reinterpret_cast<long>(&move)) + " " + 
        POMDisassembler::disassemble(move);
    throw InstanceNotFound(__FILE__,__LINE__,__func__, msg);
}

/**
 * Drops loop edges from a sub-DDG.
 *
 * This works only with simple control structures;
 * Only works for edges marked as loop edges. 
 * Currently loop edges spanning over multiple basic blocks may be missing.
 */
void DataDependenceGraph::dropBackEdges() {
    const int nc = nodeCount();

    // first loop thru all nodes.
    for (int n = 0; n < nc; n++) {
        NodeDescriptor nd = boost::vertex(n, graph_);

        // the thru all output edges of the node.
        // this is propably the fastest way to iterate over 
        // all edges of a graph. (or is just all edges faster?)
        std::pair<OutEdgeIter, OutEdgeIter> edges = 
            boost::out_edges(nd, graph_);

        for (OutEdgeIter ei = edges.first; ei != edges.second;) {
            DataDependenceEdge* e = graph_[(*ei)];
            if (e->isBackEdge()) {
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
        return static_cast<DataDependenceGraph*>(parentGraph_)
            ->isRootGraphProcedureDDG();
    }
}

/**
 * Addds inter-BB-Anti edges between the the basic blocks. 
 *
 * currently quite a heavy routine
 * 
 * @param bbn1 first basic block, executed first, sources of antidependence 
 *             edges
 * @param bbn2 second basic block, executed later, targets of antidependence 
 *             edges
 */
void
DataDependenceGraph::fixInterBBAntiEdges(
    BasicBlockNode& bbn1, BasicBlockNode& bbn2, bool loopEdges) {
    std::map<TCEString, TTAProgram::Move*> firstWrites2;
    std::map<TCEString, TTAProgram::Move*> lastReads1;
    std::map<TCEString, TTAProgram::Move*> lastWrites1;
    std::map<TCEString, TTAProgram::Move*> lastGuards1;

    TTAProgram::BasicBlock& bb1 = bbn1.basicBlock();
    TTAProgram::BasicBlock& bb2 = bbn2.basicBlock();

    // find the first writes in the next BB.
    for (int i2 = 0; i2 < bb2.instructionCount(); i2++) {
        TTAProgram::Instruction& ins = bb2.instructionAtIndex(i2);
        for (int j2 = 0; j2 < ins.moveCount(); j2++) {
            TTAProgram::Move& move = ins.move(j2);
            TTAProgram::Terminal& dest = move.destination();
            if (dest.isGPR()) {
                TCEString reg2 = DisassemblyRegister::registerName(dest);

                std::map<TCEString, TTAProgram::Move*>::iterator iter2 = 
                    firstWrites2.find(reg2);
                if (iter2 == firstWrites2.end()) {
                    firstWrites2[reg2] = &move;
                }
            }
        }
    }

    // find the last reads, writes and guard uses from first BB.
    for (int i1 = bb1.instructionCount()-1; i1 >= 0 ; i1--) {
        TTAProgram::Instruction& ins = bb1.instructionAtIndex(i1);
        for (int j1 = 0; j1 < ins.moveCount(); j1++) {
            TTAProgram::Move& move = ins.move(j1);
            TTAProgram::Terminal& dest = move.destination();
            TTAProgram::Terminal& src = move.source();
            // Writes for WaWs
            if (dest.isGPR()) {
                TCEString reg1 = DisassemblyRegister::registerName(dest);

                std::map<TCEString, TTAProgram::Move*>::iterator iter1 = 
                    lastWrites1.find(reg1);
                if (iter1 == lastWrites1.end()) {
                    lastWrites1[reg1] = &move;
                }
            }
            // reads for WaRs
            if (src.isGPR()) {
                TCEString reg1 = DisassemblyRegister::registerName(src);

                std::map<TCEString, TTAProgram::Move*>::iterator iter1 = 
                    lastReads1.find(reg1);
                if (iter1 == lastReads1.end()) {
                    lastReads1[reg1] = &move;
                }
            }
            // guard uses
            if (!move.isUnconditional()) {
                const TTAMachine::Guard& g = move.guard().guard();
                const TTAMachine::RegisterGuard* rg =
                    dynamic_cast<const TTAMachine::RegisterGuard*>(&g);
                if (rg != NULL) {
                    TCEString reg1 = DisassemblyRegister::registerName(
                        *rg->registerFile(), rg->registerIndex());
                    
                    std::map<TCEString, TTAProgram::Move*>::iterator iter2 = 
                        lastGuards1.find(reg1);
                    if (iter2 == lastGuards1.end()) {
                        lastGuards1[reg1] = &move;
                    }
                }
            }
        }
    }

    // then go thru them
    for (std::map<TCEString, TTAProgram::Move*>::iterator iter2 = 
             firstWrites2.begin();
         iter2 != firstWrites2.end(); iter2++) {
        const TCEString& reg2 = iter2->first;
        MoveNode& mn2 = nodeOfMove(*(iter2->second));
        
        // WaRs
        TTAProgram::Move* move1 = lastReads1[reg2];
        if (move1 != NULL) {
            DataDependenceEdge* edge = 
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, reg2, false, false, false,
                    false, loopEdges);
            connectOrDeleteEdge(nodeOfMove(*move1), mn2, edge);
        }
        
        // WaWs
        move1 = lastWrites1[reg2];
        if (move1 != NULL) {
            DataDependenceEdge* edge = 
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW, reg2, false, false, false,
                    false, loopEdges);
            connectOrDeleteEdge(nodeOfMove(*move1), mn2, edge);
        }
        
        // guard WaRs
        move1 = lastGuards1[reg2];
        if (move1 != NULL) {
            DataDependenceEdge* edge = 
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, reg2, true, false, false, 
                    false, loopEdges);
            connectOrDeleteEdge(nodeOfMove(*move1), mn2, edge);
        }
    }
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
    const MoveNode& src, MoveNode& dst, bool ignoreSameBBBackedges,
    bool moveOverLoopEdge) {
    // performance optimization 
    NodeDescriptor nd = descriptor(src);

    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<InEdgeIter, InEdgeIter> iEdges =
        boost::in_edges(nd, graph_);

    for (InEdgeIter ii = iEdges.first; ii != iEdges.second; ii++) {
        EdgeDescriptor ed = *ii;
        DataDependenceEdge* edge = graph_[ed];
        MoveNode* tail = graph_[boost::source(ed, graph_)];
        if (ignoreSameBBBackedges && edge->isBackEdge() &&
            &getBasicBlockNode(src) == &getBasicBlockNode(*tail)) {
            continue;
        }
        DataDependenceEdge* newEdge = new DataDependenceEdge(
            *edge, (moveOverLoopEdge && tail != &src && tail != &dst));
        connectNodes(*tail, dst, *newEdge);
    }

    std::pair<OutEdgeIter, OutEdgeIter> oEdges =
        boost::out_edges(nd, graph_);

    for (OutEdgeIter oi = oEdges.first; oi != oEdges.second; oi++) {
        EdgeDescriptor ed = *oi;
        DataDependenceEdge* edge = graph_[ed];
        MoveNode* head = graph_[boost::target(ed, graph_)];
        if (ignoreSameBBBackedges && edge->isBackEdge() &&
            &getBasicBlockNode(src) == &getBasicBlockNode(*head)) {
            continue;
        }
        DataDependenceEdge* newEdge = new DataDependenceEdge(
            *edge, (moveOverLoopEdge && head != &src && head != &dst));
        connectNodes(dst, *head, *newEdge);
    }
}

/**
 * Copies all incoming guard dependencies going to a movenode to another
 * 
 * @param src movenode to copy dependencies from
 * @param dst movenode to copy dependencies to
 */
void 
DataDependenceGraph::copyIncomingGuardEdges(
    const MoveNode& src, MoveNode& dst) {

    // performance optimization 
    NodeDescriptor nd = descriptor(src);

    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<InEdgeIter, InEdgeIter> iEdges =
        boost::in_edges(nd, graph_);

    for (InEdgeIter ii = iEdges.first; ii != iEdges.second; ii++) {
        EdgeDescriptor ed = *ii;
        DataDependenceEdge* edge = graph_[ed];
        if (edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge->dependenceType() == DataDependenceEdge::DEP_RAW &&
            edge->guardUse()) {
            DataDependenceEdge* newEdge = new DataDependenceEdge(*edge);
            MoveNode& tail = *graph_[boost::source(ed, graph_)];
            connectNodes(tail, dst, *newEdge);
        }
    }
}

/**
 * Copies all outgoing guard war dependencies going to a movenode to another
 * 
 * @param src movenode to copy dependencies from
 * @param dst movenode to copy dependencies to
 */
void 
DataDependenceGraph::copyOutgoingGuardWarEdges(
    const MoveNode& src, MoveNode& dst) {

    // performance optimization 
    NodeDescriptor nd = descriptor(src);

    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<OutEdgeIter, OutEdgeIter> oEdges =
        boost::out_edges(nd, graph_);

    for (OutEdgeIter oi = oEdges.first; oi != oEdges.second; oi++) {
        EdgeDescriptor ed = *oi;
        DataDependenceEdge* edge = graph_[ed];
        if (edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            edge->dependenceType() == DataDependenceEdge::DEP_WAR &&
            edge->guardUse()) {
            DataDependenceEdge* newEdge = new DataDependenceEdge(*edge);
            MoveNode& head = *graph_[boost::target(ed, graph_)];
            connectNodes(dst, head, *newEdge);
        }
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
    int count = 0;
    EdgeSet oEdges = outEdges(mn);
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
 * Calculates number of register RAW dependecies originating from 
 * given node
 *
 * @param mn Movenodes whose dependencies to calculate
 * @param onlySchedules only care about dependencies to Scheduled nodes.
 * @return number of register RAW dependencies originating
 * from given node
 */
int DataDependenceGraph::regRawSuccessorCount(
    const MoveNode& mn, bool onlyScheduled) {
    int count = 0;
    EdgeSet oEdges = outEdges(mn);
    for (EdgeSet::iterator iter = 
             oEdges.begin(); iter != oEdges.end(); iter++) {
        if ((*iter)->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (*iter)->dependenceType() == DataDependenceEdge::DEP_RAW) {
            MoveNode& mn = headNode(**iter);
            if (!onlyScheduled || mn.isScheduled()) {
                count++;
            }
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

bool DataDependenceGraph::rWawRawEdgesOutUncond(MoveNode& mn) {
    EdgeSet oEdges = outEdges(mn);
    for (EdgeSet::iterator iter = 
             oEdges.begin(); iter != oEdges.end(); iter++) {
        if ((*iter)->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (((*iter)->dependenceType() == DataDependenceEdge::DEP_RAW) ||
            ((*iter)->dependenceType() == DataDependenceEdge::DEP_WAW))) {
            MoveNode& head = headNode(**iter);
            if (head.move().isUnconditional()) {
                return true;
            }
        }
    }
    return false;

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
    const MoveNode& mn) const {
    DataDependenceEdge* guard = NULL;
    DataDependenceGraph::EdgeSet iEdges = inEdges(mn);

    for (DataDependenceGraph::EdgeSet::iterator i = iEdges.begin();
         i != iEdges.end(); i++) {
        DataDependenceEdge* iEdge = *i;
        if (iEdge->guardUse() && iEdge->dependenceType() == 
            DataDependenceEdge::DEP_RAW) {
            if (guard == NULL) {
                guard = iEdge;
            } else {
                if (!(*guard == *iEdge) || 
                    &tailNode(*iEdge) != &tailNode(*guard)) {
                    return NULL; // too complicated guard
                }
            }
        }
    }
    return guard;
}

DataDependenceGraph::NodeSet 
DataDependenceGraph::guardRawPredecessors(const MoveNode& node) const {
    NodeSet preds;
    DataDependenceGraph::EdgeSet iEdges = inEdges(node);
    for (DataDependenceGraph::EdgeSet::iterator i = iEdges.begin();
         i != iEdges.end(); i++) {
        DataDependenceEdge* iEdge = *i;
        if (iEdge->guardUse() && iEdge->dependenceType() == 
            DataDependenceEdge::DEP_RAW) {
            preds.insert(&tailNode(*iEdge));
        }
    }
    return preds;
}


/**
 * Returns the source of only ordinary register RAW edge to given node,
 * ie the only move which writes the value this move reads.
 *
 * backEdges: 0: do not care
 *            1: only backedges
 *            2: no backedges
 *
 * guardEdges: 0: do not care
 *             1: only guard edges
 *             2: no guard edges
 *
 * if there are multiple nodes where the value may come, return NULL,
 * or if none found.
 * 
 * @param mn MoveNode whose predecessor noves we are searching.
 * @return only move writing reg this reads or NULL if no or multiple.
 */

MoveNode* 
DataDependenceGraph::onlyRegisterRawSource(
    const MoveNode& mn, int guardEdges, int backEdges)
    const {
    MoveNode* source = NULL;
    NodeDescriptor nd = descriptor(mn);
    
    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<InEdgeIter, InEdgeIter> edges =
        boost::in_edges(nd, graph_);

    for (InEdgeIter ei = edges.first; ei != edges.second; ei++) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge* edge = graph_[ed];
        if (backEdges == 1 && !edge->isBackEdge()) continue;
        if (backEdges == 2 && edge->isBackEdge()) continue;
        if (guardEdges == 1 && !edge->guardUse()) continue;
        if (guardEdges == 2 && edge->guardUse()) continue;
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW &&
            edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !edge->headPseudo() && !edge->tailPseudo()) {
            if (source == NULL) {
                source = graph_[boost::source(ed, graph_)];
            } else {
                return NULL;
            }
        }
    }
    return source;
}

/**
 * Returns the destinations of ordinary register RAW edge to given node,
 * ie the moves which reads the value this move writes.
 *
 * @param mn MoveNode whose succssor moves we are searching.
 * @return only move reading reg this writes or NULL if no or multiple.
 */

std::map<DataDependenceEdge*, MoveNode*, DataDependenceEdge::Comparator>
DataDependenceGraph::onlyRegisterRawDestinationsWithEdges(
    const MoveNode& mn, bool allowBackEdges) const {
    std::map<DataDependenceEdge*, MoveNode*, DataDependenceEdge::Comparator>
        destination;
    NodeDescriptor nd = descriptor(mn);

    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<OutEdgeIter, OutEdgeIter> edges =
        boost::out_edges(nd, graph_);

    for (OutEdgeIter ei = edges.first; ei != edges.second; ei++) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge* edge = graph_[ed];
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW &&
            edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !edge->tailPseudo()) {
            if (!edge->headPseudo() &&
                (allowBackEdges || !edge->isBackEdge())) {
                destination.insert(
                    std::make_pair(edge, graph_[boost::target(ed, graph_)]));
            } else {
                destination.clear();
                break;
            }
        }
    }
    return destination;
}

/**
 * Returns the destinations of ordinary register RAW edge to given node,
 * ie the moves which reads the value this move writes.
 *
 * @param mn MoveNode whose succssor moves we are searching.
 * @return only move reading reg this writes or NULL if no or multiple.
 */

DataDependenceGraph::NodeSet
DataDependenceGraph::onlyRegisterRawDestinations(
    const MoveNode& mn, bool allowGuardEdges, bool allowBackEdges) const {
    NodeSet destination;
    NodeDescriptor nd = descriptor(mn);
    
    // use the internal data structures to make this fast.
    // edgeset has too much set overhead,
    // inedge(n,i) is O(n^2) , this is linear with no overhead
    std::pair<OutEdgeIter, OutEdgeIter> edges =
        boost::out_edges(nd, graph_);

    for (OutEdgeIter ei = edges.first; ei != edges.second; ei++) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge* edge = graph_[ed];
        if (edge->dependenceType() == DataDependenceEdge::DEP_RAW &&
            edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (allowGuardEdges || !edge->guardUse())
            && !edge->tailPseudo()) {
            if (!edge->headPseudo() && (allowBackEdges || !edge->isBackEdge())) {
                destination.insert(graph_[boost::target(ed, graph_)]);
            } else {
                destination.clear();
                break;
            }
        }
    }
    return destination;
}

/**
 * Tracks the origin of the data a movenode reads. Goes back thru DDG
 * and tracks register-to register reads. 
 *
 * If data comes from operation, returns the operation result read. 
 * If data comes from stack pointer, returns the stack pointer read.
 * If the data may come from multiple places (conditional writes to reg,
 * or writes to same reg in multiple BB's), return the move which read the 
 * value from the reg.
 */
const MoveNode* 
DataDependenceGraph::onlyRegisterRawAncestor(
    const MoveNode& mn, const std::string& sp) const {
    const MoveNode* node = &mn;

    while(true) {
        if (node->isSourceReg(sp)) {
            return node;
        }
        MoveNode* src = onlyRegisterRawSource(*node);
        if (src == NULL) {
            return node;
        } else {
            // this should not be needed if incoming code were sensible
            // but it often is not. extra check to prevetn forever loop.
            if (node == src) {
                return node;
            }
            node = src;
        }
    } 
}


/**
 * Returns the register war successors of the given node.
 *
 * If node has no register war successors, an empty set is returned. 
 * Note: the node can also be a successor of itself. 
 * Register war successor means successor which is
 * connected by register or ra war edge
 *
 * @param node The node of which successors to find.
 * @return Set of root nodes, can be empty. 
 */

DataDependenceGraph::NodeSet
DataDependenceGraph::regWarSuccessors(const MoveNode& node) const {
    
    NodeSet succ;

    NodeDescriptor nd = descriptor(node);
    EdgeSet out = outEdges(node);
    typedef EdgeSet::iterator EdgeIterator;

    for (EdgeIterator i = out.begin(); i != out.end(); ++i) {
        DataDependenceEdge& e = **i;
        if (e.dependenceType() == DataDependenceEdge::DEP_WAR &&
            (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
             e.edgeReason() == DataDependenceEdge::EDGE_RA)) {
            succ.insert(&headNode(**i, nd));
        }
    }
    
    return succ;
}

/**
 * Returns the register raw successors of the given node.
 *
 * If node has no register war successors, an empty set is returned. 
 * Note: the node can also be a successor of itself. 
 * Register war successor means successor which is
 * connected by register or ra war edge
 *
 * @param node The node of which successors to find.
 * @return Set of root nodes, can be empty. 
 */

DataDependenceGraph::NodeSet
DataDependenceGraph::regRawSuccessors(const MoveNode& node) const {
    
    NodeSet succ;

    NodeDescriptor nd = descriptor(node);
    EdgeSet out = outEdges(node);
    typedef EdgeSet::iterator EdgeIterator;

    for (EdgeIterator i = out.begin(); i != out.end(); ++i) {
        DataDependenceEdge& e = **i;
        if (e.dependenceType() == DataDependenceEdge::DEP_RAW &&
            (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
             e.edgeReason() == DataDependenceEdge::EDGE_RA)) {
            succ.insert(&headNode(**i, nd));
        }
    }
    
    return succ;
}

/**
 * Returns the register waw successors of the given node.
 *
 * If node has no reg waw successors, an empty set is returned. 
 * Note: the node can also be a successor of itself. 
 * register waw successor means successor which is
 * connected by register waw edge.
 *
 * @param node The node of which successors to find.
 * @return Set of root nodes, can be empty. 
 */

DataDependenceGraph::NodeSet
DataDependenceGraph::regWawSuccessors(const MoveNode& node) const {
    
    NodeSet succ;

    NodeDescriptor nd = descriptor(node);
    EdgeSet out = outEdges(node);
    typedef EdgeSet::iterator EdgeIterator;

    for (EdgeIterator i = out.begin(); i != out.end(); ++i) {
        DataDependenceEdge& e = **i;

        // if we would bypass also over ra, then allow also ra in following
        if (e.dependenceType() == DataDependenceEdge::DEP_WAW &&
            e.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {
            succ.insert(&headNode(**i, nd));
        }
    }
    
    return succ;
}


/**
 * Creates antidependencies between certains nodes of a ddg.
 * 
 * This is used after regcopyadded to insert antideps between the
 * temp registers added by the regcopyadder.
 *
 * All nodes given to the method should be scheduled.
 * The nodes should have already been added to the graph.
 *
 * @param nodes nodes whose antidependencies to add.
 */
void
DataDependenceGraph::createRegisterAntiDependenciesBetweenNodes(
    DataDependenceGraph::NodeSet& nodes) {
    
    typedef std::map<int, NodeSet > 
        MovesByCycles;
    MovesByCycles movesInCycles;
    typedef std::map<TCEString, NodeSet> NodeSetMap;
    
    NodeSetMap reads;
    NodeSetMap writes;
    // insert all moves int correct cycle in bookkeeping
    for (NodeSet::iterator nsIter = nodes.begin(); 
         nsIter != nodes.end(); nsIter++) {
        MoveNode* mn = *nsIter;
        assert(mn->isScheduled());
        movesInCycles[mn->cycle()].insert(mn);
    }

    // loop thru all cycles
    for (MovesByCycles::iterator i = movesInCycles.begin(); 
         i != movesInCycles.end(); i++) {
        
        NodeSet& nodesInCycle = i->second;
        // then check sources of all nodes in the cycle
        for (NodeSet::iterator j = nodesInCycle.begin();
             j != nodesInCycle.end(); j++) {
            MoveNode* mn = *j;
            TTAProgram::Move& move = mn->move();
            TTAProgram::Terminal& src = move.source();
            if (src.isGPR()) {
                TCEString reg = DisassemblyRegister::registerName(src);
                reads[reg].insert(mn);
            }
        }

        // then check destinations of all nodes in the cycle
        for (NodeSet::iterator j = nodesInCycle.begin();
             j != nodesInCycle.end(); j++) {

            MoveNode* mn = *j;
            TTAProgram::Move& move = mn->move();
            TTAProgram::Terminal& dst = move.destination();
            if (dst.isGPR()) {
                TCEString reg = DisassemblyRegister::registerName(dst);

                NodeSet& readsFromReg = reads[reg];
                NodeSet& writesToReg = writes[reg];
                
                // create the dependencies.
                // WaRs
                for (NodeSet::iterator k = readsFromReg.begin(); 
                     k != readsFromReg.end(); k++) {
                    MoveNode& prevMN = **k;
                    if (!exclusingGuards(prevMN, *mn)) {
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                DataDependenceEdge::EDGE_REGISTER,
                                DataDependenceEdge::DEP_WAR, reg);
                        connectOrDeleteEdge(**k, *mn, edge);
                    }
                }

                // WaWs.
                for (NodeSet::iterator k = writesToReg.begin(); 
                     k != writesToReg.end(); k++) {
                    MoveNode& prevMN = **k;
                    if (!exclusingGuards(prevMN, *mn)) {
                        DataDependenceEdge* edge = 
                            new DataDependenceEdge(
                                DataDependenceEdge::EDGE_REGISTER,
                                DataDependenceEdge::DEP_WAW, reg);
                        connectOrDeleteEdge(**k, *mn, edge);
                    }
                }

                // if this is unconditional, kills previous writes to the reg
                if (move.isUnconditional()) {
                    writes[reg].clear();
                    reads[reg].clear();
                }
                writes[reg].insert(mn);
            }
        }
    }
}



/* 
 * Checks whether two movenodes have exclusive guard, ie
 * same guard but inverted on one of them.
 *
 * If not sure returns false
 * 
 * @param mn1 first movenode to check.
 * @param mn2 second movenode to check.
 * @return true if they have same guard inverted, false otherwise.
 */
bool
DataDependenceGraph::exclusingGuards(
    const MoveNode& mn1, const MoveNode& mn2) const {
    if (!mn1.isMove() || !mn2.isMove()) {
        return false;
    }
    const TTAProgram::Move& move1 = mn1.move();
    const TTAProgram::Move& move2 = mn2.move();
    if (move1.isUnconditional() || move2.isUnconditional()) {
        return false;
    }
    TTAProgram::MoveGuard& mg1 = move1.guard();
    TTAProgram::MoveGuard& mg2 = move2.guard();
    if (mg1.isInverted() == mg2.isInverted()) {
        return false;
    }
    NodeSet incomingGuards1 =
        guardRawPredecessors(mn1);

    NodeSet incomingGuards2 = 
        guardRawPredecessors(mn2);

    return !incomingGuards1.empty() && 
        incomingGuards1 == incomingGuards2;
}

bool
DataDependenceGraph::sameGuards(
    const MoveNode& mn1, const MoveNode& mn2) const {
    if (!mn1.isMove() || !mn2.isMove()) {
        return false;
    }
    const TTAProgram::Move& move1 = mn1.move();
    const TTAProgram::Move& move2 = mn2.move();

    if (move1.isUnconditional() && move2.isUnconditional()) {
        return true;
    }

    if (move1.isUnconditional() || move2.isUnconditional()) {
        return false;
    }
    TTAProgram::MoveGuard& mg1 = move1.guard();
    TTAProgram::MoveGuard& mg2 = move2.guard();
    if (mg1.isInverted() != mg2.isInverted()) {
        return false;
    }
    NodeSet incomingGuards1 =
        guardRawPredecessors(mn1);

    NodeSet incomingGuards2 = 
        guardRawPredecessors(mn2);

    return !incomingGuards1.empty() && 
        incomingGuards1 == incomingGuards2;
}

/**
 * Checks whether guards allow bypassing.
 * 
 * @param defNode node defining a value, bypass source.
 * @param useNode node using the value, node being bypassed.
 * @return true if guards do not prevent bypassing.
 */
bool DataDependenceGraph::guardsAllowBypass(
    const MoveNode& defNode, const MoveNode& useNode, bool loopBypass) {
    
    // can always bypass from unconditional
    if (defNode.move().isUnconditional()) {
        return true;
    }

    // cannot bypass from conditional to unconditional
    if (useNode.move().isUnconditional()) {
        return false;
    }

    NodeSet defGuardDefMoves = guardDefMoves(defNode);
    NodeSet useGuardDefMoves = guardDefMoves(useNode);
    
    // guard value changed between iterations, cannot trust same src!
    // could check the backedge property from the guard edges,
    // this is suboptimal and safe
    if (loopBypass && !defGuardDefMoves.empty()) {
        return false;
    }

    // if guards defined in different place, 
    // we do now know if they are equal.
    if (defGuardDefMoves != useGuardDefMoves) {
        return false;
    }

    // if guards are equal, bypass allowed.
    return defNode.move().guard().guard().isEqual(
        useNode.move().guard().guard());
}

/**
 * Finds (only) node which writes the guard value of a move.
 * If none or multiple, return NULL
 */
MoveNode* DataDependenceGraph::onlyGuardDefOfMove(MoveNode& moveNode) {
    NodeSet guardDefs = guardDefMoves(moveNode);
    if (guardDefs.size() != 1) {
        return NULL;
    }
    return *guardDefs.begin();
}

/**
 * Returns true if the node writes a value to a predicate register
 * which is used as a guard of a jump.
 */
bool DataDependenceGraph::writesJumpGuard(const MoveNode& moveNode) {
    NodeSet succs = successors(moveNode);
    for (auto n: succs) {
        if (n->move().isJump()) {
            if (onlyGuardDefOfMove(*n) == &moveNode) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Finds the move which writes the loop limit comparison value into
 * the loop comparison operation.
 *
 * If cannot find, returs null.
 *
 * Assumes positively-gworing loop with ne (or eq + xor 1) as end condition.
 * (llvm creates this kind of loops)
 *
 * @param jumpMove jump move of a loop.
 * @return loop limit move or NULL of not found.
 */
TTAProgram::Move* 
DataDependenceGraph::findLoopLimit(MoveNode& jumpMove) {
    
    NodeSet guardDefs = guardDefMoves(jumpMove);
    if (guardDefs.size() != 1) {
        return NULL;
    }
    MoveNode* guardDef = *guardDefs.begin();
    while (guardDef->isSourceVariable()) {
        guardDef = onlyRegisterRawSource(*guardDef);
        if (guardDef == NULL) {
            return NULL;
        }
    }
    if (guardDef->isSourceOperation()) {
        ProgramOperation* gdpo = &guardDef->sourceOperation();
        bool inverted = jumpMove.move().guard().isInverted();

        // machine does not have ne operation so eq + xor used.
        if (gdpo->operation().name() == "XOR") {
            MoveNodeSet& input1Set = gdpo->inputNode(1);
            MoveNodeSet& input2Set = gdpo->inputNode(2);
            if (input1Set.count() != 1 || input2Set.count() != 1) {
                return NULL;
            }
            MoveNode& input1 = input1Set.at(0);
            MoveNode& input2 = input2Set.at(0);

            MoveNode *regMove = NULL;
            if (input1.move().source().isGPR() && 
                input2.move().source().isImmediate()) {
                regMove = &input1;
            } 

            if (input2.move().source().isGPR() && 
                input1.move().source().isImmediate()) {
                regMove = &input2;
            }

            // bypassed directly from eq?
            if (input1.isSourceOperation() &&
                input2.move().source().isImmediate()) {
                gdpo = &input1.sourceOperation();
            } else {
                if (input2.isSourceOperation() &&
                    input1.move().source().isImmediate()) {
                    gdpo = &input2.sourceOperation();
                } else { // not bypassed.
                    if (regMove != NULL) {
                        MoveNode* xorSrc = onlyRegisterRawSource(*regMove);
                        if (xorSrc != NULL && xorSrc->isSourceOperation()) {
                            inverted = !inverted;
                            gdpo = &xorSrc->sourceOperation();
                        } else {
                            return NULL;
                        }
                    } else {
                        return NULL;
                    }
                }
            }
        }

        if ((gdpo->operation().name() == "NE" && !inverted) ||
            (gdpo->operation().name() == "EQ"  && inverted)) {
            MoveNodeSet& input1Set = gdpo->inputNode(1);
            MoveNodeSet& input2Set = gdpo->inputNode(2);
            if (input1Set.count() != 1 || input2Set.count() != 1) {
                return NULL;
            }
            MoveNode& input1 = input1Set.at(0);
            MoveNode& input2 = input2Set.at(0);
            if ((input1.move().source().isGPR() ||
                 input1.isSourceOperation()) && 
                input2.move().source().isImmediate()) {
                return &input2.move();
            } 
            if ((input2.move().source().isGPR() ||
                 input2.isSourceOperation()) && 
                input1.move().source().isImmediate()) {
                return &input1.move();
            }
        }
    }
    return NULL;
}

/** 
 * Trigger of an operation may change when scheduling.
 * This method updates fu state dependence edges to point to trigger instead of
 * some other node of an operation. (they should point to trigger).
 */
void 
DataDependenceGraph::moveFUDependenciesToTrigger(MoveNode& trigger) {
    if (!trigger.isDestinationOperation()) {
        return;
    }
    ProgramOperation& po = trigger.destinationOperation();

    // move input edges.
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& node = po.inputMove(i);
        if (&node != &trigger) {
            EdgeSet iEdges = rootGraphInEdges(node);
            for (EdgeSet::iterator i=iEdges.begin(); i != iEdges.end(); i++) {
                DataDependenceEdge& e = **i;
                if (e.edgeReason() == DataDependenceEdge::EDGE_FUSTATE) {
                    moveInEdge(node, trigger, e);
                }
            }
        }
    }

    // move output edges.
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& node = po.inputMove(i);
        if (&node != &trigger) {
            EdgeSet oEdges = rootGraphOutEdges(node);
            for (EdgeSet::iterator i=oEdges.begin(); i != oEdges.end(); i++) {
                DataDependenceEdge& e = **i;
                if (e.edgeReason() == DataDependenceEdge::EDGE_FUSTATE) {
                    moveOutEdge(node, trigger, e);
                }
            }
        }
    }
}

/**
 * Finds a liverange, consisting of one (or multiple if guarded writes)
 * write and one or multiple reads to same registers.
 * if none found, returns empty pair.
 * @return first set contains writes, second set uses
 */
LiveRange*
DataDependenceGraph::findLiveRange(
    MoveNode& moveNode, bool dest, bool guard) const {

    NodeSet queuedWrites;
    NodeSet queuedReads;
    NodeSet queuedGuards;

    typedef EdgeSet::iterator EdgeIterator;

    LiveRange* liveRange = new LiveRange;

    if (dest == true) {
        queuedWrites.insert(&moveNode);
    } else {
        if (guard) {
            assert(moveNode.move().isUnconditional());
            queuedGuards.insert(&moveNode);
        } else {
            queuedReads.insert(&moveNode);
        }
    }

    // loop as long as we have not checked successors of some
    // write or predecessors of some read.

    // this is done in double-while-loop. only stop when neither
    // loop wants to continue. 
    while (!queuedWrites.empty() || !queuedReads.empty() ||
           !queuedGuards.empty()) {
        // first check writes.
        while (!queuedWrites.empty()) {
            MoveNode& write = **queuedWrites.begin();
            
            NodeDescriptor nd = descriptor(write);
            EdgeSet out = rootGraphOutEdges(write);
            
            for (EdgeIterator i = out.begin(); i != out.end(); ++i) {
                DataDependenceEdge& e = **i;

                // only register raws. pseudo deps not yet supported.
                if (e.dependenceType() == DataDependenceEdge::DEP_RAW &&
                    e.edgeReason() == DataDependenceEdge::EDGE_REGISTER) {
                    MoveNode& succ = 
                        (static_cast<const DataDependenceGraph*>
                         (rootGraph()))->headNode(e, nd);
            
                    if (e.isBackEdge() || e.headPseudo() ||
                        e.tailPseudo() ||
                        !hasNode(succ)) {
                        liveRange->clear();
                        return liveRange;
                    } else {
                        if (e.guardUse()) {
                            // not yet handled. queue it
                            if (liveRange->guards.find(&succ) ==
                                liveRange->guards.end()) {
                                queuedGuards.insert(&succ);
                            }
                        } else {
                            // not yet handled. queue it
                            if (liveRange->reads.find(&succ) ==
                                liveRange->reads.end()) {
                                queuedReads.insert(&succ);
                            }
                        }
                    }
                }
            }
            // this is fully checked. add to result, remove from queue
            liveRange->writes.insert(&write);
            queuedWrites.erase(&write);
        }

        if (!queueRawPredecessors(
                queuedReads, liveRange->reads, queuedWrites,
                liveRange->writes, false)) {
            liveRange->clear();
            return liveRange;
        }

        if (!queueRawPredecessors(
                queuedGuards, liveRange->guards, queuedWrites,
                liveRange->writes, true)) {
            liveRange->clear();
            return liveRange;
        }
    }
    return liveRange;
}

bool
DataDependenceGraph::queueRawPredecessors(
    NodeSet& queue, NodeSet& finalDest, NodeSet& predQueue,
    NodeSet& predFinalDest, bool guard) const {

    typedef EdgeSet::iterator EdgeIterator;

    // check one read.
    while (!queue.empty()) {
        MoveNode& read = **queue.begin();
        NodeDescriptor nd = descriptor(read);
        EdgeSet in = rootGraphInEdges(read);
        for (auto e : in) {
            if (e->dependenceType() == DataDependenceEdge::DEP_RAW &&
                e->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                e->guardUse() == guard) {
                MoveNode& pred =
                    (static_cast<const DataDependenceGraph*>
                     (rootGraph()))->tailNode(*e, nd);
                if (e->isBackEdge() || e->headPseudo() ||
                    e->tailPseudo() || !hasNode(pred)) {
                    return false;
                } else {
                    if (predFinalDest.find(&pred) ==
                        predFinalDest.end()) {
                        predQueue.insert(&pred);
                    }
                }
            }
        }
        // this is fully checked. add to result, remove from queue
        finalDest.insert(&read);
        queue.erase(&read);
    }
    return true;
}

/**
 * Source of a move is renamed to a register which is not used in
 * this ddg. Removes old edges not needed anymore.
 *
 * If destination of edge also renamed to this reg, keep the edge, 
 * update the data of the edge.
 *
 * @param mn MoveNode whose source or guard is renamed.
 * @param guard true if renamed gaurd, not source.
 */
DataDependenceGraph::UndoData
DataDependenceGraph::sourceRenamed(MoveNode& mn) {

    UndoData undoData;

    TTAProgram::Terminal& term = mn.move().source();
    const TCEString newReg = DisassemblyRegister::registerName(term);

    for (int i = 0; i < outDegree(mn); i++) {
        DataDependenceEdge& oEdge = outEdge(mn,i);
    
        if (oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            oEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
            !oEdge.tailPseudo() && !oEdge.guardUse()) {

            MoveNode& head = headNode(oEdge);
            TTAProgram::Terminal& writeTerm = head.move().destination();

            // if this edge disappers because of the renaming?
            if (oEdge.headPseudo() ||
                (writeTerm.isGPR() && (
                     writeTerm.index() != term.index() ||
                     &writeTerm.registerFile() != &term.registerFile()))) {
                undoData.removedEdges.insert(RemovedEdgeDatum(mn, head, oEdge));
                removeEdge(oEdge, NULL, &mn);
                i--; // don't skip one edge here!
            } else {
                // update the edge. also other end changed
                undoData.changedDataEdges[&oEdge] = oEdge.data();
                oEdge.setData(newReg);
            }
        }
    }
    return undoData;
}

DataDependenceGraph::UndoData
DataDependenceGraph::guardRenamed(MoveNode&mn) {

    UndoData undoData;

    assert(!mn.move().isUnconditional());
    const TTAMachine::RegisterGuard* rg =
        dynamic_cast<const TTAMachine::RegisterGuard*>(
            &mn.move().guard().guard());
    assert(rg != NULL);
    const TCEString newReg = rg->registerFile()->name() +
        '.' + Conversion::toString(rg->registerIndex());

    for (int i = 0; i < outDegree(mn); i++) {
        DataDependenceEdge& oEdge = outEdge(mn,i);

        if (oEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            oEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
            !oEdge.tailPseudo() && oEdge.guardUse()) {

            MoveNode& head = headNode(oEdge);
            TTAProgram::Terminal& writeTerm = head.move().destination();

            // if this edge disappers because of the renaming?
            if (oEdge.headPseudo() ||
                (writeTerm.isGPR() && (
                     writeTerm.index() != rg->registerIndex() ||
                     &writeTerm.registerFile() != rg->registerFile()))) {
                undoData.removedEdges.insert(RemovedEdgeDatum(mn, head, oEdge));
                removeEdge(oEdge, NULL, &mn);
                i--; // don't skip one edge here!
            } else {
                // update the edge. also other end changed
                undoData.changedDataEdges[&oEdge] = oEdge.data();
                oEdge.setData(newReg);
            }
        }
    }
    return undoData;
}

/**
 * Destination of a move is renamed to a register which is not used in
 * this ddg. Removes old edges not needed anymore and updates some edges.
 * Assumes all the RAW successors of this node are also udpated to read thr
 * new register.
 */

DataDependenceGraph::UndoData
DataDependenceGraph::destRenamed(MoveNode& mn) {

    UndoData undoData;
    undoData.newEdges = copyDepsOver(mn, true, false);

    TTAProgram::Terminal& term = mn.move().destination();
    const TCEString newReg = DisassemblyRegister::registerName(term);

    for (int i = 0 ; i < inDegree(mn); i++) {
        DataDependenceEdge& iEdge = inEdge(mn,i);
        MoveNode& tail = tailNode(iEdge);

        // WAR's
        if (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
             iEdge.dependenceType() == DataDependenceEdge::DEP_WAW && 
            !iEdge.headPseudo()) {

            TTAProgram::Terminal& writeTerm = tail.move().destination();

            // if this edge disappers because of the renaming?
            if (iEdge.tailPseudo() ||
                (writeTerm.isGPR() && (
                     writeTerm.index() != term.index() ||
                     &writeTerm.registerFile() != &term.registerFile()))) {
                undoData.removedEdges.insert(RemovedEdgeDatum(tail, mn, iEdge));
                removeEdge(iEdge, NULL, &mn);
                i--; // don't skip one edge here!
            } else {
                // update the edge. also other end changed
                undoData.changedDataEdges[&iEdge] = iEdge.data();
                iEdge.setData(newReg);
            }
        }

        if (iEdge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            iEdge.dependenceType() == DataDependenceEdge::DEP_WAR &&
            !iEdge.headPseudo()) {

            bool removeE = false;
            if (iEdge.tailPseudo()) {
                removeE = true;
                continue;
            }

            if (!iEdge.guardUse()) {
                TTAProgram::Terminal& readTerm = tail.move().source();
                if (readTerm.isGPR() && (
                        readTerm.index() != term.index() ||
                        &readTerm.registerFile() != &term.registerFile())) {
                    removeE = true;
                }
            }
            
            if (iEdge.guardUse()) {
                if (tail.move().isUnconditional()) {
                    removeE = true;
/* something is wrong. but ignore it for now
                    std::cerr << "failing node: " << mn.toString() << std::endl;
                    std::cerr << "failinf tail: " << tail.toString() << std::endl;
                    std::cerr << "EDGE: " << iEdge.toString() << std::endl;
                    writeToDotFile("uncond_gu_war.dot");
*/

                } else {
                    const TTAMachine::RegisterGuard* rg =
                        dynamic_cast<const TTAMachine::RegisterGuard*>(
                            &tail.move().guard().guard());
                    if (rg->registerFile() != &term.registerFile() ||
                        rg->registerIndex() != term.index()) {
                        removeE = true;
                    }
                }
            }
            // if this edge disappers because of the renaming?
            if (removeE) {
                undoData.removedEdges.insert(RemovedEdgeDatum(tail, mn, iEdge));
                removeEdge(iEdge, NULL, &mn);
                i--; // don't skip one edge here!
            } else {
                // update the edge. also other end changed
                undoData.changedDataEdges[&iEdge] = iEdge.data();
                iEdge.setData(newReg);
            }
        }
    }

    for (int i = 0; i < outDegree(mn); i++) {
        DataDependenceEdge& edge = outEdge(mn,i);

        if (edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER) {
            continue;
        }

        if (edge.dependenceType() == DataDependenceEdge::DEP_RAW &&
            !edge.tailPseudo()) {
            // update the data!
            edge.setData(newReg);

        } else {
            if (edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
                MoveNode& head = headNode(edge);
                TTAProgram::Terminal& writeTerm = head.move().destination();

                // if this edge disappers because of the renaming?
                if (edge.tailPseudo()) {
                    continue;
                }
                if (edge.headPseudo() ||
                    (writeTerm.isGPR() && (
                         writeTerm.index() != term.index() ||
                         &writeTerm.registerFile() != &term.registerFile()))){
                     undoData.removedEdges.insert(
                         RemovedEdgeDatum(mn, head, edge));
                    removeEdge(edge, &mn, NULL);
                    i--;
                } else {
                    // update the edge. also other end changed
                    undoData.changedDataEdges[&edge] = edge.data();
                    edge.setData(newReg);
                }
            }
        }
    }
    return undoData;
}

void 
DataDependenceGraph::renamedSimpleLiveRange(
    MoveNode& src, MoveNode& dest, MoveNode& antidepPoint,
    DataDependenceEdge& connectingEdge,
    const TCEString& oldReg, const TCEString& newReg) {
    
    // if nothing changed, retuen
    if (oldReg == newReg) {
        return;
    }

    // copy antideps over these two nodes
    copyDepsOver(src, dest, true, false);

    NodeDescriptor ndADP = descriptor(antidepPoint);
    EdgeSet resultInEdges = inEdges(antidepPoint);
    EdgeSet firstInEdges = inEdges(src);
    EdgeSet firstOutEdges = outEdges(src);
    EdgeSet lastOutEdges = outEdges(dest);

    // move antideps coming to adeppoint into src
    for (EdgeSet::iterator i = resultInEdges.begin(); 
         i != resultInEdges.end(); i++) {
        DataDependenceEdge& e = **i;
        if (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e.headPseudo() &&
            (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
             e.dependenceType() == DataDependenceEdge::DEP_WAR)) {
            assert(e.data() == newReg);
            MoveNode& tail = tailNode(e, ndADP);
            // do not create loop.
            if (e.loopDepth() == 0 && &tail == &src) {
                removeEdge(e);
            } else {
                moveInEdge(antidepPoint, src, e);
            }
        }
    }

    DataDependenceEdge* warEdge = new DataDependenceEdge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_WAR,
        newReg, false, false, false, false, 0);

    DataDependenceEdge* wawEdge = new DataDependenceEdge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_WAW,
        newReg, false, false, false, false, 0);

    connectNodes(src, antidepPoint, *wawEdge);
    connectNodes(dest, antidepPoint, *warEdge);

    assert(connectingEdge.data() == oldReg);
    connectingEdge.setData(newReg);

    // remove antidep edges that were removed because of this renameing
    for (EdgeSet::iterator i = firstInEdges.begin(); 
         i != firstInEdges.end(); i++) {
        DataDependenceEdge& e = **i;
        if (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
             e.dependenceType() == DataDependenceEdge::DEP_WAR)) {
            removeEdge(e, NULL, &src);
        }
    }

    for (EdgeSet::iterator i = firstOutEdges.begin(); 
         i != firstOutEdges.end(); i++) {
        DataDependenceEdge& e = **i;
        if (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e.tailPseudo() && e.data() == oldReg &&
            (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
             e.dependenceType() == DataDependenceEdge::DEP_WAR)) {
            removeEdge(e, &src, NULL);
        }
    }
    for (EdgeSet::iterator i = lastOutEdges.begin(); 
         i != lastOutEdges.end(); i++) {
        DataDependenceEdge& e = **i;
        if (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e.tailPseudo() && e.data() == oldReg &&
            (e.dependenceType() == DataDependenceEdge::DEP_WAW ||
             e.dependenceType() == DataDependenceEdge::DEP_WAR)) {
            removeEdge(e, &src, NULL);
        }
    }
}



MoveNode* 
DataDependenceGraph::findLimitingAntidependenceSource(MoveNode& mn) {
    NodeDescriptor nd = descriptor(mn);
    MoveNode* limitingAntidep = NULL;
    EdgeSet iEdges = inEdges(mn);
    for (EdgeSet::iterator ie = iEdges.begin(); ie != iEdges.end(); ie++) {
        DataDependenceEdge& e = **ie;
        // TODO: WAW?
        if (e.dependenceType() == DataDependenceEdge::DEP_WAR && 
            e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e.headPseudo() && !e.tailPseudo() && !e.guardUse()) {
            MoveNode& tail = tailNode(e,nd);
            if (tail.isScheduled() && (limitingAntidep == NULL || 
                                       tail.cycle()>limitingAntidep->cycle())){
                limitingAntidep = &tail;
            }
        }
    }
    return limitingAntidep;
}

MoveNode* 
DataDependenceGraph::findLimitingAntidependenceDestination(MoveNode& mn) {
    NodeDescriptor nd = descriptor(mn);
    MoveNode* limitingAntidep = NULL;
    EdgeSet oEdges = outEdges(mn);
    for (EdgeSet::iterator oe = oEdges.begin(); oe != oEdges.end(); oe++) {
        DataDependenceEdge& e = **oe;
        // TODO: WAW?


        if (e.dependenceType() == DataDependenceEdge::DEP_WAR && 
            e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e.headPseudo() && !e.tailPseudo() && !e.guardUse()) {
            MoveNode& head = headNode(e,nd);
            if (head.isScheduled() && (limitingAntidep == NULL || 
                                       head.cycle()<limitingAntidep->cycle())){
                limitingAntidep = &head;
            }
        }
    }
    return limitingAntidep;
}

/**
 * Copies incoming edges which come from nodes on other BB's to a node
 * into another node. If an edge comes from inside same BB, does not copy.
 *
 * @param source node containing original incoming edges.
 * @param nodeCopy node where to copy the inedges to.
 */
void 
DataDependenceGraph::copyExternalInEdges(
    MoveNode& nodeCopy, const MoveNode& source) {
    const BasicBlockNode* origBB = &getBasicBlockNode(source);
    
    EdgeSet edges = inEdges(source);
    for (EdgeSet::iterator i = edges.begin(); i != edges.end(); i++) {
        Edge& edge = **i;
        MoveNode& tail = tailNode(edge);
        if (&getBasicBlockNode(tail) != origBB) {
            DataDependenceEdge* newEdge = new DataDependenceEdge(edge);
            connectNodes(tail, nodeCopy, *newEdge);
        }
    }
}

/**
 * Copies outgoing edges which goes from a node to nodes in other BB's
 * into another node. If an edge goes to a node inside same BB, does not copy.
 *
 * @param source node containing original outgoingcoming edges.
 * @param nodeCopy node where to copy the outedges to.
 */
void 
DataDependenceGraph::copyExternalOutEdges(
    MoveNode& nodeCopy, const MoveNode& source) {
    const BasicBlockNode* origBB = &getBasicBlockNode(source);
    
    EdgeSet edges = outEdges(source);
    for (EdgeSet::iterator i = edges.begin(); i != edges.end(); i++) {
        Edge& edge = **i;
        MoveNode& head = headNode(edge);
        if (&getBasicBlockNode(head) != origBB) {
            DataDependenceEdge* newEdge = new DataDependenceEdge(edge);
            connectNodes(nodeCopy, head, *newEdge);
        }
    }
}

/**
 * Creates dependencies from incoming definitions in other BB's to a reg use.
 * 
 * @param mnd data about the register usage.
 */
void 
DataDependenceGraph::updateRegUse(
    const MoveNodeUse& mnd, const TCEString& reg, TTAProgram::BasicBlock& bb) {

    // create RAW's from definitions in previous BBs.
    std::set<MoveNodeUse>& defReaches = 
        bb.liveRangeData_->regDefReaches_[reg];
    for (std::set<MoveNodeUse>::iterator i = defReaches.begin();
         i != defReaches.end(); i++) {

        const MoveNodeUse& source = *i;
        if (hasNode(*source.mn())) {
            DataDependenceEdge* dde =
                // create dependency edge
                new DataDependenceEdge(
                    mnd.ra() ? DataDependenceEdge::EDGE_RA :
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_RAW, reg, mnd.guard(), false, 
                    source.pseudo(), mnd.pseudo(), source.loop());

            // and connect.
            connectOrDeleteEdge(*source.mn(), *mnd.mn(), dde);
        }
    }
}

/**
 * Creates dependencies to a register write from  MN's in other BBs.
 *
 * @param mnd movenode which writes to a register
 * @param reg register wrere the movenode writes to
 * @param createAllAntideps whether to create antideps from all BB's
 *        or just from same bb in case of single-bb loop.
 */
void
DataDependenceGraph::updateRegWrite(
    const MoveNodeUse& mnd, const TCEString& reg, TTAProgram::BasicBlock& bb) {
    // WaWs
    std::set<MoveNodeUse>& defReaches = 
        bb.liveRangeData_->regDefReaches_[reg];
    for (std::set<MoveNodeUse>::iterator i = defReaches.begin();
         i != defReaches.end(); i++) {

        const MoveNodeUse& source = *i;
        if (hasNode(*source.mn()) && source.mn()->isMove() &&
            (hasAllRegisterAntidependencies() || 
             &getBasicBlockNode(*source.mn()) == 
             &getBasicBlockNode(*mnd.mn()))) {
            // create dependency edge
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    mnd.ra() ? DataDependenceEdge::EDGE_RA :
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW, reg, false, false, 
                    source.pseudo(), mnd.pseudo(), source.loop());
            // and connect.
            connectOrDeleteEdge(*source.mn(), *mnd.mn(), dde);
        }
    }
    
    // WaRs
    std::set<MoveNodeUse>& useReaches = 
        bb.liveRangeData_->regUseReaches_[reg];
    for (std::set<MoveNodeUse>::iterator i = useReaches.begin();
         i != useReaches.end(); i++) {

        const MoveNodeUse& source = *i;
        if (hasNode(*source.mn()) && source.mn()->isMove() &&
            (hasAllRegisterAntidependencies() || 
             &getBasicBlockNode(*source.mn()) == 
            &getBasicBlockNode(*mnd.mn()))) {
            // create dependency edge
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    mnd.ra() ? DataDependenceEdge::EDGE_RA :
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, reg, source.guard(), false, 
                    source.pseudo(), mnd.pseudo(), source.loop());
            // and connect.
            connectOrDeleteEdge(*source.mn(), *mnd.mn(), dde);
        }
    }
}

/**
 * Removes guard raw edges coming to some node.
 * 
 * This is called if the guard is removed from the node.
 *
 * @param node The node.
 */
void 
DataDependenceGraph::removeIncomingGuardEdges(MoveNode& node) {
    NodeDescriptor nd = descriptor(node);
    std::pair<InEdgeIter, InEdgeIter> edges =
        boost::in_edges(nd, graph_);

    // loop thru all edges
    for (InEdgeIter ei = edges.first; ei != edges.second; ) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge& edge = *graph_[ed];
        if (edge.guardUse() && edge.dependenceType() == 
            DataDependenceEdge::DEP_RAW) {
            boost::remove_edge(*ei, graph_);

            // removing messes up the iterator. start again from first.
            edges = boost::in_edges(nd, graph_);
            ei = edges.first;
        } else {
            ei++;
        }
    }
}

/**
 * Removes guard raw edges coming to some node.
 * 
 * This is called if the guard is removed from the node.
 *
 * @param node The node.
 */
void 
DataDependenceGraph::removeOutgoingGuardWarEdges(MoveNode& node) {
    NodeDescriptor nd = descriptor(node);
    std::pair<OutEdgeIter, OutEdgeIter> edges =
        boost::out_edges(nd, graph_);

    // loop thru all edges
    for (OutEdgeIter ei = edges.first; ei != edges.second; ) {
        EdgeDescriptor ed = *ei;
        DataDependenceEdge& edge = *graph_[ed];
        if (edge.guardUse() && edge.dependenceType() == 
            DataDependenceEdge::DEP_WAR) {
            boost::remove_edge(*ei, graph_);

            // removing messes up the iterator. start again from first.
            edges = boost::out_edges(nd, graph_);
            ei = edges.first;
        } else {
            ei++;
        }
    }
}

/**
 * Returns true in case the given dependency cannot be (currently) avoided by 
 * techniques such as register renaming or speculation.
 *
 * If the edge is "avoidable", it doesn't meen it always is. E.g., sometimes
 * the register renamer can find a better register to remove the dependency, 
 * sometimes it cannot. That is, this method returns true only if it's *certain*
 * we cannot do anything about the dependency with the current compiler backend 
 * or by, e.g., adding more registers to the machine.
 */
bool
DataDependenceGraph::isNotAvoidable(const DataDependenceEdge& edge) const {

    if (!edge.isFalseDep()) return true;
    MoveNode& tail = tailNode(edge);
    MoveNode& head = headNode(edge);

    /* The control dependency caused antidep case. In case the antidep
       overwrites the register with a different predicate than the other,
       it is not possible to simply rename the other register, but, e.g.,
       speculative execution has to be added with a final write to the
       original register.

       Example:

       a) p1: foo -> r1
       b) p1: r1 -> X
       c) p2: foo -> r1
       d) p2 r1 -> Y jne
       e) r1 -> foo

       In case p2 = !p1 then we can parallelize freely. In case
       p2 = p1 then we can parallelize after a reg rename of r1.
       Otherwise, speculative or similar transformation has to be
       done, which currently is not supported by tcecc.
    */
    if ((edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) &&        
        (!tail.move().isUnconditional() && !head.move().isUnconditional()) &&
        !(tail.move().guard().guard().isEqual(head.move().guard().guard()) ||
          tail.move().guard().guard().isOpposite(head.move().guard().guard())))
        return true;

    if ((edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER) &&        
        ((tail.move().isUnconditional() && !head.move().isUnconditional()) ||
         (!tail.move().isUnconditional() && head.move().isUnconditional())))
        return true;
    
        
    return false;
}

DataDependenceGraph::NodeSet
DataDependenceGraph::regDepSiblings(const MoveNode& mn) const {

    NodeSet potentialSiblings;
    NodeSet siblings;
    NodeSet regDepSources;

    EdgeSet ins = inEdges(mn);
    int edgeCounter = 0;
    for (auto e: ins) {
        if ((e->edgeReason() != DataDependenceEdge::EDGE_REGISTER &&
             e->edgeReason() != DataDependenceEdge::EDGE_RA) ||
            e->headPseudo() || e->tailPseudo() || e->guardUse()) {
            continue;
        }
        edgeCounter++;

        MoveNode& tail = tailNode(*e);
        regDepSources.insert(&tail);
        const EdgeSet& outs = outEdges(tail);
        for (auto f: outs) {
            if ((f->edgeReason() != DataDependenceEdge::EDGE_REGISTER &&
                 f->edgeReason() != DataDependenceEdge::EDGE_RA) ||
                f->headPseudo() || f->tailPseudo() || f->guardUse()) {
                continue;
            }
            if (e != f && *e == *f) {
                MoveNode& head = headNode(*f);
                if (&head != &mn) {
                    potentialSiblings.insert(&head);
                }
            }
        }
    }

    for (auto n : potentialSiblings) {
        bool ok = true;
        EdgeSet ins2 = inEdges(*n);
        int edgeCounter2 = 0;
        for (auto e: ins2) {
            if ((e->edgeReason() != DataDependenceEdge::EDGE_REGISTER &&
                 e->edgeReason() != DataDependenceEdge::EDGE_RA) ||
                e->headPseudo() || e->tailPseudo() || e->guardUse()) {
                continue;
            }

            bool foundSame = false;
            const MoveNode& tail = tailNode(*e);
            for (auto f : ins) {
                if (*e == *f && &tailNode(*f) == &tail) {
                    foundSame = true;
                    break;
                }
            }

            // did not find similar edge from the inputs of the original node
            if (!foundSame) {
                ok = false;
                break;
            }
            edgeCounter2++;
        }
        if (ok && edgeCounter2 == edgeCounter) {
            siblings.insert(n);
        }
    }
    return siblings;
}

bool DataDependenceGraph::isLoopInvariant(const MoveNode& mn) const {
    EdgeSet iEdges = inEdges(mn);
    for (EdgeSet::iterator i = iEdges.begin(); i != iEdges.end(); i++) {
        DataDependenceEdge& e = **i;
        if (e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            e.dependenceType() == DataDependenceEdge::DEP_RAW &&
            !e.guardUse()) {
            return false;
        }
    }
    return true;
}

bool DataDependenceGraph::hasOtherRegWawSources(const MoveNode& mn) const {
    EdgeSet iEdges = inEdges(mn);
    for (auto e: iEdges) {
        if (e->edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            e->dependenceType() != DataDependenceEdge::DEP_WAW) {
            continue;
        }
        MoveNode& tail = tailNode(*e);
        if (&tail != &mn) {
            return true;
        }
    }
    return false;
}

void DataDependenceGraph::undo(UndoData& undoData) {
    for (auto i : undoData.changedDataEdges) {
        i.first->setData(i.second);
    }

    for (auto i: undoData.removedEdges) {
        connectNodes(i.nTail, i.nHead, i.edge);
    }

    for (auto i: undoData.newEdges) {
        removeEdge(*i);
    }
}

DataDependenceGraph::EdgeSet
DataDependenceGraph::operationInEdges(const MoveNode& node) const {
    std::pair<InEdgeIter, InEdgeIter> edges = boost::in_edges(
        descriptor(node), graph_);

    EdgeSet result;

    for (InEdgeIter ei = edges.first; ei != edges.second; ++ei) {
        DataDependenceEdge* edge = graph_[(*ei)];
        if (edge->edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
            edgeDescriptors_[edge] = *ei;
            result.insert(edge);
        }
    }
    return result;
}
