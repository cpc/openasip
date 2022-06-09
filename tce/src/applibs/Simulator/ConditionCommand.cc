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
 * @file ConditionCommand.cc
 *
 * Implementation of ConditionCommand class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ConditionCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"
#include "TclConditionScript.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
ConditionCommand::ConditionCommand() : 
    SimControlLanguageCommand("condition") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
ConditionCommand::~ConditionCommand() {
}

/**
 * Executes the "condition" command.
 *
 * Modifies conditions of breakpoints.
 *
 * @param arguments The handle of the breakpoint of which condition to modify.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool
ConditionCommand::execute(const std::vector<DataObject>& arguments) {
    if (!checkArgumentCount(arguments.size() - 1, 1, 1)) {
        return false;
    } 

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    if (arguments.size() == 2) {
        if  (!checkIntegerArgument(arguments[1])) {
            return false;
        }
        const int breakpointHandle = arguments[1].integerValue();
        try {
            StopPointManager& stopPointManager =
                simulatorFrontend().stopPointManager();
            stopPointManager.stopPointWithHandleConst(breakpointHandle);

            TclConditionScript condition(NULL, "");
            if (!askConditionFromUser(condition)) {
                return false;
            }
            assert(condition.script().size() > 0);
            if (condition.script().at(0) == "1") {
                stopPointManager.removeCondition(breakpointHandle);
            } else {
                stopPointManager.setCondition(breakpointHandle, condition);
            }
            printBreakpointInfo(breakpointHandle);
        } catch (const InstanceNotFound&) {
            interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_BREAKPOINT_NOT_FOUND).str());
            return false;
        }
        

    }
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
ConditionCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_CONDITION).str();
}
