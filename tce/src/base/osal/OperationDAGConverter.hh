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

/**
 * Parser for creating Operation DAG from Operation DAG Language and
 * for writing Operation DAG back to language form.
 */
class OperationDAGConverter {
public:
    static OperationDAG* createDAG(std::string sourceCode);
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
