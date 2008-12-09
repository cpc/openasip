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
 * @file BPCommand.cc
 *
 * Implementation of BPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "BPCommand.hh"
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
BPCommand::BPCommand() : SimControlLanguageCommand("bp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
BPCommand::~BPCommand() {
}

/**
 * Executes the "bp" command.
 *
 * A breakpoint stops the simulation whenever the Simulator reaches 
 * a certain point in the program. It is possible to add a condition to 
 * a breakpoint, to control when the Simulator must stop with increased 
 * precision. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
BPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        
        return false;
    }

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
BPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_BREAK).str();
}
