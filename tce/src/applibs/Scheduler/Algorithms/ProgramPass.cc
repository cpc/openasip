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
