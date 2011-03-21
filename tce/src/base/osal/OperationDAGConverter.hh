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
 * @file OperationDAGConverter.hh
 *
 * Declaration of OperationDAGConverter class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_CONVERTER_HH
#define TTA_OPERATION_DAG_CONVERTER_HH

#include <string>
#include <set>
#include <map>

#include <Operand.hh>

class OperationDAG;
class OperationDAGNode;
class OperationPimpl;

/**
 * Parser for creating Operation DAG from Operation DAG Language and
 * for writing Operation DAG back to language form.
 */
class OperationDAGConverter {
public:
    static OperationDAG* createDAG(const OperationPimpl& operation, std::string sourceCode);
    static std::string createOsalCode(const OperationDAG& dag);
    static std::string createSimulationCode(
        const OperationDAG& dag, 
        std::vector<std::string>* varReplacements = NULL);

private:
    typedef std::pair<const OperationDAGNode*,int> VariableKey;

    static std::string castedVar(
        std::string var, Operand::OperandType type);
    
    static bool writeNode(
        std::string& retVal,
        const OperationDAG& dag, const OperationDAGNode& node,
        std::map<VariableKey, std::string>& varBindings,
        std::set<const OperationDAGNode*>& alreadyHandled,
        int& tempVarCount,
        std::set<const OperationDAGNode*>& currentlyHandling,
        std::map<std::string, std::string>* opReplace = NULL,
        std::vector<std::string>* varReplacements = NULL);
};

#endif
