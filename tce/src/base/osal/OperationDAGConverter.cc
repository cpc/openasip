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
 * @file OperationDAGConverter.cc
 *
 * Implementation of OperationDAGConverter class.
 *
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#include "OperationDAGConverter.hh"
#include "OperationDAGBuilder.hh"
#include "OperationDAGLanguageParser.hh"


#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "OperationPool.hh"
#include "OperationNode.hh"
#include "OperationDAGEdge.hh"
#include "TerminalNode.hh"
#include "Operation.hh"
#include "TCEString.hh"

/**
 * Creates OperationDAG out of OSAL DAG language source code.
 *
 * @param sourceCode OSAL DAG Language source code.
 * @return Dynamically allocated OperationDAG instance.
 * @exception IllegalParameters There was an error during parsing.
 */
OperationDAG*
OperationDAGConverter::createDAG(std::string sourceCode) {
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
        OperationDAGBuilder builder(*retVal, *root);
    
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
        
        if (termNode != NULL) {
            // Node is input or output terminal. Either set 
            // variablename e.g. IO(1) to node to varBindings or 
            // write variable
            // value to output node e.g. "IO(4) = tmp12;\n"
            
            std::string ioName;
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
            
            if (dag.inDegree(*termNode) == 0) {
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

                writeNode(retVal, dag, tail, varBindings, alreadyHandled, tempVarCount, currentlyHandling, opReplace, varReplacements);
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

            assert(opNode != NULL && 
                   "Must be either OperationNode or Terminal Node.");

            Operation &refOp = opNode->referencedOperation();
            
            DEBUG_CODE(std::cerr << recursion_level 
                      << "Started handling opnode " << int(&node) << ":" 
                       << currentStepsToRoot << ": " + refOp.name() + "\n";);

            // go through in edges, copy bindings and create input string for
            // EXEC_OPERATION
            
            std::vector<std::string> operandVec(
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
                
                VariableKey sourceKey(&tail, srcOperand);
                
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

                std::string rightSide = 
                    castedVar(operandVec[0], refOp.operand(1).type());

                // if unary operator
                if (refOp.numberOfInputs() == 1) {
                    rightSide = simOp + rightSide;
                } else {                   
                    for (int i = 1; i < refOp.numberOfInputs(); i++) {
                        rightSide += " " + simOp + " " + 
                            castedVar(operandVec[i], 
                                      refOp.operand(i+1).type());
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

std::string 
OperationDAGConverter::castedVar(
    std::string var, Operand::OperandType type) {

    switch (type) { 
    case Operand::SINT_WORD:
        return "static_cast<SIntWord>(" + var + ".value_.sIntWord)";
    case Operand::UINT_WORD:
        return "static_cast<UIntWord>(" + var + ".value_.uIntWord)";
    case Operand::FLOAT_WORD:
        return "static_cast<FloatWord>(" + var + ".value_.floatWord)";
    case Operand::DOUBLE_WORD:
        return "static_cast<DoubleWord>(" + var + ".value_.doubleWord)";
    default:
        return var;
    }
}


/**
 * 
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

    writeNode(retVal, dag, dag.node(0), varBindings, 
              alreadyHandled, tempVarCount, currentlyHandling);
    
    return retVal;
}

/**
 *
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
        
    writeNode(retVal, dag, dag.node(0), varBindings, 
              alreadyHandled, tempVarCount, currentlyHandling,
              &opReplacements, varReplacements);
    
    return retVal;
}
