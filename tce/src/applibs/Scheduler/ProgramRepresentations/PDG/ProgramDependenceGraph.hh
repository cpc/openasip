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
 * @file ProgramDependenceGraph.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_DEPENDENCE_GRAPH_HH
#define TTA_PROGRAM_DEPENDENCE_GRAPH_HH

#include "BoostGraph.hh"
#include "ProgramDependenceEdge.hh"
#include "ProgramDependenceNode.hh"
#include "ControlDependenceGraph.hh"

class DataDependenceGraph;

class ProgramDependenceGraph :
    public BoostGraph<ProgramDependenceNode, ProgramDependenceEdge> {

public:
    ProgramDependenceGraph(
        ControlDependenceGraph& cdg,
        DataDependenceGraph& ddg);
    virtual ~ProgramDependenceGraph();
    
private:
    typedef std::map<ControlDependenceNode*, ProgramDependenceNode*,
                     ControlDependenceNode::Comparator> 
        ControlToProgram; 
        
    void removeGuardedJump(
        ControlToProgram&,
        ProgramDependenceNode&, 
        ControlDependenceNode&);
    
    ControlDependenceGraph* cdg_;
    /*DataDependenceGraph* ddg_;*/
    
};

#endif
