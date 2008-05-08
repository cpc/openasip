/**
 * @file TerminalNode.hh
 *
 * Declaration of TerminalNode class.
 *
 * Represents input and output values of operation, 
 * which can be reconnected to other operations.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_NODE_HH
#define TTA_TERMINAL_NODE_HH

#include "OperationDAGNode.hh"

/**
 * Input and output node of the operation DAG.
 *
 * A MoveNode represents one input or output value of 
 * an operation.
 */
class TerminalNode : public OperationDAGNode {
public:

    TerminalNode(int operandIndex);
    TerminalNode(const TerminalNode& other);

    virtual ~TerminalNode() {}

    virtual GraphNode* clone() const; 

    virtual int operandIndex() const;
    
    virtual std::string toString() const;

private:
    int operandIndex_;
};

#endif
