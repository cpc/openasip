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
 * @file MachCommand.cc
 *
 * Implementation of MachCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MachCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
MachCommand::MachCommand() : CustomCommand("mach") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
MachCommand::~MachCommand() {
}

/**
 * Executes the "mach" command.
 *
 * Loads a new machine file using the SimulatorFrontend.
 *
 * @param arguments Filename is given as argument to the command.
 * @return True if loading the program was successful.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
MachCommand::execute(const std::vector<DataObject>& arguments)
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
        simulatorFrontend.loadMachine(fileName);
    } catch (const Exception& e) {
        DataObject* errorMessage = new DataObject();
        errorMessage->setString(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_UNABLE_TO_LOAD_MACHINE).str() + e.errorMessageStack(true));
        interpreter()->setResult(errorMessage);
        return false;
    } catch (...) {
        assert(false);
        return false;
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
MachCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_MACH).str();
}

