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
 * @file Proxim.cc
 *
 * Implementation of Proxim class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <cstdlib>
#include <iostream>
#include <wx/cmdline.h>
#include <wx/imagpng.h>
#include "Proxim.hh"
#include "Application.hh"
#include "Exception.hh"
#include "SimulatorInterpreter.hh"
#include "ProximMainFrame.hh"
#include "ProximSimulationThread.hh"
#include "GUIOptions.hh"
#include "Environment.hh"
#include "GUIOptionsSerializer.hh"
#include "ProximConstants.hh"
#include "FileSystem.hh"
#include "WxConversion.hh"
#include "ProximToolbox.hh"
#include "ErrorDialog.hh"
#include "config.h"

IMPLEMENT_APP(Proxim)

using std::cerr;
using std::endl;
using std::string;

const std::string Proxim::CONFIG_FILE_NAME = "Proxim.conf";
const std::string Proxim::SCHEMA_FILE_NAME = "confschema.xsd";

/**
 * The constructor.
 */
Proxim::Proxim():
    wxApp(), mainFrame_(NULL), commandRegistry_(NULL), simulation_(NULL),
    options_(NULL) {
}


/**
 * The Destructor.
 */
Proxim::~Proxim() {
    delete commandRegistry_;
}



/**
 * Initializes the application.
 *
 * Creates command registry, application main frame and a simulation thread.
 * The main frame console linereader is set as the linereader for the
 * simulator thread.
 *
 * @return True, if the application was succesfully initialized.
 */
bool
Proxim::OnInit() {

    const wxCmdLineEntryDesc cmdLineDesc[] = {
        { wxCMD_LINE_PARAM,  NULL, NULL, _T("input file"),
          wxCMD_LINE_VAL_STRING,
          (wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL) },
        { wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0 }
    };

    wxCmdLineParser parser(cmdLineDesc);
    parser.SetCmdLine(argc, argv);
    parser.Parse();

    std::string machineToLoad = "";
    std::string programToLoad = "";

    if (parser.GetParamCount() == 1) {
        programToLoad = WxConversion::toString(parser.GetParam(0));
    } else if (parser.GetParamCount() == 2) {
        machineToLoad = WxConversion::toString(parser.GetParam(0));
        programToLoad = WxConversion::toString(parser.GetParam(1));
    } else if (parser.GetParamCount() > 2) {
        std::cout << ProximConstants::PROXIM_TITLE << " " 
                  << Application::TCEVersionString() << std::endl;
        std::cout << "Usage: proxim [program file]" << endl;
        std::cout << "       proxim [machine file] [program file]" << endl;
        
        return false;
    }


    // load image handler for pngs
    wxImage::AddHandler(new wxPNGHandler);

    commandRegistry_ = new CommandRegistry();
    simulation_ = new ProximSimulationThread();
    options_ = new GUIOptions("proxim-configuration");

    loadOptions();

    mainFrame_ = new ProximMainFrame(
	_T("TTA Processor Simulator"), wxDefaultPosition, wxSize(800, 600));

    simulation_->Create();
    simulation_->initialize(mainFrame_);
    simulation_->Run();

    mainFrame_->Show(true);
    SetTopWindow(mainFrame_);

    SimulatorInterpreter* interpreter = simulation_->interpreter();

    if (machineToLoad != "") {
        interpreter->interpret(
            ProximConstants::SCL_LOAD_MACHINE + " " + machineToLoad);
        if (interpreter->result().size() > 0) {
            wxString message = _T("Error loading machine file '");
            message.Append(WxConversion::toWxString(machineToLoad));
            message.Append(_T("'.\n"));
            message.Append(WxConversion::toWxString(interpreter->result()));
            ErrorDialog dialog(mainFrame_, message);
            dialog.ShowModal();
            return true;
        }
    }

    if (programToLoad != "") {
        interpreter->interpret(
            ProximConstants::SCL_LOAD_PROGRAM + " " + programToLoad);
        if (interpreter->result().size() > 0) {
            wxString message = _T("Error loading program file '");
            message.Append(WxConversion::toWxString(programToLoad));
            message.Append(_T("'.\n"));
            message.Append(WxConversion::toWxString(interpreter->result()));
            ErrorDialog dialog(mainFrame_, message);
            dialog.ShowModal();
        }
    }
    return true;
}


/**
 * Deletes the simulation thread when the application exits.
 *
 * @return Exit code.
 */
int
Proxim::OnExit() {
    simulation_->Delete();
    simulation_->Wait();
    delete simulation_;
    return EXIT_SUCCESS;
}


/**
 * Returns pointer to the simulation thread.
 *
 * @return Pointer to the simulation thread.
 */
ProximSimulationThread*
Proxim::simulation() {
    return simulation_;
}


/**
 * Returns reference to the application command registry.
 *
 * The command registry stores all commands available for execution
 * in the main fram menubar and toolbar.
 *
 * @return Reference to the application command registry.
 */
CommandRegistry&
Proxim::commandRegistry() {
    return *commandRegistry_;
}

/**
 * Loads GUIOptions for Proxim from the configuration file.
 *
 * If the configuration file is erroneous or not found, default options
 * will be used.
 */
void
Proxim::loadOptions() {

    if (options_ != NULL) {
        delete options_;
    }

    bool optionsOK = false;

    string configFile = Environment::confDirPath("") +
        FileSystem::DIRECTORY_SEPARATOR + CONFIG_FILE_NAME;

    string schemaFile = Environment::dataDirPath("Proxim") +
        FileSystem::DIRECTORY_SEPARATOR + SCHEMA_FILE_NAME;

    GUIOptionsSerializer reader(ProximConstants::CONFIGURATION_NAME);
    reader.setSourceFile(configFile);
    reader.setSchemaFile(schemaFile);
    reader.setUseSchema(true);

    try {
        ObjectState* optionsState = reader.readState();
        options_ = new GUIOptions(optionsState);
        delete optionsState;
        options_->validate();
        options_->setFileName(configFile);
        optionsOK = true;
    } catch (Exception& e) {
        cerr << "Error loading config file " << configFile << ":" << endl
             << e.errorMessage() << endl
             << "Default options will be used." << endl;
    }

    if (!optionsOK) {
        createDefaultOptions();
    } else {
        options_->clearModified();
    }
}

/**
 * Creates a default set of options if the configuration file is erroneous
 * or not found.
 */
void
Proxim::createDefaultOptions() {

    ToolbarButton* buttonMachine = new ToolbarButton(
        0, ProximConstants::COMMAND_NAME_OPEN_MACHINE);

    ToolbarButton* buttonProgram = new ToolbarButton(
        1, ProximConstants::COMMAND_NAME_OPEN_PROGRAM);

    ToolbarButton* buttonRun = new ToolbarButton(
        3, ProximConstants::COMMAND_NAME_RUN);

    ToolbarButton* buttonResume = new ToolbarButton(
        4, ProximConstants::COMMAND_NAME_RESUME);

    ToolbarButton* buttonKill = new ToolbarButton(
        5, ProximConstants::COMMAND_NAME_KILL);

    ToolbarButton* buttonStepi = new ToolbarButton(
        6, ProximConstants::COMMAND_NAME_STEPI);

    ToolbarButton* buttonNexti = new ToolbarButton(
        7, ProximConstants::COMMAND_NAME_NEXTI);

    // F2 -> run
    KeyboardShortcut* scRun = new KeyboardShortcut(
        ProximConstants::COMMAND_NAME_RUN, 2, false, false, 0);

    // F4 -> kill
    KeyboardShortcut* scKill = new KeyboardShortcut(
        ProximConstants::COMMAND_NAME_KILL, 4, false, false, 0);

    // F5 -> stepi
    KeyboardShortcut* scStepi = new KeyboardShortcut(
        ProximConstants::COMMAND_NAME_STEPI, 5, false, false, 0);

    // F6 -> nexti
    KeyboardShortcut* scNexti = new KeyboardShortcut(
        ProximConstants::COMMAND_NAME_NEXTI, 6, false, false, 0);

    // F9 -> resume
    KeyboardShortcut* scResume = new KeyboardShortcut(
        ProximConstants::COMMAND_NAME_RESUME, 9, false, false, 0);

    options_ = new GUIOptions(ProximConstants::CONFIGURATION_NAME);
    string fileName = Environment::confDirPath("") +
        FileSystem::DIRECTORY_SEPARATOR + CONFIG_FILE_NAME;

    options_->setFileName(fileName);

    options_->addToolbarButton(buttonMachine);
    options_->addToolbarButton(buttonProgram);
    options_->addToolbarButton(buttonRun);
    options_->addToolbarButton(buttonResume);
    options_->addToolbarButton(buttonKill);
    options_->addToolbarButton(buttonStepi);
    options_->addToolbarButton(buttonNexti);
    options_->addSeparator(2);
    options_->setToolbarLayout(GUIOptions::TEXT);
    options_->setToolbarVisibility(true);

    options_->addKeyboardShortcut(scRun);
    options_->addKeyboardShortcut(scStepi);
    options_->addKeyboardShortcut(scNexti);
    options_->addKeyboardShortcut(scResume);
    options_->addKeyboardShortcut(scKill);
}

/**
 * Returns references to the application options.
 *
 * @return GUIOptions of the application.
 */
GUIOptions&
Proxim::options() {
    return *options_;
}
