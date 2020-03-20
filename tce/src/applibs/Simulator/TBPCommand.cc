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
 * @file TBPCommand.cc
 *
 * Implementation of TBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "TBPCommand.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
TBPCommand::TBPCommand() : SimControlLanguageCommand("tbp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
TBPCommand::~TBPCommand() {
}

/**
 * Executes the "tbp" command.
 *
 * Sets a temporary breakpoint, which is automatically deleted after 
 * the first time it stops the simulation. The arguments args are the 
 * same as for the bp command. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool
TBPCommand::execute(const std::vector<DataObject>& arguments) {
    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        return false;
    }
    breakpoint.setDeletedAfterTriggered(true);

    StopPointManager& stopPointManager = 
        simulatorFrontend().stopPointManager();
    return printBreakpointInfo(stopPointManager.add(breakpoint));
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
TBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_TBREAK).str();
}
