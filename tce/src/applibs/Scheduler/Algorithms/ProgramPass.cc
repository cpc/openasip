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
 * @file ProgramPass.cc
 *
 * Definition of ProgramPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
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
    TTAProgram::Program& program,
    const TTAMachine::Machine& targetMachine,
    ProcedurePass& procedurePass)
    throw (Exception) {

    for (int procIndex = 0; procIndex < program.procedureCount(); 
         ++procIndex) {
        TTAProgram::Procedure& proc = program.procedure(procIndex);
        procedurePass.handleProcedure(proc, targetMachine);
    }
}
