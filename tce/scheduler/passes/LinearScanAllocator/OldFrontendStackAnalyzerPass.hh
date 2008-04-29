/**
 * Declaration of OldFrontendStackAnalyzer class.
 *
 * This class inserts the annotations to the stack frame initialization
 * and deinitialization core so that it can easily be 
 * removed and recreated later.
 *
 */

#ifndef TTA_OLD_FRONTEND_STACK_ANALYZER_PASS_HH
#define TTA_OLD_FRONTEND_STACK_ANALYZER_PASS_HH

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

class OldFrontendStackAnalyzerPass : public StartableSchedulerModule {
public:
    OldFrontendStackAnalyzerPass();
    
    virtual ~OldFrontendStackAnalyzerPass();
    void start() 
        throw (Exception);
    bool needsProgram() const;
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:

};


#endif
