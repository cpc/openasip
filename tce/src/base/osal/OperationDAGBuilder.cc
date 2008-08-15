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
 * @file OperationDAGBuilder.cc
 *
 * Declaration of OperationDAGBuilder class.
 *
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#include "OperationDAGBuilder.hh"
#include "OperationDAGLanguageParser.hh"

#include "OperationPool.hh"
#include "OperationNode.hh"
#include "OperationDAGEdge.hh"
#include "TerminalNode.hh"
#include "ConstantNode.hh"
#include "Operation.hh"
#include "Operand.hh"

/**
 * Constructor.
 *
 * @param dag OperationDAG where to create DAG representation of operation.
 * @param root Tokenized OSAL DAG Language code. 
 */
OperationDAGBuilder::OperationDAGBuilder(
    OperationDAG& dag, const TokenizerData::TokenTreeNode& root) : 
    dag_(&dag), rootNode_(root), currentOperation_(NULL) { 
}

/**
 * Parses token tree presentation of OSAL DAG language to OperationDAG.
 */
void 
OperationDAGBuilder::parse() {
    
    // Print out tokenized dag form...
    // std::cerr << rootNode_.toStr() << std::endl;

    // Loop through all leafs of root node and call finalization.        
    for (int i = 0; i < rootNode_.leafCount();i++) {
        const TokenizerData::TokenTreeNode* currNode = 
            &(rootNode_.leaf(i));
        parseNode(currNode);
    }        
    finalize();
}
    
/**
 * Parses one token tree node to DAG representation.
 *
 * @param node Node to parse.
 */
void 
OperationDAGBuilder::parseNode(
    const TokenizerData::TokenTreeNode* node) {        
    
    switch(node->token().type_) {
    case TokenizerData::EXPRESSION_STATEMENT: {
        // ignore empty expressions
        if (node->leafCount() == 1)  {
            parseNode(&node->leaf(0));
        }
    } break;
    
    case TokenizerData::DECLARATION: {
        for (int i = 1; i < node->leafCount(); i++) {
            // TODO: check thata init declarator doesn't 
            //       doesnt't have leafs
            declareVariable(&node->leaf(i));
        }
    } break;
    
    case TokenizerData::ASSIGNMENT_EXPRESSION: {
        if (node->leafCount() == 1) {
            parseNode(&node->leaf(0));
            
        }  else if (node->isAssignment()) {               
            assignVariable(&node->leaf(0), &node->leaf(2));
            
        }  else if (node->isFunctionCall()) {
            // TODO: check if is execution macro                
            // get postfixpart
            node = &node->leaf(1);
            
            // leaf(0) create operation node
            createOperationNode(node->leaf(0).token().stringValue());
            
            // leaf(1)- ... connect nodes..
            for (int i = 1; i < node->leafCount(); i++) {
                connectOperandToNode(&node->leaf(i), i);
            }
            
        } else {
            std::cerr << "Cannot parse: " << node->toStr();
        }
    } break;
    
    default:
        std::cerr << "Failed parsing: " << node->token().stringValue() 
                  << std::endl;
    }
}

/**
 * Starts creation of new operation node.
 *
 * @param operation Name of operation which is referred by this node.
 * @exception IllegalParameters Operation by requested name doesn't exist.
 */
void 
OperationDAGBuilder::createOperationNode(const std::string& operation) {
    OperationPool opPool;
    Operation& op = opPool.operation(operation.c_str());
  
    if (&op == &NullOperation::instance()) {
        throw IllegalParameters(__FILE__, __LINE__, __func__,
                                "Operation: \"" + operation + "\" doesn't exist.");
    }        
    currentOperation_ = new OperationNode(op);
    dag_->addNode(*currentOperation_);        
}

/**
 * Connects a variable node to given operand index of last created
 * operation node.
 *
 * @param var Variable to connect to operand of created operation node.
 * @param operandIndex Index of operand to which variable is connected.
 * @exception IllegalParameters Operand with requested index doesn't exist.
 */
void 
OperationDAGBuilder::connectOperandToNode(
    const TokenizerData::TokenTreeNode* var, int operandIndex) {
    
    // check if input or output
    Operation& currOp = currentOperation_->referencedOperation();        
    
    if (currOp.operand(operandIndex).isInput()) {
        VariableBinding* srcNode = &getBinding(var);
        OperationDAGEdge* newEdge = 
            new OperationDAGEdge(srcNode->second, operandIndex);
        dag_->connectNodes(*srcNode->first, *currentOperation_, *newEdge);
        
    } else if (currOp.operand(operandIndex).isOutput()) {
        // set node, operandIndex pair for the variable
        VariableBinding varBind = 
            VariableBinding(currentOperation_, operandIndex);
        assignVariable(var, varBind);
        
    } else {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__,
            "Operation doesn't have operand number: " + 
            Conversion::toString(operandIndex));
    }
}   

/**
 * Finalizes DAG by creating output terminals.
 *
 * TerminalNode objects are created for Those IO() operands, which are 
 * referred, but doesn't already have TerminalNode objects.
 */
void 
OperationDAGBuilder::finalize() {        
    // find IO(x) references, which doesn't has yet terminal
    for (std::map<std::string,TerminalBinding>::iterator iter = 
             ioVariables_.begin(); iter != ioVariables_.end(); iter++) {
        
        if (iter->second.first == NULL) {                
            VariableBinding& ioVar = getBinding(iter->second.second);
            iter->second.first = new TerminalNode(iter->second.second);
            dag_->addNode(*(iter->second.first));
            OperationDAGEdge* newEdge = 
                new OperationDAGEdge(ioVar.second,1);
            dag_->connectNodes(
                *(ioVar.first), *(iter->second.first), *newEdge);
        }
    }
    
    // TODO: verify IO variable information of operation description
}

/**
 * Declared new variable which can be used for temporary storage of
 * IO() variables.
 * 
 * @param var Token tree node containing variable name.
 * @exception IllegalParameters If variable already exists.
 */
void 
OperationDAGBuilder::declareVariable(
    const TokenizerData::TokenTreeNode* var) {
    std::string varName = getVariableName(var);
    if (variableBindings_.find(varName) != variableBindings_.end()) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__,
            "Variable \"" + var->token().stringValue() + 
            "\" already exists.");
    }
    variableBindings_[varName] = VariableBinding(NULL, 0);
}

/**
 * Assign binding of destination variable to be source binding.
 *
 * @param src Tree node variable whose binding is set to destination.
 * @param dst Binding that is set to source's binding.
 */
void 
OperationDAGBuilder::assignVariable(
    const TokenizerData::TokenTreeNode* dst, VariableBinding& src) {
    std::string dstName = getVariableName(dst);

    if (variableBindings_.find(dstName) == variableBindings_.end() && 
        ioVariables_.find(dstName) == ioVariables_.end()) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__,
            "Variable \"" + dst->token().stringValue() + 
            "\" was not declared.");
    }

    variableBindings_[dstName] = src;
}     

/**
 * Assign binding of destination variable to be same than source.
 *
 * @param src Tree node variable whose binding is set to destination.
 * @param dst Tree node variable whose binding is set to same that source's.
 */
void 
OperationDAGBuilder::assignVariable(
    const TokenizerData::TokenTreeNode* dst,
    const TokenizerData::TokenTreeNode* src) {
    std::string dstName = getVariableName(dst);
    variableBindings_[dstName] = getBinding(src);
}     

/**
 * Retuns name of variable, for token tree node containing IO() call or
 * variable name.
 *
 * @param var IO() call or variable name.
 * @return Generated variable name for internal variable map.
 * @exception IllegalParameter Parameter is not variable..
 */
std::string 
OperationDAGBuilder::getVariableName(
    const TokenizerData::TokenTreeNode* var) {
    // check if variable is function IO(2) or normal var
    if (var->isFunctionCall()) {
        int srcOperand = getIOOperand(var);
        
        std::string retVal = 
            "IO(" + Conversion::toString(srcOperand) + ")";
        
        // mark terminal as referenced, but not created
        if (ioVariables_.find(retVal) == ioVariables_.end()) {
            ioVariables_[retVal] = TerminalBinding(NULL, srcOperand);
        }
        
        return retVal;
        
    } else if (var->token().isIdentifier()) {
        return var->token().stringValue();
        
    } else {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, 
            "Parameter \"" + var->token().stringValue() + 
            "\" is not variable.");
    }
}

/**
 * Returns node and operand index of any existing variable, IO() call or 
 * constant.
 *
 * @param var Funcion call or single identifier type of token tree node.
 * @return Node and operand index of any existing variable or IO() call.
 * @exception IllegalParameter Parameter isn't function call or variable name.
 */
OperationDAGBuilder::VariableBinding& 
OperationDAGBuilder::getBinding(const TokenizerData::TokenTreeNode* var) {
    
    // check if variable is function IO(2) or normal var
    if (var->isFunctionCall()) {
        int srcOperand = getIOOperand(var);
        return getBinding(srcOperand);
        
    } else if (var->token().isIdentifier()) {
        std::string varName = getVariableName(var);
        return getBinding(varName);
        
    } else if (var->isInteger()) {
        return getConstantBinding(var->intValue());

    } else {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, 
            "Parameter \"" + var->token().stringValue() + 
            "\" is not variable.");
    }
}

/**
 * Returns node and operand index of any existing variable. 
 *
 * @param varName Name of the variable whose current binding is requested.
 * @return Node and operand index where variable is currently assigned. 
 * @exception IllegalParameters Variable is not declared.
 */
OperationDAGBuilder::VariableBinding& 
OperationDAGBuilder::getBinding(std::string varName) {        
    if (variableBindings_.find(varName) ==  variableBindings_.end()) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, 
            "Trying to use uninitialized variable: " + varName);
    }
    return variableBindings_[varName];
}

/**
 * Returns node and operand index of any existing variable. 
 *
 * @param varName Name of the variable whose current binding is requested.
 * @return Node and operand index where variable is currently assigned. 
 * @exception IllegalParameters Variable is not declared.
 */
OperationDAGBuilder::VariableBinding& 
OperationDAGBuilder::getConstantBinding(int value) {
    if (constantBindings_.find(value) ==  constantBindings_.end()) {
        ConstantNode* newNode = new ConstantNode(value);
        constantBindings_[value] = VariableBinding(newNode, 1);
        dag_->addNode(*newNode);
    }

    return constantBindings_[value];
}

/**
 * Returns node and operand index of IO() variable. 
 *
 * If called first time creates TerminalNode and binds IO() to that
 * node with operand index 1.
 *
 * @param operandIndex Number of IO whose current binding is requested.
 * @return Node and operand index where IO() is currently assigned. 
 */
OperationDAGBuilder::VariableBinding& 
OperationDAGBuilder::getBinding(int operandIndex) {        
    
    // if first reference, and its never set before, 
    // create startnode for variable and set 
    // outputbind to be 1
    
    // if variable name is used, it has to be created 
    // and initialized before
    std::string varName = 
        "IO(" + Conversion::toString(operandIndex) + ")";        
    
    if (variableBindings_.find(varName) == variableBindings_.end()) {
        TerminalNode* newNode = new TerminalNode(operandIndex);
        variableBindings_[varName] = VariableBinding(newNode, 1);
        dag_->addNode(*newNode);
        
        // mark terminal as created for check phase in finalize
        ioVariables_[varName] = TerminalBinding(newNode, operandIndex);
    }
    
    return getBinding(varName);
}    
    
/**
 * Returns operand's number of tree node, which contains reference to 
 * IO() variable.
 *
 * @param var Token tree node, which contains parsed IO(x) call.
 * @return Operand number of IO(x) type of token tree node. 
 * @exception IllegalParameters If var isn't parsed IO() call.
 */
int 
OperationDAGBuilder::getIOOperand(const TokenizerData::TokenTreeNode* var) {
    if (var->leafCount() != 2 || 
        var->leaf(0).token().stringValue() != "IO" ||  
        !var->leaf(1).leaf(0).token().isIntegerLiteral()) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, 
            "Invalid parameter node \"" + var->token().stringValue() + 
            "\" != \"IO(x)\" call.");
    }
    return var->leaf(1).leaf(0).token().intValue();
}
