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
 * @file QuitCommand.cc
 *
 * Implementation of QuitCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "QuitCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorInterpreter.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
QuitCommand::QuitCommand(const std::string& commandString) : 
    CustomCommand(commandString) {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
QuitCommand::~QuitCommand() {
}

/**
 * Executes the "quit" command.
 *
 * This method does the uninitialization of the simulator.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
QuitCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 0)) {
        return false;
    } 
    dynamic_cast<SimulatorInterpreter*>(interpreter())->setQuitCommandGiven();
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
QuitCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_QUIT).str();
}

