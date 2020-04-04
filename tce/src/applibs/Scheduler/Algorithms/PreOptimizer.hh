/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file PreOptimizer.hh
 *
 * Declaration of PreOptimizer class.
 *
 * This optimizer removes unneeded predicate arithmetic by using
 * opposite guard instead where the guard is used.
 *
 * Also optimizes register usage, changes registers of address calculations.
 *
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRE_OPTIMIZER_HH
#define TTA_PRE_OPTIMIZER_HH

#include "ProcedurePass.hh"
#include "ProgramPass.hh"
#include "ControlFlowGraphPass.hh"
#include "DataDependenceGraph.hh"
#include "ControlFlowGraph.hh"

namespace TTAProgram {
    class CodeSnippet;
    class Procedure;
    class Program;
    class InstructionReferenceManager;
    class Instruction;
}

namespace TTAMachine {
    class Machine;
}

class DataDependenceGraph;
class ProgramOperation;

class PreOptimizer:
    public ProcedurePass, public ProgramPass , public ControlFlowGraphPass {

public:
    PreOptimizer(InterPassData& data);

    void handleProgram(
        TTAProgram::Program& program, const TTAMachine::Machine& targetMachine);

    void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine);

    void handleControlFlowGraph(
        ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine);

    void handleCFGDDG(
        ControlFlowGraph& cfg,
        DataDependenceGraph& ddg);

    std::string shortDescription() const { 
        return "optimizes away guard nagation operations, "
            "uses opposite guards instead"; };
private:
    ControlFlowGraph::NodeSet inverseGuardsOfHeads(
        DataDependenceGraph& ddg,
        DataDependenceGraph::EdgeSet& oEdges);

    bool checkGuardReversalAllowed(
        DataDependenceGraph& ddg,
        DataDependenceGraph::EdgeSet& oEdges);

    bool tryToOptimizeAddressReg(
        DataDependenceGraph& ddg, ProgramOperation&po);

    ControlFlowGraph::NodeSet
    tryToRemoveXor(
        DataDependenceGraph& ddg, ProgramOperation& po,
        TTAProgram::InstructionReferenceManager* irm,
        ControlFlowGraph& cfg);

    ControlFlowGraph::NodeSet
    tryToRemoveEq(
        DataDependenceGraph& ddg, ProgramOperation& po,
        TTAProgram::InstructionReferenceManager* irm,
        ControlFlowGraph& cfg);
    
    ControlFlowGraph::NodeSet
    tryToRemoveGuardInversingOp(
        DataDependenceGraph& ddg, ProgramOperation& po,
        TTAProgram::InstructionReferenceManager* irm,
        ControlFlowGraph& cfg);

    bool cfgAllowsJumpReversal(
        TTAProgram::Instruction& ins, ControlFlowGraph& cfg);


};

#endif
