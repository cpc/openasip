/**
 * @file ProgramDependenceNode.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */
#ifndef TTA_PROGRAM_DEPENDENCE_NODE_HH
#define PROGRAM_DEPENDENCE_NODE_HH

#include "MoveNode.hh"
#include "ControlDependenceNode.hh"
/**
*/
class ProgramDependenceNode : public GraphNode {
public:
    ProgramDependenceNode(ControlDependenceNode& cdgNode, int nodeID);
    ProgramDependenceNode(
        MoveNode& mNode, 
        int nodeID, 
        bool predicate = false);
    virtual ~ProgramDependenceNode();
   
    bool isRegionNode() const;
    bool isPredicateMoveNode() const;
    bool isMoveNode() const;
    MoveNode& moveNode();
    const MoveNode& moveNode() const;
    std::string dotString() const;
    std::string toString() const;
    
protected:
    friend class ProgramDependenceGraph;
    void setPredicateMoveNode();
    
private:
    MoveNode* mNode_;
    ControlDependenceNode* cdgNode_;
    bool region_;
    bool predicate_;
};

#endif
