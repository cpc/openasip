/**
 * @file ControlDependenceEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * control dependence edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_DEPENDENCE_EDGE_HH
#define TTA_CONTROL_DEPENDENCE_EDGE_HH

#include <string>

#include "GraphEdge.hh"

/**
 * Edge of the control dependence graph (control dependence edge). Each edge
 * implies control dependency.
 */
class ControlDependenceEdge : public GraphEdge {
public:
    friend class ProgramDependenceGraph;
    enum CDGEdgeType {
        CDEP_EDGE_NORMAL,
        CDEP_EDGE_TRUE,
        CDEP_EDGE_FALSE};

    ControlDependenceEdge(
        CDGEdgeType edgeType = CDEP_EDGE_NORMAL) :
        edgeType_(edgeType) {}

    virtual ~ControlDependenceEdge();

    std::string toString() const;
    bool isNormalEdge() const;
    bool isTrueEdge() const;
    bool isFalseEdge() const;
    CDGEdgeType edgeType() const;
    
protected:
    void invertEdgePredicate();       
private:
    CDGEdgeType edgeType_;
};

#endif
