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
 * @file ProgCommand.cc
 *
 * Implementation of ProgCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProgCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "SimulatorToolbox.hh"
#include "Exception.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
ProgCommand::ProgCommand() : SimControlLanguageCommand("prog") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
ProgCommand::~ProgCommand() {
}

/**
 * Executes the "prog" command.
 *
 * Loads a new program file using the SimulatorFrontend.
 *
 * @param arguments Filename is given as argument to the command.
 * @return True if loading the program was successful.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
ProgCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    assert(interpreter() != NULL);

    SimulatorTextGenerator& textGen = SimulatorToolbox::textGenerator();    
    if (arguments.size() < 2) {
        DataObject* errorMessage = new DataObject();
        errorMessage->setString(
            textGen.text(Texts::TXT_NO_FILENAME_DEFINED).str());
        interpreter()->setResult(errorMessage);
        return false;
    }

    if (arguments.size() > 2) {
        DataObject* errorMessage = new DataObject();
        errorMessage->setString(
            textGen.text(Texts::TXT_ONLY_ONE_FILENAME_EXPECTED).str());
        interpreter()->setResult(errorMessage);
        return false;	
    }

    SimulatorInterpreterContext& interpreterContext = 
        dynamic_cast<SimulatorInterpreterContext&>(interpreter()->context());

    SimulatorFrontend& simulatorFrontend = 
        interpreterContext.simulatorFrontend();

    try {
        const std::string fileName = 
            FileSystem::expandTilde(arguments.at(1).stringValue());	
        simulatorFrontend.loadProgram(fileName);
    } catch (const Exception& e) {
        DataObject* errorMessage = new DataObject();
        errorMessage->setString(e.errorMessage());
        interpreter()->setResult(errorMessage);
        return false;
    } catch (...) {
        assert(false);
        return false;
    }
    
    if (simulatorFrontend.automaticFinishImpossible())
        outputStream() 
            << textGen.text(Texts::TXT_AUTOMATIC_FINISH_IMPOSSIBLE).str()
            << std::endl;

    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
ProgCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
	Texts::TXT_INTERP_HELP_PROG).str();
}

