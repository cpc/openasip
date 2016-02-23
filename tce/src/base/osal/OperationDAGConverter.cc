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
 * @file OperationDAGConverter.cc
 *
 * Implementation of OperationDAGConverter class.
 *
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OperationDAGConverter.hh"
#include "OperationDAGBuilder.hh"
#include "OperationDAGLanguageParser.hh"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-local-typedef")
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
POP_COMPILER_DIAGS

#include "OperationPool.hh"
#include "OperationNode.hh"
#include "OperationDAGEdge.hh"
#include "TerminalNode.hh"
#include "ConstantNode.hh"
#include "Operation.hh"
#include "TCEString.hh"
#include "OperationDAG.hh"

/**
 * Creates OperationDAG out of OSAL DAG language source code.
 *
 * @param sourceCode OSAL DAG Language source code.
 * @return Dynamically allocated OperationDAG instance.
 * @exception IllegalParameters There was an error during parsing.
 */
OperationDAG*
OperationDAGConverter::createDAG(const OperationPimpl& operation, std::string sourceCode) {
    OperationDAGLanguageGrammar g;
    skip_grammar skip;
  
    // lets add semicolon to end of file to make boost 1.34 happy
    sourceCode += "\n;";

    const char *orig = sourceCode.c_str();
  
    //std::cerr << "source code" << std::endl << sourceCode << std::endl;
    
    parse_info<const char*> result = 
        parse(orig, g, skip); 
 
    if (result.full) {
        const TokenizerData::TokenTreeNode* root = g.tokenData_.tokenTree();
        OperationDAG* retVal = new OperationDAG();
        OperationDAGBuilder builder(operation, *retVal, *root);
    
        //    std::cerr << g.tokenData_.tokenTree()->toStr() << std::endl;
    
        builder.parse();        
        return retVal;	
    
    } else {
        for (unsigned int i = 0; i < skip.strippedParts.size(); i++) {      
            skip.strippedParts[i].second -= 
                reinterpret_cast<long int>(skip.strippedParts[i].first);
            
            skip.strippedParts[i].first -= reinterpret_cast<long int>(orig);
      
            //std::cerr << "Skipped " << int(skip.strippedParts[i].second)
            //<< " chars at pos " << int(skip.strippedParts[i].first) 
            //<< " line: \""
            //<< sourceCode.substr(int(skip.strippedParts[i].first),  
            //int(skip.strippedParts[i].second)) << "\"" << std::endl;
        }
    
        // find character count in original code 
        unsigned int charsToPos = 0;
        for (unsigned int i = 0; i < result.length ; i++) {
      
            for (unsigned int j = 0; j < skip.strippedParts.size(); j++) {
                if ((unsigned long int)skip.strippedParts[j].first
                    == charsToPos) {
                    charsToPos += 
                        reinterpret_cast<long int>(skip.strippedParts[j].second);
	  
                    //std::cerr << std::endl << "Added at position: " 
                    //<<  int(skip.strippedParts[j].first) 
                    //<< " to char length: " 
                    //<< int(skip.strippedParts[j].second); 
                }
            }
      
            charsToPos++;	
        }
    
        // find line number 
        int lineNumber = 1;
        for (unsigned int i = 0; i < charsToPos; i++) {
            if (sourceCode.at(i) == '\n') {
                lineNumber++;
            }
        }
    
        // get logical line
        int lineStart = sourceCode.rfind(';', charsToPos);
        int lineEnd = sourceCode.find(';', charsToPos);       
    
        std::string errorLine =
            boost::algorithm::trim_copy(
                sourceCode.substr(lineStart + 1, lineEnd - lineStart));
    
        //     std::cerr << "result.stop: " << result.stop << std::endl 
        // 	      << "result.hit: " << result.hit << std::endl 
        // 	      << "result.full: " << result.full << std::endl 
        // 	      << "result.length: " << result.length << std::endl;
        
        std::string message  = "Parsing failed at the position " + 
            Conversion::toString(result.length) + " in the code.\n" +
            "line  " + Conversion::toString(lineNumber) + ": " + errorLine;
        
        //std::cerr << message << std::endl;
        
        throw IllegalParameters(__FILE__, __LINE__, __func__, message);
    }  
    return NULL;
}


/**
 * Writes node and all the nodes connected as osal code.
 *
 * @param retVal String where to code is written.
 * @param dag Dag that contains the nodes.
 * @param node Currently written node.
 * @param varBindings Names of variables that are bound for TerminalNodes and 
 *                    OperandNode outputs.
 * @param alreadyHandled If node is included to this ser if it is 
 *                       already fully written and all its operands are handled.
 * @param tempVarCount Number of variables created.
 * @param currentlyHandling Node is in this set if it's currently handled in 
 *                          some recursion level.
 * @param opReplace Map of operation names and  C operators that can simulate them.
 * @return false If node is already in middle of handling.
 */
#define DEBUG_CODE(x) 

bool
OperationDAGConverter::writeNode(
    std::string& retVal,
    const OperationDAG& dag, 
    const OperationDAGNode& node,
    std::map<VariableKey, std::string>& varBindings,
    std::set<const OperationDAGNode*>& alreadyHandled,
    int& tempVarCount, 
    std::set<const OperationDAGNode*>& currentlyHandling,
    std::map<std::string, std::string>* opReplace,
    std::vector<std::string>* varReplacements) {

    DEBUG_CODE(static std::string recursion_level = ""; recursion_level += "----";);

    int currentStepsToRoot = dag.stepsToRoot(node);    
    
    if (currentlyHandling.find(&node) != currentlyHandling.end()) {
        DEBUG_CODE(recursion_level = recursion_level.erase(0,4););
        return false;
    }
    
    if (alreadyHandled.find(&node) == alreadyHandled.end()) {
        
        currentlyHandling.insert(&node);
        
        const TerminalNode* termNode = 
            dynamic_cast<const TerminalNode*>(&node);
        const ConstantNode* constNode = 
            dynamic_cast<const ConstantNode*>(&node);

        if (constNode != NULL) {
            currentlyHandling.erase(constNode);
            alreadyHandled.insert(constNode);
            VariableKey termKey(constNode, 0);
            varBindings[termKey] = constNode->toString();
            return true;
        } else if (termNode != NULL) {
            // Node is input or output terminal or a const. Either set 
            // variablename e.g. IO(1) to node to varBindings or 
            // write variable
            // value to output node e.g. "IO(4) = tmp12;\n"
            
            std::string ioName = "";
            if (!varReplacements || 
                (termNode->operandIndex() - 1) > (int)varReplacements->size()) {
                ioName = "IO(" +  
                    Conversion::toString(termNode->operandIndex()) + ")";
            } else {
                ioName = varReplacements->at(termNode->operandIndex() - 1);
            }
            
            VariableKey termKey(termNode, termNode->operandIndex());
            
            DEBUG_CODE(std::cerr << recursion_level 
                       << "**** Started handling term node " << int(&node) 
                       << ":" << currentStepsToRoot << ":" + ioName + "\n";);
            
            const bool inputTerminalNode = dag.inDegree(*termNode) == 0;
            if (inputTerminalNode) {
                // set veriable binding for reading terminal value
                varBindings[termKey] = ioName;                
                DEBUG_CODE(std::cerr << recursion_level 
                           << "Added input terminal: " + ioName + "\n";);
                
            } else {
                
                assert(dag.outDegree(*termNode) == 0);
                
                OperationDAGEdge& srcEdge = dag.inEdge(*termNode, 0);
                OperationDAGNode& tail = dag.tailNode(srcEdge);
              
                DEBUG_CODE(std::cerr << recursion_level 
                           << "** Read input of terminal " << ioName << "\n";);

                writeNode(
                    retVal, dag, tail, varBindings, alreadyHandled, 
                    tempVarCount, currentlyHandling, opReplace, varReplacements);
                DEBUG_CODE(std::cerr << recursion_level 
                           << "** Ready reading input of terminal " << ioName << "\n";);
                VariableKey srcKey(&tail, srcEdge.srcOperand());

                // write result to output terminal
                retVal += ioName + " = " + varBindings[srcKey] + ";\n";
            }

            DEBUG_CODE(std::cerr << recursion_level 
                       << "added terminal " << int (&node) 
                       << "to already handled nodes\n";);
            
            currentlyHandling.erase(termNode);
            alreadyHandled.insert(termNode);
            
        } else {
            // Node is operation node. Read inputs and create destination
            // variables for outputs. In the end write EXEC_OPERATION
            // line to code.
            
            const OperationNode* opNode = 
                dynamic_cast<const OperationNode*>(&node);
            
            if (opNode == NULL) {
                abortWithError(
                    (boost::format(
                        "Must be either OperationNode or Terminal Node. "
                        "Got: %s.") % node.toString()).str());
            }

            Operation &refOp = opNode->referencedOperation();
            
            DEBUG_CODE(std::cerr << recursion_level 
                      << "Started handling opnode " << int(&node) << ":" 
                       << currentStepsToRoot << ": " + refOp.name() + "\n";);

            // go through in edges, copy bindings and create input string for
            // EXEC_OPERATION
            
            std::vector<std::string> operandVec(
                refOp.numberOfInputs() + refOp.numberOfOutputs());

            // kludge: this vector has 'true' in the operand's
            // position only if the operand is a constant and
            // thus needs not to be casted to IntWord with
            // castedVar later when generating the operation
            // parameters
            std::vector<bool> isConstOperand(
                refOp.numberOfInputs() + refOp.numberOfOutputs());
            
            // input parameters
            for (int i = 0; i < dag.inDegree(node); i++) {
                OperationDAGEdge& edge = dag.inEdge(node, i);
                OperationDAGNode& tail = dag.tailNode(edge);
                
                const TerminalNode* termTail = 
                    dynamic_cast<const TerminalNode*>(&tail);
                
                int srcOperand;
                if (termTail != 0) {
                    srcOperand = termTail->operandIndex();
                } else {
                    srcOperand = edge.srcOperand();
                }
                
                VariableKey sourceKey;
                if (dynamic_cast<const ConstantNode*>(&tail) != NULL) {
                    sourceKey = VariableKey(&tail, 0);
                    isConstOperand[edge.dstOperand()-1] = true;
                } else {
                    sourceKey = VariableKey(&tail, srcOperand);
                    isConstOperand[edge.dstOperand()-1] = false;
                }
                
                // if input not already handled, treat it first.
                DEBUG_CODE(std::cerr << recursion_level 
                           << "** Read input op: " << refOp.name() << ":" << i << "\n";);

                if (!writeNode(retVal, dag, tail, varBindings, 
                               alreadyHandled, tempVarCount, currentlyHandling, opReplace, varReplacements)) {

                    DEBUG_CODE(std::cerr << recursion_level 
                               << "Input can't be read yet: " << refOp.name() << ":" << i << "\n";);

                    currentlyHandling.erase(&node);

                    DEBUG_CODE(recursion_level = recursion_level.erase(0,4););
                    return false;
                }

                DEBUG_CODE(std::cerr << recursion_level 
                           << "** Ready reading input of op: " << refOp.name() << ":" << i << "\n";);
            
                // add input to correct place in EXEC_OPERATION parameter list
                if (varBindings[sourceKey].empty()) {
                    throw IllegalParameters(
                        __FILE__, __LINE__, __func__, 
                        std::string(
                            "DAG cannot be written to OSAL code, because illegal output edge. "
                            "Edge srcOperand: ") + Conversion::toString(srcOperand));
                }
                
                operandVec[edge.dstOperand()-1] = varBindings[sourceKey];

                DEBUG_CODE(std::cerr << recursion_level 
                           << "Added operand: " << varBindings[sourceKey] 
                           << " key: (" << sourceKey.first << ":" << sourceKey.second << ")"  
                           << " to " << refOp.name() << " paramer vector index: " 
                           << edge.dstOperand() - 1 <<  std::endl;);
            }
            
            // and outputs
            for (int i = 0; i < refOp.numberOfOutputs(); i++) {
                int opNumber = i + refOp.numberOfInputs() + 1;
                VariableKey operandKey(&node, opNumber);
                tempVarCount++;
                std::string tempVarName = "tmp" + 
                    Conversion::toString(tempVarCount);
                retVal = "SimValue " + tempVarName + ";\n" + retVal;
                varBindings[operandKey] = tempVarName;
                
                DEBUG_CODE(std::cerr << recursion_level 
                           << "Created variable: " << tempVarName 
                           << " key: (" << operandKey.first << ":" << operandKey.second << ")"  
                           << " for " << refOp.name() << ":" << opNumber << std::endl;);
                    
                // add output to  EXEC_OPERATION parameter list
                operandVec[opNumber-1] = varBindings[operandKey];
                DEBUG_CODE(std::cerr << recursion_level 
                          << "Added operand: " << varBindings[operandKey]
                          << " to " << refOp.name() << " paramer vector index: " 
                           << opNumber -1 <<  std::endl;);
            }        
        
            // this node is processed.
            DEBUG_CODE(std::cerr << recursion_level << "added node " 
                       << int (&node) << "to already handled nodes\n" ;);

            currentlyHandling.erase(&node);
            alreadyHandled.insert(&node);

            // Write the execute operation code
            if (opReplace != NULL && 
                opReplace->find(refOp.name()) != opReplace->end()) {
                std::string simOp = (*opReplace)[refOp.name()];

                std::string rightSide = "";
                if (isConstOperand[0]) {
                    // the consts need not to be casted
                    rightSide = operandVec[0];
                } else {
                    rightSide = castedVar(operandVec[0], refOp.operand(1).type());
                }

                // if unary operator
                if (refOp.numberOfInputs() == 1) {
                    rightSide = simOp + rightSide;
                } else {                   
                    for (int i = 1; i < refOp.numberOfInputs(); i++) {
                        if (isConstOperand[i]) {
                            rightSide += 
                                " " + simOp + " " + operandVec[i];
                        } else {
                            rightSide += 
                                " " + simOp + " " + 
                                castedVar(
                                    operandVec[i], 
                                    refOp.operand(i+1).type());
                        }
                    }
                }
                
                assert(refOp.numberOfOutputs() == 1 &&
                       "Cant write simulation replacement code only"
                       "for singleoutput operations.");
                
                // assing right side to first outputoperand.
                retVal += operandVec[refOp.numberOfInputs()] + 
                    " = " + rightSide + ";\n";

            } else {
                retVal += "{ EXEC_OPERATION(" + std::string(refOp.name());
                for (unsigned int i = 0; i < operandVec.size(); i++) {
                    retVal += ", " + operandVec[i];
                }            
                retVal +=  "); } \n";        

                DEBUG_CODE(std::cerr << recursion_level 
                           << "Added EXEC: " + refOp.name() << std::endl;);
            }
            
            // write outgoing nodes  
            for (int i = 0; i < dag.outDegree(node); i++) {
                OperationDAGNode& headNode = 
                    dag.headNode(dag.outEdge(node,i));
                
                // handle only those nodes, which are one step away from 
                // curren node.
                if (dag.stepsToRoot(headNode) == currentStepsToRoot + 1) {
                    DEBUG_CODE(std::cerr << recursion_level 
                               << "** Goto output op: " << refOp.name() 
                               << ":" << i << "\n";);

                    writeNode(retVal, dag, headNode, varBindings, 
                              alreadyHandled, tempVarCount, currentlyHandling, opReplace, varReplacements);

                    DEBUG_CODE(std::cerr << recursion_level 
                               << recursion_level 
                               << "** Ready going output op: " << refOp.name() 
                               << ":" << i << "\n";);
                }
            }
        }
    }

    DEBUG_CODE(recursion_level = recursion_level.erase(0,4););
    return true;
}

/**
 * Cast variable to be correct type.
 */
std::string 
OperationDAGConverter::castedVar(
    std::string var, Operand::OperandType type) {

    switch (type) { 
    case Operand::SINT_WORD:
        return var + ".sIntWordValue()";
    case Operand::UINT_WORD:
        return var + ".uIntWordValue()";
    case Operand::FLOAT_WORD:
        return var + ".floatWordValue()";
    case Operand::HALF_FLOAT_WORD:
        return var + ".halfFloatWordValue()";
    case Operand::DOUBLE_WORD:
        return var + ".doubleWordValue()";
    default:
        return var;
    }
}


/**
 * Write OSAL DAG code for graph.
 *
 * @param dag Graph to write as OSAL code (basically C subset)
 */    
std::string 
OperationDAGConverter::createOsalCode(const OperationDAG& dag) {
    std::string retVal; 
    std::map<VariableKey, std::string> varBindings;
    std::set<const OperationDAGNode*> alreadyHandled;
    std::set<const OperationDAGNode*> currentlyHandling;
    int tempVarCount = 0;
    
    if (dag.nodeCount() == 0) {
        return retVal;
    }

    // Writes recursively all the DAG from node(0)
    writeNode(retVal, dag, dag.node(0), varBindings, 
              alreadyHandled, tempVarCount, currentlyHandling);
    
    return retVal;
}

/**
 * Optimizations, when compiled simulation code is created from DAG.
 *
 * OSAL implementations of operantions will not be called for calculating
 * basic C operations.
 */
std::string 
OperationDAGConverter::createSimulationCode(
    const OperationDAG& dag,
    std::vector<std::string>* varReplacements) {
    std::string retVal; 
    std::map<VariableKey, std::string> varBindings;
    std::set<const OperationDAGNode*> alreadyHandled;
    std::set<const OperationDAGNode*> currentlyHandling;
    int tempVarCount = 0;
    std::map<std::string, std::string> opReplacements;    
    
    opReplacements["ADD"] = "+";
    opReplacements["SUB"] = "-";
    opReplacements["MUL"] = "*";
    opReplacements["DIV"] = "/";
    opReplacements["DIVU"] = "/";    
    opReplacements["EQ"] = "==";
    opReplacements["GT"] = ">"; 
    opReplacements["GTU"] = ">";
    opReplacements["SHL"] = "<<";
    opReplacements["SHR"] = ">>";
    opReplacements["SHRU"] = ">>";
    opReplacements["AND"] = "&";
    opReplacements["IOR"] = "|";
    opReplacements["XOR"] = "^";
    opReplacements["NEG"] = "-";
    opReplacements["NEGF"] = "-";
    opReplacements["ADDF"] = "+";
    opReplacements["SUBF"] = "-";
    opReplacements["MULF"] = "*";
    opReplacements["DIVF"] = "/";
    opReplacements["EQF"] = "==";
    opReplacements["GTF"] = ">";
    opReplacements["CFI"] = "(UIntWord)";
    opReplacements["CIF"] = "(FloatWord)";
    opReplacements["CFD"] = "(DoubleWord)";
    opReplacements["CDF"] = "(FloatWord)";
    opReplacements["MOD"] = "%";
    opReplacements["MODU"] = "%";
    opReplacements["MULH"] = "*";

    // Writes recursively all the DAG from node(0)
    // But this recursion does not work so added the loop
    // to make sure all nodes get processed.
    OperationDAGNode* node;
    for (int i = 0; i < dag.nodeCount(); i++) {
        node = &dag.node(i);
        if (alreadyHandled.find(node) == alreadyHandled.end()) {
            writeNode(retVal, dag, *node, varBindings, 
                      alreadyHandled, tempVarCount, currentlyHandling,
                      &opReplacements, varReplacements);
        }
    }
    return retVal;
}
