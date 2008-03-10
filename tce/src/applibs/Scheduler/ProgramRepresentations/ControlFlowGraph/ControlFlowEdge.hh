/**
 * @file ControlFlowEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_FLOW_EDGE_HH
#define TTA_CONTROL_FLOW_EDGE_HH

#include <string>

#include "GraphEdge.hh"

/**
 * Edge of the control flow graph (control flow edge). Each edge implies a
 * possible change of execution flow from the source basic block to the
 * target basic block.
 *
 */
class ControlFlowEdge : public GraphEdge {
public:
    enum CFGEdgePredicate {
        CFLOW_EDGE_NORMAL,
        CFLOW_EDGE_TRUE,
        CFLOW_EDGE_FALSE,
	CFLOW_EDGE_LOOP_BREAK
    };

    ControlFlowEdge(
        const int edgeID,
        CFGEdgePredicate edgePredicate = CFLOW_EDGE_NORMAL,
        bool isJumpEdge = true);

    virtual ~ControlFlowEdge();

    bool isControlFlowEdge() const;
    bool isNormalEdge() const;
    bool isTrueEdge() const;
    bool isFalseEdge() const;
    bool isCallPassEdge() const;
    bool isJumpEdge() const;
    bool isLoopBreakEdge() const;
    std::string toString() const;

private:
    CFGEdgePredicate edgePredicate_;
    bool isJumpEdge_;
};

#endif
