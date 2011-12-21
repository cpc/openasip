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
 * @file ProgramDependenceGraph.cc
 *
 * Implementation of prototype of graph-based program representation:
 * declaration of the program dependence graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */


#include "ProgramDependenceGraph.hh"
#include "MapTools.hh"
#include "Exception.hh"
#include "MoveGuard.hh"
#include "Move.hh"



ProgramDependenceGraph::~ProgramDependenceGraph() {}

/**
 * Constructor.
 * Build Program Dependence Graph from Control and Data Dependence graphs.
 * @param cdg Control Dependence Graph of procedure
 * @param ddg Data Dependence Graph of procedure
 */
ProgramDependenceGraph::ProgramDependenceGraph(
    ControlDependenceGraph& cdg, DataDependenceGraph& ddg) : 
    BoostGraph<ProgramDependenceNode, ProgramDependenceEdge>(cdg.name()), 
    cdg_(&cdg), ddg_(&ddg) {

    ControlToProgram cNodePNode;
    std::map<BasicBlockNode*, ControlDependenceNode*> BBNodeCDNode;
    std::map<MoveNode*, ProgramDependenceNode*> movePD;

    // Creates PDG node for each region node of CDG
    for (int i = 0; i < cdg.nodeCount(); i++) {
        ControlDependenceNode& cnode = cdg.node(i); 
        if (cnode.isRegionNode()
            || cnode.isEntryNode()
            || cnode.isExitNode()) {
            ProgramDependenceNode* newNode = new
                ProgramDependenceNode(cnode,nodeCount());
            addNode(*newNode);
            cNodePNode.insert(
                std::pair<ControlDependenceNode*, ProgramDependenceNode*>(
                    &cnode, newNode));
            if (cnode.isEntryNode()) {
                BBNodeCDNode.insert(
                    std::pair<BasicBlockNode*, ControlDependenceNode*>(
                        cnode.basicBlockNode(), &cdg.node(i)));                
            }
        } else {
            BBNodeCDNode.insert(
                std::pair<BasicBlockNode*, ControlDependenceNode*>(
                    cnode.basicBlockNode(), &cdg.node(i)));
        }
    }
    // Copies edges between region nodes of CDG into the PDG
    for (int i = 0; i < cdg.edgeCount(); i++) {
        ControlDependenceEdge& edge = cdg.edge(i);
        ControlDependenceNode* headNode;
        ControlDependenceNode* tailNode;
        headNode = &cdg.headNode(edge);
        tailNode = &cdg.tailNode(edge);        
        if ((headNode->isRegionNode()
              || headNode->isEntryNode()
              || headNode->isExitNode())
            && (tailNode->isRegionNode()
              || tailNode->isEntryNode()
              || tailNode->isExitNode())) {
            // headNode is one with arrow in boost graphs
            ProgramDependenceNode* sourceNode;
            ProgramDependenceNode* targetNode;
            sourceNode = MapTools::valueForKey<ProgramDependenceNode*>(
                cNodePNode, tailNode);
            targetNode = MapTools::valueForKey<ProgramDependenceNode*>(
                cNodePNode, headNode);
            ProgramDependenceEdge* pEdge;
            pEdge = new ProgramDependenceEdge(edge);
            connectNodes(*sourceNode, *targetNode, *pEdge);
        }
    }
    
    // Add each MoveNode of DDG also to PDG
    for (int i = 0; i < ddg.nodeCount(); i++) {
        ProgramDependenceNode* newNode = NULL;
        // Guarded jumps and calls becomes predicates
        MoveNode& node = ddg.node(i); 
        if (node.isMove() &&
            node.move().isControlFlowMove() &&
            !node.move().isUnconditional()) {
            newNode = new 
                ProgramDependenceNode(node, nodeCount(), true);
        } else if (node.isMove() &&
            node.move().isControlFlowMove() &&
            node.move().isJump() &&
            !node.move().isReturn()){
            continue;
        } else {
            newNode = new ProgramDependenceNode(node, nodeCount());
        }
        addNode(*newNode);
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
        if (!MapTools::containsKey(movePD, &ddg.headNode(edge))) {
            continue;
        }        
        try {
            pSource = MapTools::valueForKey<ProgramDependenceNode*>(
                movePD, &ddg.tailNode(edge));
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessageStack());
        }
        try {
            pTarget = MapTools::valueForKey<ProgramDependenceNode*>(
                movePD, &ddg.headNode(edge));
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessageStack());
        }        
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
        cNode = MapTools::valueForKey<ControlDependenceNode*>(
            BBNodeCDNode, b);
        ProgramDependenceNode* pNode = NULL;
        try {
            pNode = MapTools::valueForKey<ProgramDependenceNode*>(
                movePD, &node);
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessageStack());
        }                               
        // For each MoveNode, find in which Basic Block it was
        // and all input edges that went into CDG for given Basic Block
        // are also added to the MoveNode
        for (int j = 0; j < cdg.inDegree(*cNode); j++) {
            ControlDependenceNode* source;
            source = &cdg.tailNode(cdg.inEdge(*cNode, j));
            if (source->isRegionNode() || source->isEntryNode()) {
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
        throw InvalidData(
            __FILE__, __LINE__, __func__, 
            "Guarded jump has inDegree different from 2!");
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
    
    for (int j = 0; j < cdg_->outDegree(cNode); j++) {
        ControlDependenceNode* target;
        target = &cdg_->headNode(cdg_->outEdge(cNode, j));
        if (target->isRegionNode()) {
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
