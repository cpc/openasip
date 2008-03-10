/**
 * @file ProgramGraph.hh
 *
 * Declaration of ProgramGraph class.
 *
 * ProgramGraph represents a program in PDG form.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi) 
 * @note rating: red
 */

#ifndef TTA_PROGRAM_GRAPH_HH
#define TTA_PROGRAM_GRAPH_HH

#include <string>
#include <vector>
#include "Exception.hh"
#include "Move.hh"
#include "Program.hh"
#include "MoveNode.hh"
#include "ControlFlowGraph.hh"
#include "ControlDependenceGraph.hh"
#include "DataDependenceGraph.hh"
#include "ProgramDependenceGraph.hh"


namespace TTAProgram{
    class Move;
    class Program;
}

/*
 * ProgramGraph represents program
 */
class ProgramGraph {
public:
    
    ProgramGraph(TTAProgram::Program& program);
    virtual ~ProgramGraph();
    
    TTAProgram::Program& generateProgram() const;
    MoveNode& nodeOf(const TTAProgram::Move&) const;
    int graphCount() const;
    ProgramDependenceGraph* graphAt(int);
    ProgramDependenceGraph* graph(const std::string);
    
private:
    /// Copying forbidden
    ProgramGraph(const ProgramGraph&);
    /// Assignment forbidden
    ProgramGraph& operator=(const ProgramGraph&);

    /// Original program
    TTAProgram::Program& program_;
    
    /// Vector of CFG's for each procedure
    std::vector<ControlFlowGraph*> cfgs_;
    /// Vector of CDG's for each procedure
    std::vector<ControlDependenceGraph*> cdgs_;
    /// Vector of DDG's for each procedure
    std::vector<DataDependenceGraph*> ddgs_;    
    /// Vector of PDG's for each procedure
    std::vector<ProgramDependenceGraph*> pdgs_;
};

#endif
