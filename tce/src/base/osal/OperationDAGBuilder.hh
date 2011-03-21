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
 * @file OperationDAGBuilder.hh
 *
 * Declaration of OperationDAGBuilder class.
 *
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OperationDAGLanguageParser.hh"

#include "OperationPool.hh"
#include "OperationNode.hh"
#include "OperationDAGEdge.hh"
#include "TerminalNode.hh"
#include "Operation.hh"

/**
 * Builds OperationDAG from parsed and tokenized OSAL code.
 */
class OperationDAGBuilder {
public:
    OperationDAGBuilder(
        const OperationPimpl& operation,
        OperationDAG& dag, 
        const TokenizerData::TokenTreeNode& root);

    void parse();
    
private:
    /// Node and operand which is referred by a variable.
    typedef std::pair<OperationDAGNode*, int> VariableBinding;

    /// Information of object and index of operand of a variable.
    typedef std::pair<TerminalNode*, int> TerminalBinding;
    
    void parseNode(const TokenizerData::TokenTreeNode* node);
    void createOperationNode(const std::string& operation);

    void connectOperandToNode(
        const TokenizerData::TokenTreeNode* var, int operandIndex);

    void finalize();

    void declareVariable(const TokenizerData::TokenTreeNode* var);

    void assignVariable(
        const TokenizerData::TokenTreeNode* dst, VariableBinding& src);
    void assignVariable(
        const TokenizerData::TokenTreeNode* dst,
        const TokenizerData::TokenTreeNode* src);
    
    std::string getVariableName(const TokenizerData::TokenTreeNode* var);

    VariableBinding& getBinding(const TokenizerData::TokenTreeNode* var);
    VariableBinding& getBinding(std::string varName);
    VariableBinding& getBinding(int operandIndex);
    VariableBinding& getConstantBinding(int value);

    int getIOOperand(const TokenizerData::TokenTreeNode* var);

    /// DAG where all nodes and edges are added.
    OperationDAG* dag_;
    
    /// Root of tokenized data parsed from OSAL DAG source code.
    const TokenizerData::TokenTreeNode& rootNode_;

    /// Currently created handled operation.
    OperationNode* currentOperation_;
    
    /// Node and operand which are referred by IO(x) or declared variable. 
    std::map<std::string, VariableBinding> variableBindings_;

    /// Node representing constant value. 
    std::map<int, VariableBinding> constantBindings_;
    
    /// IO(x) variables and corresponding TerminalNodes and operand indices.
    std::map<std::string, TerminalBinding> ioVariables_;

    const OperationPimpl& operation_;
};
