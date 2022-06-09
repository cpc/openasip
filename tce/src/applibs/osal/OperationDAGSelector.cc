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
#include "ConstantNode.hh"
#include "TCEString.hh"
#include "ImmInfo.hh"

/**
 * Returns a list of dags of an operation, which use only 
 * given set of operations.
 *
 * Additionally, immediate info can be provided to this function. The info
 * describes immediate transport capabilities to the operands of the (known)
 * operations. ImmInfo discards dags that have constant values bound to operand
 * that can not have the immediate transported to.
 *
 * @param opName Name of operation whose DAGs are requested.
 * @param opSet Set of operations that are allowed to be referred by 
 *              the returned DAGs.
 * @param immInfo The immediate info.
 * @return List of DAGs which comply the search parameters.
 */
OperationDAGSelector::OperationDAGList
OperationDAGSelector::findDags(
    const std::string& opName,
    OperationSet opSet,
    const ImmInfo* immInfo) {

    OperationDAGList retDags;
    OperationPool opPool;
    Operation& op = opPool.operation(opName.c_str());

    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& currDag = op.dag(i);        
        
        if (op.dagError(i) != "") {
            throw IllegalParameters(__FILE__,__LINE__,__func__,
                TCEString("Operation:") + op.name()
                + " has invalid dag, index: " + Conversion::toString(i)
                + "\n\tError: " + op.dagError(i));
        }

        if (countUnknownOperations(currDag, opSet) != 0) {
            continue; // Discard the dag with unsupported operations
        }

        if (immInfo) {
            bool discardDag = false;
            for (int i = 0; i < currDag.nodeCount(); i++) {
                const ConstantNode* cNode =
                    dynamic_cast<ConstantNode*>(&currDag.node(i));
                if (cNode == nullptr) continue;

                for (auto& edge : currDag.outEdges(*cNode)) {
                    const OperationNode* opNode = dynamic_cast<OperationNode*>(
                        &currDag.headNode(*edge));
                    assert(opNode &&
                        "Operation DAG node is other than OperationNode.");

                    if (immInfo->count(
                            opNode->referencedOperation(),
                            edge->dstOperand())) {
                        // TODO check if constant can be encoded as short
                        // immediate.

                        // TODO: should check if the operand can be swapped.
                        // If so, alter the dag to get the better immediate
                        // transport support.
                    } else {
                        discardDag = true;
                        break;
                    }
                }
                if (discardDag) break;
            }
            if (discardDag) {
                continue;
            }
        }
        retDags.push_back(&currDag);
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
