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
 * @file OperationDAGBuilder.hh
 *
 * Declaration of OperationDAGBuilder class.
 *
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
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
        OperationDAG& dag, const TokenizerData::TokenTreeNode& root);

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
};
