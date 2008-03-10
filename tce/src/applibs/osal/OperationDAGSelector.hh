/**
 * @file OperationDAGSelector.hh
 *
 * Declaration of OperationDAGSelector.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_SELECTOR_HH
#define TTA_OPERATION_DAG_SELECTOR_HH

#include <string>
#include <vector>
#include <set>

#include "OperationDAG.hh"
#include "Operation.hh"

/**
 * Class that search DAGs from operation set.
 */
class OperationDAGSelector {
public:
    
    /**
     * STL Container for returning and finding DAGs.
     */
    class OperationDAGList : public std::vector<OperationDAG*> {
    public:
        
        /**
         * Returns DAG that had least nodes.
         *
         * @return DAG that had least nodes.
         */
        OperationDAG& smallestNodeCount() const {

            if (empty()) {
                return OperationDAG::null;
            }

            OperationDAG* leastNodes = *begin();

            for (std::vector<OperationDAG*>::const_iterator i = begin(); 
                 i != end(); i++) {
                
                if ((*i)->nodeCount() < leastNodes->nodeCount()) {
                    leastNodes = *i;
                }
            }
            
            return *leastNodes;
        }        
    };
    
    /// @todo this should be gotten from tta machine applib, machine function
    /// for requesting native opset of a machine.
    typedef std::set<std::string> OperationSet;

    static OperationDAGList findDags(
        std::string opName, OperationSet opSet);
    
    static OperationDAG* createExpandedDAG(
        const Operation& op, OperationSet& opSet);
    
    static OperationSet llvmRequiredOpset();

private:
    OperationDAGSelector();
    virtual ~OperationDAGSelector();

    static int countUnknownOperations(
        OperationDAG& dag, OperationSet& opSet);

};

#endif
