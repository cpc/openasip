/**
 * Declaration of OldFrontendStackAnalyzer class.
 *
 * This class inserts the annotations to the stack frame initialization
 * and deinitialization code so that it can easily be 
 * removed and recreated later.
 *
 */

#include "OldFrontendStackAnalyzerPass.hh"
#include "OldFrontendStackAnalyzer.hh"

#include "Program.hh"

using namespace TTAProgram;
using namespace TTAMachine;

SCHEDULER_PASS(OldFrontendStackAnalyzerPass);

OldFrontendStackAnalyzerPass::OldFrontendStackAnalyzerPass() {}

OldFrontendStackAnalyzerPass::~OldFrontendStackAnalyzerPass() {}

void 
OldFrontendStackAnalyzerPass::start() 
    throw (Exception) {

    OldFrontendStackAnalyzer analyzer;

    for (int i = 0; i < program_->procedureCount(); i++) {
        analyzer.analyzeAndAnnotateProcedure(program_->procedure(i));
    }
}

bool
OldFrontendStackAnalyzerPass::needsProgram() const {
    return true;
}
