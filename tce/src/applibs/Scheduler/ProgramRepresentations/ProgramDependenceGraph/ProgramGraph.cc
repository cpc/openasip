/**
 * @file ProgramGraph.cc
 *
 * Implementation of ProgramGraph class.
 *
 * ProgramGraph represents a program in PDG form.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ProgramGraph.hh"
#include "Port.hh"
#include "Bus.hh"
#include "UniversalMachine.hh"
#include "NullMove.hh"
#include "NullProgram.hh"
#include "ControlFlowGraph.hh"
#include "ControlDependenceGraph.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "ProgramDependenceGraph.hh"

/**
 * Constructor.
 *
 * Creates a new ProgramGraph from Program
 * @param program Program in POM
 */

ProgramGraph::ProgramGraph(TTAProgram::Program& program) : program_(program){
    for (int i = 0; i < program.procedureCount(); i++) {
        ControlFlowGraph* cfg = NULL;
        cfg = new ControlFlowGraph(program.procedure(i));    
        cfgs_.push_back(cfg);
        ControlDependenceGraph* cdg = NULL;
        cdg = new ControlDependenceGraph(*cfg);
        cdgs_.push_back(cdg);
        DataDependenceGraphBuilder builder;
        DataDependenceGraph* ddg = NULL;
        ddg = builder.build(*cfg);
        ddgs_.push_back(ddg);   
        ProgramDependenceGraph* pdg = NULL;
        pdg = new ProgramDependenceGraph(*cdg, *ddg);        
        pdgs_.push_back(pdg);
    }
}

/**
 * Destructor.
 *
 * Does not do anything.
 */
ProgramGraph::~ProgramGraph() {
}

/**
 * Convert Program Graph to Program Object Model
 * and returns POM
 * @return POM
 */
TTAProgram::Program&
ProgramGraph::generateProgram() const {
    ///TODO: convert PDG to POM
    /// this is basically scheduling
    return TTAProgram::NullProgram::instance();
}

/**
 * Returns MoveNode of program representation corresponding
 * to Move of POM.
 * @param move move from POM
 * @return MoveNode corresponding to move
 */
MoveNode&
ProgramGraph::nodeOf(const TTAProgram::Move&) const{
    ///TODO: find a move and return corresponding MoveNode
    return *(new MoveNode(TTAProgram::NullMove::instance()));
}

/**
 * Returns number of PDG's in a program graph, equals number of procedures
 * in program.
 * @return number of PDG's in a graph
 */
int 
ProgramGraph::graphCount() const {
    return pdgs_.size();
}

/**
 * Returns a PDG for procedure identified by index.
 * @param i index of a procedure
 * @return PDG for given procedure
 */
ProgramDependenceGraph* 
ProgramGraph::graphAt(int i) {
    if (i < 0 || i >= graphCount()){
        throw InvalidData(__FILE__, __LINE__, __func__, "Trying to access"
            " graph out of a scope!");
    }
    return pdgs_.at(i);
}

/**
 * Returns a PDG for procedure identified by procedure name.
 * @param name name of a procedure
 * @return PDG for given procedure
 */
ProgramDependenceGraph* 
ProgramGraph::graph(const std::string name) {
    for (int i = 0; i < graphCount(); i++) {
        if (pdgs_.at(i)->name() == name) {
            return pdgs_.at(i);
        }
    }
    throw InvalidData(__FILE__, __LINE__, __func__, "Can not find graph of "
        "procedure " + name);
}
