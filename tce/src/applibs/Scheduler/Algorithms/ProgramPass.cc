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
 * @file ProgramPass.cc
 *
 * Definition of ProgramPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProgramPass.hh"
#include "Program.hh"
#include "ProcedurePass.hh"
#include "Application.hh"
#include "Procedure.hh"
#include "POMDisassembler.hh"

/**
 * Constructor.
 */
ProgramPass::ProgramPass(InterPassData& data) : 
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
ProgramPass::~ProgramPass() {
}

/**
 * Executes the given procedure pass on each procedure of the given
 * program in the original program order.
 *
 * A helper function for implementing most simplest types of program passes.
 *
 * @param program The program to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @param procedurePass The procedure pass to execute.
 * @exception In case handling is unsuccesful for any reason (cfg might 
 * still get modified).
 */
void
ProgramPass::executeProcedurePass(
    TTAProgram::Program& program, const TTAMachine::Machine& targetMachine,
    ProcedurePass& procedurePass) {
    for (int procIndex = 0; procIndex < program.procedureCount(); 
         ++procIndex) {
        TTAProgram::Procedure& proc = program.procedure(procIndex);
        procedurePass.handleProcedure(proc, targetMachine);
    }
}

void
ProgramPass::handleProgram(
    TTAProgram::Program& program, const TTAMachine::Machine& targetMachine) {
    ProcedurePass* procPass = dynamic_cast<ProcedurePass*>(this);
    if (procPass != NULL) {
        executeProcedurePass(program, targetMachine, *procPass);
    } else {
        abortWithError("Program pass is not also a procedure pass so you "
                       "must overload handleProgram method!");
    }
}
