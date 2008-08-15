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

SCHEDULER_PASS(OldFrontendStackAnalyzerPass)

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

/**
 * A short description of the module, usually the module name,
 * in this case "OldFrontendStackAnalyzerPass".
 *
 * @return The description as a string.
 */   
std::string
OldFrontendStackAnalyzerPass::shortDescription() const {
    return "Startable: OldFrontendStackAnalyzerPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
OldFrontendStackAnalyzerPass::longDescription() const {
    std::string answer = "Startable: OldFrontendStackAnalyzerPass.";
    answer += " Inserts the annotations to the stack frame initialization\
        and deinitialization code so that it can easily be \
        removed and recreated later.";
    return answer;
}
