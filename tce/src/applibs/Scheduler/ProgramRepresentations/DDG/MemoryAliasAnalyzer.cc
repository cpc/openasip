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
/*
 * @file MemoryAliasAnalyzer.cc
 *
 * Implementation of Memory Alias Analyzer interface
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @author Heikki Kultala 2010 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "MemoryAliasAnalyzer.hh"
#include "MoveNode.hh"
#include "ProgramOperation.hh"
#include "TCEString.hh"
#include "Operand.hh"
#include "OperationDAG.hh"
#include "OperationNode.hh"
#include "TerminalNode.hh"

/**
 * Compares two indeces (memory addresses or part of memory address).
 * Checks memory operand size and return unknown for partial alias.
 *
 * This methods takes care of checking memory access size and handling
 * partial aliasing (returning unknown for those)
 * 
 * @param index1 first index to compare
 * @param index2 second index to compare
 * @param mn1 first node
 * @param mn2 second node
 * @return true if alias, false if not alias, unknown for partial alias.
 */
MemoryAliasAnalyzer::AliasingResult 
MemoryAliasAnalyzer::compareIndeces(
    int index1, int index2, const ProgramOperation& pop1, 
    const ProgramOperation& pop2) {
    const Operation& op1 = pop1.operation();
    const Operation& op2 = pop2.operation();

    int maus1 = mausOfOperation(op1);
    int maus2 = mausOfOperation(op2);

    // unknown op? don't know how much memory it touches.
    if (maus1 == 0 || maus2 == 0) {
        return ALIAS_UNKNOWN;
    }

    // if addresses are the same.
    if (index1 == index2) {
        // partial? 
        if (maus1 != maus2) {
            return ALIAS_UNKNOWN;
        } else {
            return ALIAS_TRUE;
        }
    }
    
    int maus = std::max(maus1, maus2);

    // partial alias or not?
    if ((index1 & (~(maus-1))) == (index2 & (~(maus-1)))) {
        return ALIAS_UNKNOWN;
    } else {
        return ALIAS_FALSE;
    }
}
/**
 * Returns the size of memory oepration memory access in mau's.
 * if unknown, returns 0.
 */
unsigned int
MemoryAliasAnalyzer::mausOfOperation(const Operation& op) {
    
    // no mem address if does not use memory
    if (!op.usesMemory()) {
        throw InvalidData(__FILE__,__LINE__,__func__, "does not use mem!");
    }

    // loops thru all inputs, checks if it is address.
    // the index for op.input() starts from 0 
    for (int i = 0; i < op.numberOfInputs(); i++) {
        Operand& operand = op.input(i);
        if (operand.isAddress()) {
            return operand.memoryUnits();
        }
    }

    // TODO: assumes only operation of dag uses memory.
    // like base + offset stores/loads.
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& oDag = op.dag(i);
        OperationDAG::NodeSet tailNodes = oDag.endNodes();
        for (OperationDAG::NodeSet::iterator j = tailNodes.begin();
             j != tailNodes.end(); j++) {
            OperationDAGNode* n = *j;
            OperationNode* on = dynamic_cast<OperationNode*>(n);
            if (on != NULL) {
                Operation& tailOp = on->referencedOperation();
                if (tailOp.usesMemory()) {
                    return mausOfOperation(tailOp);
                }
            }
        }
    }
    // unknown or no specified
    return 0;
}

/**
 * Gets the direct address-writing move of a ProgramOperation which is a memory
 * operation.
 *
 * If none found, return null
 *
 * @param po programOperation whose address write move is being searched.
 * @return MoveNode which writes address to a mem operation or NULL.
 */
const MoveNode*
MemoryAliasAnalyzer::addressOperandMove(
    const ProgramOperation& po) {
    const Operation& op = po.operation();

    // no mem address if does not use memory
    if (!op.usesMemory()) {
        return NULL;
    }

    // loops thru all inputs, checks if it is an address.
    // the index for op.input() starts from 0
    for (int i = 0, inputCount = op.numberOfInputs() ; i < inputCount; i++) {
        Operand& operand = op.input(i);
        if (operand.isAddress()) {
            // the index for po.inpuNode starts from 0 , so add +1
            MoveNodeSet& mns = po.inputNode(i+1);
            if (mns.count() != 1) {
                return NULL;
            } else {
                return &mns.at(0);
            }
        }
    }
    return NULL;
}


MemoryAliasAnalyzer::TwoPartAddressOperandDetection 
MemoryAliasAnalyzer::findTwoPartAddressOperands(
    const ProgramOperation& po) {
    
    TwoPartAddressOperandDetection result;
    //   std::set<int> operandIndeces;
    
    const Operation& op = po.operation();
    // TODO: assumes only one operation of dag uses memory.
    // like base + offset stores/loads.        
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& oDag = op.dag(i);
        OperationDAG::NodeSet tailNodes = oDag.endNodes();
        for (OperationDAG::NodeSet::iterator j = tailNodes.begin();
             j != tailNodes.end(); j++) {
            OperationDAGNode* n = *j;
            OperationNode* on = dynamic_cast<OperationNode*>(n);
            if (on == NULL) {
                continue;
            }
            Operation& tailOp = on->referencedOperation();
            if (!tailOp.usesMemory()) {
                continue;
            }
            int addrOperandNum = 0;
            for (int k = 0, inputCount = tailOp.numberOfInputs() ; 
                 k < inputCount; k++) {
                Operand& operand = tailOp.input(k);
                if (operand.isAddress()) {
                    addrOperandNum = k+1;
                }
            }
            if (addrOperandNum == 0) {
                continue;
            }
            
            // now we know the address operand of the mem op in dag.
            
            OperationDAGNode* addressSource = NULL;
            for (int k = 0; k < oDag.inDegree(*n); k++) {
                OperationDAGEdge& e = oDag.inEdge(*n,k);
                if (e.dstOperand() == addrOperandNum) {
                    addressSource = &oDag.tailNode(e);
                }
            }
            if (addressSource == NULL) {
                continue;
            }
            
            OperationNode* addrSrcOn = dynamic_cast<OperationNode*>(
                addressSource);
            if (addrSrcOn == NULL) {
                continue;
            }
            Operation& addrOp = addrSrcOn->referencedOperation();
            if (addrOp.name() == "add" ||
                addrOp.name() == "ADD") {
                result.offsetOperation = 
                    TwoPartAddressOperandDetection::ADD;
            } else {
                if (addrOp.name() == "sub" ||
                    addrOp.name() == "SUB") {
                    result.offsetOperation = 
                        TwoPartAddressOperandDetection::SUB;
                } else {
                    result.offsetOperation = 
                        TwoPartAddressOperandDetection::NOT_FOUND;
                    continue;
                }
            }
            OperationDAG::NodeSet addrInputs = oDag.predecessors(
                *addressSource);
            
            if (addrInputs.size() != 2) {
                result.clear();
                continue;
            }
            for (OperationDAG::NodeSet::iterator k = addrInputs.begin();
                 k != addrInputs.end(); k++) {
                TerminalNode* tn = dynamic_cast<TerminalNode*>(*k);
                if (tn == NULL) {
                    break;
                }
                if (result.operand1 == 0) {
                    result.operand1 = tn->operandIndex();
                } else {
                    result.operand2 = tn->operandIndex();
                }
            }
            if (result.operand2 != 0) {
                return result;
            } else {
                result.clear();
            }
        }
    }
    return result;
}
