/**
 * Declaration of OldFrontendStackAnalyzer class.
 *
 * This class inserts the annotations to the stack frame initialization
 * and deinitialization core so that it can easily be 
 * removed and recreated later.
 *
 */

#ifndef TTA_OLD_FRONTEND_STACK_ANALYZER_HH
#define TTA_OLD_FRONTEND_STACK_ANALYZER_HH

#include <string>

#include "ProgramAnnotation.hh"
#include "StartableSchedulerModule.hh"

#include "Exception.hh"

#include "StackFrameData.hh"

namespace TTAProgram {
    class Move;
    class Instruction;
    class Procedure;
    class Terminal;
    class TerminalRegister;
    class ProgramAnnotation;
}

class OldFrontendStackAnalyzer {
public:
    OldFrontendStackAnalyzer();
    virtual ~OldFrontendStackAnalyzer();

    void analyzeAndAnnotateProcedure(TTAProgram::Procedure& proc) 
        throw (IllegalProgram);

private:

    StackFrameData* readCrt0Header(TTAProgram::Procedure& proc)
        throw (IllegalProgram);

    StackFrameData* readCommonProcedureHeader(TTAProgram::Procedure& proc)
        throw (IllegalProgram);

    StackFrameData* readProcedureHeader(TTAProgram::Procedure& proc)
        throw (IllegalProgram);
    
    void annotateStackOffsets(
        TTAProgram::Procedure& proc, const StackFrameData& sfd) 
        throw (IllegalProgram);

    void annotateStackUsage(
        TTAProgram::Move& move, const StackFrameData& sfd);

    void annotateInstruction(
        TTAProgram::Instruction&ins, TTAProgram::ProgramAnnotation::Id id, 
        const std::string& payload) throw (IllegalProgram);

    bool isStoreAddress(TTAProgram::Terminal& terminal);

    TTAProgram::Terminal* stackPointer_;

};


#endif
