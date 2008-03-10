/**
 * @file ProgramPass.hh
 *
 * Declaration of ProgramPass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_PASS_HH
#define TTA_PROGRAM_PASS_HH

#include "Exception.hh"
#include "SchedulerPass.hh"

class ProcedurePass;

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

/**
 * Interface for scheduler passes that handle fully-linked programs.
 */
class ProgramPass : public SchedulerPass {
public:
    ProgramPass(InterPassData& data);
    virtual ~ProgramPass();

    virtual void handleProgram(
        TTAProgram::Program& program,
        const TTAMachine::Machine& targetMachine)
        throw (Exception) = 0;

    static void executeProcedurePass(
        TTAProgram::Program& program,
        const TTAMachine::Machine& targetMachine,
        ProcedurePass& procedurePass)
        throw (Exception);
};
#endif
