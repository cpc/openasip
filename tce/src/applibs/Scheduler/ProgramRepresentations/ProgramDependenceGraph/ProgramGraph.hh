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
