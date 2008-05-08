/**
 * @file ConstantNode.hh
 *
 * Declaration of ConstantNode class.
 *
 * Represents constant internal inputs of in operation DAG
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONSTANT_NODE_HH
#define TTA_CONSTANT_NODE_HH

#include "OperationDAGNode.hh"

/**
 * Constant input (not listed in IOs) node of the operation DAG.
 */
class ConstantNode : public OperationDAGNode {
public:

    ConstantNode(int value);

    ConstantNode(const ConstantNode& other);

    virtual ~ConstantNode() {}

    virtual GraphNode* clone() const; 

    virtual int value() const;

    virtual std::string toString() const;

private:
    int value_;
};

#endif
