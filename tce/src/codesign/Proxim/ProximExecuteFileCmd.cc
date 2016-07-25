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
 * @file ProximExecuteFileCmd.cc
 *
 * Implementation of ProximExecuteFileCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <fstream>
#include "ProximExecuteFileCmd.hh"
#include "ProximConstants.hh"
#include "ProximToolbox.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "TracedSimulatorFrontend.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxOPEN wxFD_OPEN
#endif

/**
 * The Constructor.
 */
ProximExecuteFileCmd::ProximExecuteFileCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EXECUTE_FILE, NULL) {

    simulator_ = ProximToolbox::frontend();
}

/**
 * The Destructor.
 */
ProximExecuteFileCmd::~ProximExecuteFileCmd() {
}


/**
 * Executes the command.
 */
bool
ProximExecuteFileCmd::Do() {
    wxString message = _T("Choose a file.");
    wxString wildcard = _T("Command history logs (*.log)|*.log|All files|*.*");
    wxFileDialog dialog(
        parentWindow(), message, _T(""), _T(""), wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return false;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());
    std::ifstream file(filename.c_str());

    if (file.bad()) {
        wxString message = _T("Error opening file '");
        message.Append(dialog.GetPath());
        message.Append(_T("'."));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    ProximLineReader& lineReader = ProximToolbox::lineReader();

    while (!file.eof()) {
        std::string command;
        getline(file, command);
        lineReader.input(command);
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximExecuteFileCmd::icon() const {
    return "exec.png";
}


/**
 * Returns ID of this command.
 */
int
ProximExecuteFileCmd::id() const {
    return ProximConstants::COMMAND_EXECUTE_FILE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximExecuteFileCmd*
ProximExecuteFileCmd::create() const {
    return new ProximExecuteFileCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximExecuteFileCmd::isEnabled() {

    if (simulator_ != NULL) {
	return true;
    } else {
	return false;
    }
}
