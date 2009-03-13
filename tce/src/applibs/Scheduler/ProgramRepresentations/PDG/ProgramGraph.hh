/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ProgramGraph.hh
 *
 * Declaration of ProgramGraph class.
 *
 * ProgramGraph represents a program in PDG form.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi) 
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
