/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
