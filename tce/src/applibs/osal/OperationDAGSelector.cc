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
 * @file OperationDAGSelector.cc
 *
 * Definition of OperationDAGSelector class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "OperationDAGSelector.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "OperationNode.hh"
#include "TCEString.hh"

/**
 * Returns minimum opset that is required by llvm.
 *
 * @return Minimumn opset that is required for llvm.
 */
OperationDAGSelector::OperationSet 
OperationDAGSelector::llvmRequiredOpset() {
    OperationSet requiredOps;
       
    requiredOps.insert("ADD");
    requiredOps.insert("SUB");
    requiredOps.insert("MUL");
    requiredOps.insert("DIV");
    requiredOps.insert("DIVU");
    requiredOps.insert("DIV");
    requiredOps.insert("MOD");
    requiredOps.insert("MODU");

    requiredOps.insert("LDW");
    requiredOps.insert("LDH");
    requiredOps.insert("LDHU");
    requiredOps.insert("LDQ");
    requiredOps.insert("LDQU");
    requiredOps.insert("STW");
    requiredOps.insert("STH");
    requiredOps.insert("STQ");

    requiredOps.insert("SXHW");
    requiredOps.insert("SXQW");

    requiredOps.insert("AND");
    requiredOps.insert("XOR");
    requiredOps.insert("IOR");

    requiredOps.insert("SHL");
    requiredOps.insert("SHR");
    requiredOps.insert("SHRU");

    requiredOps.insert("EQ");
    requiredOps.insert("NE");
    requiredOps.insert("LT");
    requiredOps.insert("LTU");
    requiredOps.insert("LE");
    requiredOps.insert("LEU");
    requiredOps.insert("GT");
    requiredOps.insert("GTU");
    requiredOps.insert("GE");
    requiredOps.insert("GEU");

   
    // -- Floating point operations --
    requiredOps.insert("ADDF");
    requiredOps.insert("SUBF");
    requiredOps.insert("MULF");
    requiredOps.insert("DIVF");
    requiredOps.insert("NEGF");
    requiredOps.insert("SQRTF");

    requiredOps.insert("CFI");
    requiredOps.insert("CFIU");
    requiredOps.insert("CIF");
    requiredOps.insert("CIFU");

    // Ordered FP comparison operations
    requiredOps.insert("EQF");
    requiredOps.insert("NEF");
    requiredOps.insert("LTF");
    requiredOps.insert("LEF");
    requiredOps.insert("GTF");
    requiredOps.insert("GEF");

    // Unordered FP comparison operations
    requiredOps.insert("EQUF");
    requiredOps.insert("NEUF");
    requiredOps.insert("LTUF");
    requiredOps.insert("LEUF");
    requiredOps.insert("GTUF");
    requiredOps.insert("GEUF");

    // Ordered/unordered operations
    requiredOps.insert("ORDF");
    requiredOps.insert("UORDF");

    return requiredOps;
}

/**
 * Returns a list of dags of an operation, which use only 
 * given set of operations.
 *
 * @param opName Name of operation whose DAGs are requested.
 * @param opSet Set of operations that are allowed to be referred by 
 *              the returned DAGs.
 * @return List of DAGs which comply the search parameters.
 */
OperationDAGSelector::OperationDAGList
OperationDAGSelector::findDags(const std::string& opName, OperationSet opSet) {

    OperationDAGList retDags;
    OperationPool opPool;
    Operation& op = opPool.operation(opName.c_str());

    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& currDag = op.dag(i);        
        
        if (countUnknownOperations(currDag, opSet) == 0) {
            retDags.push_back(&currDag);
        }
    }
               
    return retDags;
}

/**
 * Returns number of operations that DAG contains which are 
 * not in given opset.
 *
 * Currently does not check recursively, but only one level used operations.
 *
 * @param dag DAG which is checked.
 * @param opSet Operations that are found.
 * @return Number of operations that DAG contains that did not exist in opset.
 */
int
OperationDAGSelector::countUnknownOperations(
    OperationDAG& dag, OperationSet& opSet) {

    int strangeOpCount = 0;
    
    for (int i = 0; i < dag.nodeCount(); i++) {
        OperationNode* node = dynamic_cast<OperationNode*>(&dag.node(i));
        
        // check if operation was found from opset
        if (node != NULL) {
            Operation& refOp = node->referencedOperation();
            
            if (opSet.find(refOp.name()) == opSet.end()) {
                strangeOpCount++;
            }
        }
    }
    
    return strangeOpCount;
}

/**
 * Tries to find simplest graph that is expanded with given opset.
 *
 * @todo Implement function when neccessary. Right now returns DAG for
 *       operation that has smallest number of operations that are not
 *       found in given opset.
 *
 * @param op Operation whose expanded dag is requested.
 * @param opSet Operation names which are allowed to use for expanding.
 * @return Dynamically allocated DAG for operation expanded with given opset.
 *         OperationDAG::null if there is no dag for operation.
 */
OperationDAG* 
OperationDAGSelector::createExpandedDAG(
    const Operation& op, OperationSet& opSet) {
    
    OperationDAGList foundDags;
    int lastUnknownOperations = INT_MAX;
    
    // find DAG that has lowest cont of operations that are not in opset
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& currDag = op.dag(i);
        int strangeCount = countUnknownOperations(currDag, opSet);
        
        if (strangeCount <= lastUnknownOperations) {
            
            if (strangeCount < lastUnknownOperations) {            
                lastUnknownOperations = strangeCount;
                foundDags.clear();
            }
            
            foundDags.push_back(&currDag);            
        }     
    }
    
    OperationDAG& selectedDag = foundDags.smallestNodeCount();
    
    if (selectedDag.isNull()) {
        return &selectedDag;
    } else {
        return new OperationDAG(selectedDag);
    }    
}

#if 0

/**
 * Replaces destination DAG node with copy of source DAG.
 */
void replaceNode(
    OperationDAG& srcDag, 
    OperationDAG& dstDag, OperationDAGNode &dstNode) {
    
// std::cerr << "*************** Started replacing node: "
//    << dynamic_cast<OperationNode*>(&dstNode)->referencedOperation().name()
//    << " ***********" << std::endl;

    typedef std::pair<OperationDAGNode*, int> OperandDef;
    typedef std::deque<OperandDef> OperandDefList;
    typedef std::map<int, OperandDefList> TerminalBindings;
   
    // map containing bindings of each operand, first in vector is input 
    // the rest are outputs
    TerminalBindings bindings;       

    // get operand bindings of terminal nodes of source dag that is used 
    // to replace dst node
    for (int i = 0; i < srcDag.nodeCount(); i++) {
        TerminalNode* term = dynamic_cast<TerminalNode*>(&srcDag.node(i));

        if (term != NULL) {
            int termOperandNumber = term->operandIndex();
                        
            if (srcDag.inDegree(*term) == 1) {
                // there can be only one input 
                OperationDAGEdge& edge = srcDag.inEdge(*term, 0);
                OperationDAGNode& tail = srcDag.tailNode(edge);
                OperandDef inputOp(&tail, edge.srcOperand());
                
// std::cerr << "Added SOURCE for  IO(" << termOperandNumber << ") binding: " 
//           << "(" << unsigned(&tail) << ":" << edge.srcOperand() << ")"
//           << std::endl;

                bindings[termOperandNumber].push_front(inputOp);
            } else {
                // loop through all the outputs
                for (int j = 0; j < srcDag.outDegree(*term); j++) {
                    OperationDAGEdge& edge = srcDag.outEdge(*term, j);
                    OperationDAGNode& head = srcDag.headNode(edge);
                    OperandDef outputOp(&head, edge.dstOperand());

//  std::cerr << "Added DST for  IO(" << termOperandNumber << ") binding: " 
//            << "(" << unsigned(&head) << ":" << edge.dstOperand() << ")"
//            << std::endl;

                    bindings[termOperandNumber].push_back(outputOp);
                }
            }
        }
    }
    
    // get operand bindings of destination dag where the replaced node is
    for (int i = 0; i < dstDag.inDegree(dstNode); i++) {
        OperationDAGEdge& edge = dstDag.inEdge(dstNode, i);
        int termOperandNumber = edge.dstOperand();        
        OperationDAGNode& tail = dstDag.tailNode(edge);
        OperandDef inputOp(&tail, edge.srcOperand());        

// std::cerr << "Added (living in dst DAG) SOURCE for  IO(" 
//           << termOperandNumber << ") binding: " 
//           << "(" << unsigned(&tail) << ":" << edge.srcOperand() << ")"
//           << std::endl;

        bindings[termOperandNumber].push_front(inputOp);    
    }
    
    for (int i = 0; i < dstDag.outDegree(dstNode); i++) {
        OperationDAGEdge& edge = dstDag.outEdge(dstNode, i);
        int termOperandNumber = edge.srcOperand();
        OperationDAGNode& head = dstDag.headNode(edge);
        OperandDef outputOp(&head, edge.dstOperand());        

// std::cerr << "Added (living in dst DAG) DST for  IO(" 
//           << termOperandNumber << ") binding: " 
//           << "(" << unsigned(&head) << ":" << edge.dstOperand() << ")"
//           << std::endl;
        
        bindings[termOperandNumber].push_back(outputOp);    
    }
    
    // ******** Copy non terminal nodes and connections to new DAG **********
    
    // keep track which nodes of destination DAG corresponds original 
    // nodes of source DAG. map[dstNode] = srcNode..
    std::map<OperationDAGNode*, OperationDAGNode*> bookKeeper;
    
    for (int i = 0; i < srcDag.nodeCount(); i++) {
        OperationNode* opNode = dynamic_cast<OperationNode*>(&srcDag.node(i));
        
        // ignore copying terminal nodes
        if (opNode != NULL) {
            
            if (bookKeeper.find(opNode) == bookKeeper.end()) {
                bookKeeper[opNode] = 
                    dynamic_cast<OperationDAGNode*>(opNode->clone());
                dstDag.addNode(*(bookKeeper[opNode]));
// std::cerr << "Creating node: map[" << unsigned(opNode) << "] = " 
//           << unsigned(bookKeeper[opNode])
//           << " name: " << opNode->referencedOperation().name()
//                           << std::endl;
            } else {
// std::cerr << "Node " << unsigned(opNode) << ":" 
//           << opNode->referencedOperation().name()
//           << " was already created." << std::endl;
            }
            
            // connect all output nodes 
            for (int j = 0; j < srcDag.outDegree(*opNode); j++) {            
                OperationDAGEdge& edge = srcDag.outEdge(*opNode, j);
                OperationDAGNode& headNode = srcDag.headNode(edge);
                
                // ignode terminal nodes
                if (dynamic_cast<TerminalNode*>(&headNode) == NULL) {

                    // create new node if not connected
                    if (bookKeeper.find(&headNode) == bookKeeper.end()) {
                        // if terminal node ignore... 
                        bookKeeper[&headNode] = 
                            dynamic_cast<OperationDAGNode*>(headNode.clone());
                        dstDag.addNode(*(bookKeeper[&headNode]));
//   std::cerr << "Creating HEAD node: map[" 
//   << unsigned(&headNode) << "] = " 
//   << unsigned(bookKeeper[&headNode])
//   << " name: " 
//   << dynamic_cast<OperationNode*>(&headNode)->referencedOperation().name()
//   << std::endl;
                    } else {
//   std::cerr << "Node " << unsigned(&headNode) << ":" 
//   << dynamic_cast<OperationNode*>(&headNode)->referencedOperation().name()
//   << " was already created." << std::endl;
                    }
                    
                    OperationDAGEdge* edgeCopy = 
                        dynamic_cast<OperationDAGEdge*>(edge.clone());
                    
//   std::cerr << "Connecting copied nodes" << std::endl;
                    
                    dstDag.connectNodes(
                        *bookKeeper[opNode], *bookKeeper[&headNode], 
                        *edgeCopy);
                }
            }
        }
    }
    
    // **** Connect new nodes with old dst DAG nodes ****
    
    for (TerminalBindings::iterator i = bindings.begin(); 
         i != bindings.end(); i++) {
        
        OperandDefList& defList = i->second;
        
        OperationDAGNode* srcNode = defList[0].first;
        int srcOperand = defList[0].second;
        
        for (unsigned int j = 1; j < defList.size(); j++) {
            OperationDAGNode* dstNode = defList[j].first;
            int dstOperand = defList[j].second;            
            OperationDAGEdge* edge = 
                new OperationDAGEdge(srcOperand, dstOperand);
            
            // check which node was originally in dst DAG
            if (dstDag.hasNode(*srcNode)) {
// std::cerr << "trying to connect Input (" 
//           << unsigned(srcNode) << ":" << edge->srcOperand() 
//           << ") operand: " << i->first 
//           << " to " << unsigned(bookKeeper[dstNode]) 
//           << ":" << edge->dstOperand()
//           << " op name: " << dynamic_cast<OperationNode*>(
//           bookKeeper[dstNode])->referencedOperation().name() 
//                           << std::endl;

                dstDag.connectNodes(*srcNode, *(bookKeeper[dstNode]), *edge);
            } else {
// std::cerr << "trying to connect Output ("
//  << unsigned(dstNode) << ":" << edge->dstOperand() 
//  << ") operand: " << i->first 
//  << " to " << unsigned(bookKeeper[srcNode]) << ":" << edge->srcOperand()
//  << " op name: " << dynamic_cast<OperationNode*>(
//  bookKeeper[srcNode])->referencedOperation().name() 
//  << std::endl;

                dstDag.connectNodes(*(bookKeeper[srcNode]), *dstNode, *edge);
            }
        }
    }
    
    // delete replaced operation node from the destination graph
    dstDag.removeNode(dstNode);
    delete &dstNode;
// std::cerr << "*********** Node replaced successfully *******" << std::endl;
}

/**
 * Creates expanded DAG out of graph in parameter.
 *
 * @param dag Graph which is used to create expanded graph.
 * @return Expanded graph.
 */
OperationDAG* 
OperationDAGSelector::createExpandedDag(const OperationDAG& dag) {    
    OperationDAG* dagCopy = new OperationDAG(dag);    
    // check all operation nodes
    for (int i = 0; i < dagCopy->nodeCount(); i++) {        
        OperationNode* opNode = 
            dynamic_cast<OperationNode*>(&dagCopy->node(i));
        
        if (opNode != NULL && 
            !opNode->referencedOperation().dag(0).isTrivial()) {
            // we changed graph, so we have to start again 
            // iterating nodes of graph
            i = 0; 
            OperationDAG& childDag = opNode->referencedOperation().dag(0);
            replaceNode(childDag, *dagCopy, *opNode);            
        }
    }
    return dagCopy;
}

#endif
