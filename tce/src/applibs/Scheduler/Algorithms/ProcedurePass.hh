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
 * @file ProcedurePass.hh
 *
 * Declaration of ProcedurePass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCEDURE_PASS_HH
#define TTA_PROCEDURE_PASS_HH

#include "Exception.hh"
#include "SchedulerPass.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Procedure;
}

class ControlFlowGraph;
class ControlFlowGraphPass;

/**
 * Interface for scheduler passes that handle procedures.
 */
class ProcedurePass : public SchedulerPass {
public:
    ProcedurePass(InterPassData& data);
    virtual ~ProcedurePass();

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine);

    static void copyCfgToProcedure(
        TTAProgram::Procedure& procedure, ControlFlowGraph& cfg);

    static void executeControlFlowGraphPass(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetmachine, ControlFlowGraphPass& cfgp);
};
#endif
