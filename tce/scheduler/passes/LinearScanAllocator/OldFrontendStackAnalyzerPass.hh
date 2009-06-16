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
 * @file OldFronendStackAnalyzerPass.hh
 *
 * Declaration of OldFrontendStackAnalyzer class.
 *
 * This class inserts the annotations to the stack frame initialization
 * and deinitialization code so that it can easily be 
 * removed and recreated later.
 * 
 * @author Heikki Kultala 2006-2007?
 * @note rating: red
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
