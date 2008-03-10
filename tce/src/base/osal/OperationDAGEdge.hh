/**
 * @file OperationDAGEdge.hh
 *
 * Declaration of operation directed acyclic graph edge.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_EDGE_HH
#define TTA_OPERATION_DAG_EDGE_HH

#include <string>

#include "GraphEdge.hh"

class OperationDAGEdge : public GraphEdge {
public:
    OperationDAGEdge(int srcOperand, int dstOperand);
    OperationDAGEdge(const OperationDAGEdge& other);
    virtual ~OperationDAGEdge() {} 
    
    virtual GraphEdge* clone() const;
    
    int srcOperand() const;
    int dstOperand() const;

    std::string toString() const;    

private:
    int srcOperand_;
    int dstOperand_;
};

#endif
