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
 * @file ProgramPass.cc
 *
 * Definition of ProgramPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/timer.hpp>
#include <boost/format.hpp>

#include "ProgramPass.hh"
#include "Program.hh"
#include "ProcedurePass.hh"
#include "Application.hh"
#include "Procedure.hh"
#include "POMDisassembler.hh"
#include "InterPassDatum.hh"
#include "InterPassData.hh"

/**
 * Constructor.
 */
ProgramPass::ProgramPass(InterPassData& data) :
    SchedulerPass(data)  {
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
    boost::timer totalTime;

    FunctionNameList proceduresToProcess, proceduresToIgnore;
    if (procedurePass.interPassData().hasDatum("FUNCTIONS_TO_PROCESS")) {
        proceduresToProcess = 
            dynamic_cast<FunctionNameList&>(
                procedurePass.interPassData().datum("FUNCTIONS_TO_PROCESS"));
    } else if (procedurePass.interPassData().hasDatum("FUNCTIONS_TO_IGNORE")) {
        proceduresToIgnore = 
            dynamic_cast<FunctionNameList&>(
                procedurePass.interPassData().datum("FUNCTIONS_TO_IGNORE"));
    }
    
    std::size_t proceduresDone = 0;
    // always call procedureCount() again because a pass might have
    // added a new procedure to the program that needs to be handled
    for (int procIndex = 0; procIndex < program.procedureCount(); 
         ++procIndex) {
        TTAProgram::Procedure& proc = program.procedure(procIndex);
               
        if (proceduresToProcess.size() > 0 && 
            proceduresToProcess.find(proc.name()) == 
            proceduresToProcess.end())
            continue;

        if (proceduresToIgnore.size() > 0 &&
            proceduresToIgnore.find(proc.name()) !=
            proceduresToIgnore.end())
            continue;
            

        boost::timer currentTime;
        std::size_t totalProcedures = 0;
        if (Application::verboseLevel() > 0) {

            if (proceduresToProcess.size() > 0) {
                totalProcedures = proceduresToProcess.size();
            } else {
                totalProcedures = program.procedureCount();
            }

            totalProcedures -= proceduresToIgnore.size();
            Application::logStream() 
                << std::endl
                << "procedure: " << proc.name() 
                << (boost::format(" (%d/%d)") 
                    % (proceduresDone + 1) % totalProcedures).str();
        }
        procedurePass.handleProcedure(proc, targetMachine);
        ++proceduresDone;

        if (Application::verboseLevel() > 0) {
            double cur = currentTime.elapsed();
            double tot = totalTime.elapsed();
            long currentElapsed = static_cast<long>(cur);
            long totalElapsed = static_cast<long>(tot);
            long eta = 
                static_cast<long>(
                    (tot / proceduresDone) * 
                    (totalProcedures - proceduresDone));
            Application::logStream()
                << (boost::format(
                        " %d min %d s. Total %d min %d s. ETA in %d min %d s")
                    % (currentElapsed / 60) % (currentElapsed % 60)
                    % (totalElapsed / 60) % (totalElapsed % 60)
                    % (eta / 60) % (eta % 60)).str()
                << std::endl;
        }
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
