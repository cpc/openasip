/**
 * @file ProgramDependenceGraph.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_DEPENDENCE_GRAPH_HH
#define TTA_PROGRAM_DEPENDENCE_GRAPH_HH

#include "BoostGraph.hh"
#include "ProgramDependenceEdge.hh"
#include "ProgramDependenceNode.hh"
#include "ControlFlowGraph.hh"
#include "ControlDependenceGraph.hh"
#include "DataDependenceGraph.hh"

class ProgramDependenceGraph :
    public BoostGraph<ProgramDependenceNode, ProgramDependenceEdge> {

public:
    ProgramDependenceGraph(
        ControlDependenceGraph& cdg,
        DataDependenceGraph& ddg);
    virtual ~ProgramDependenceGraph();
    std::string name() const;
    
private:
    typedef std::map<ControlDependenceNode*, ProgramDependenceNode*> 
        ControlToProgram; 
        
    void removeGuardedJump(
        ControlToProgram&,
        ProgramDependenceNode&, 
        ControlDependenceNode&);
    
    std::string procedureName_;
    ControlDependenceGraph* cdg_;
    DataDependenceGraph* ddg_;
    
};

#endif
