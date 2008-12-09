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
 * @file SimControlLanguageCommand.hh
 *
 * Declaration of SimControlLanguageCommand class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMCONLANG_COMMAND
#define TTA_SIMCONLANG_COMMAND

#include <string>
#include <vector>
#include <iostream>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
#include "Address.hh"

class SimulatorFrontend;
class Breakpoint;
class TclConditionScript;
class ExpressionScript;

///////////////////////////////////////////////////////////////////////////////
// SimControlLanguageCommand
///////////////////////////////////////////////////////////////////////////////

/**
 * This is a base class for Simulator Control Language commands.
 *
 * Provides services command and useful for all Simulator Control Language
 * commands.
 */
class SimControlLanguageCommand : public CustomCommand {
public:
    SimControlLanguageCommand(const std::string& name);
    virtual ~SimControlLanguageCommand();

    SimulatorFrontend& simulatorFrontend();
    const SimulatorFrontend& simulatorFrontendConst();
    virtual void printNextInstruction();
    virtual void printStopInformation();
    virtual void printStopReasons();
    virtual bool printBreakpointInfo(unsigned int breakpointHandle);
    virtual void printSimulationTime();

    virtual std::ostream& outputStream();

    bool checkSimulationInitialized();
    bool checkSimulationNotAlreadyRunning();
    bool checkSimulationStopped();
    bool checkSimulationEnded();
    bool checkParallelSimulation();
    bool checkProgramLoaded();

    InstructionAddress parseInstructionAddressExpression(
        const std::string& expression)
        throw (IllegalParameters);    

    TTAProgram::Address parseDataAddressExpression(
        const std::string& expression)
        throw (IllegalParameters);    

    bool parseBreakpoint(
        const std::vector<DataObject>& arguments, 
        Breakpoint& target);

    bool askConditionFromUser(TclConditionScript& target);
    bool askExpressionFromUser(ExpressionScript& target);

    bool verifyBreakpointHandles(
        const std::vector<DataObject>& arguments, 
        std::size_t startIndex = 1);
};

///////////////////////////////////////////////////////////////////////////////
// SimControlLanguageSubCommand
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for classes that implement subcommands of Simulator Control 
 * Language.
 */
class SimControlLanguageSubCommand {
public:
    SimControlLanguageSubCommand(SimControlLanguageCommand& parentCommand);
    virtual ~SimControlLanguageSubCommand();
    virtual bool execute(const std::vector<DataObject>& arguments) = 0;
    virtual SimControlLanguageCommand& parent();
private:
    /// the main command
    SimControlLanguageCommand& parentCommand_;    
};


#endif
