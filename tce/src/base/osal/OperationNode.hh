/**
 * @file OperationNode.hh
 *
 * Declaration of OperationNode class.
 *
 * Represents one operation which might be constructed out of other operations
 * or of a OSAL opartion.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_NODE_HH
#define TTA_OPERATION_NODE_HH

#include "OperationDAGNode.hh"

class OperationDAG;
class Operation;

/**
 * One operation node of the operation DAG.
 */
class OperationNode : public OperationDAGNode {
public:
    OperationNode(Operation& operation);
    OperationNode(const OperationNode& other);
    
    virtual GraphNode* clone() const; 

    virtual ~OperationNode() {}
    
    Operation& referencedOperation() const;
    virtual std::string toString() const;
        
private:
    Operation* referencedOperation_;
};

#endif
