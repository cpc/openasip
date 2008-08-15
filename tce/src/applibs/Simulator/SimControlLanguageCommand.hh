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
 * @file SimControlLanguageCommand.hh
 *
 * Declaration of SimControlLanguageCommand class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
