/**
 * @file OperationDAGConverter.hh
 *
 * Declaration of OperationDAGConverter class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
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
