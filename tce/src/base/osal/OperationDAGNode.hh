/**
 * @file OperationDAGNode.hh
 *
 * Declaration of OperationDAGNode class.
 *
 * Base for all OperationDAG nodes.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_NODE_HH
#define TTA_OPERATION_DAG_NODE_HH

#include <string>

#include "GraphNode.hh"

/**
 * Base class for all operation DAG nodes.
 */
class OperationDAGNode : public GraphNode {
public:
    virtual ~OperationDAGNode() {}
};

#endif
