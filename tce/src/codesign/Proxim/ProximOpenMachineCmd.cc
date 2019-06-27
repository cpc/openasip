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
 * @file ProximOpenMachineCmd.cc
 *
 * Implementation of ProximOpenMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/filedlg.h>
#include "ProximOpenMachineCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "FileSystem.hh"
#include "ProximSimulationThread.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxOPEN wxFD_OPEN
#endif

/**
 * The Constructor.
 */
ProximOpenMachineCmd::ProximOpenMachineCmd():
    GUICommand(ProximConstants::COMMAND_NAME_OPEN_MACHINE, NULL) {

    }

/**
 * The Destructor.
 */
ProximOpenMachineCmd::~ProximOpenMachineCmd() {
}


/**
 * Executes the command.
 */
bool
ProximOpenMachineCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Architecture Definition Files (*.adf)|*.adf");
    wildcard.Append(_T("|Processor Configuration Files (*.pcf)|*.pcf"));
    wildcard.Append(_T("|All files|*.*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());
    std::string extension = FileSystem::fileExtension(file);

    // Simulator interpreter command is chose according to file extension.
    // Processor configuration files (.pcf), have to be opened using
    // 'conf' command.
    if (extension == ".pcf") {
	command = ProximConstants::SCL_LOAD_CONF + " \"" + file + "\"";
    } else {
	command = ProximConstants::SCL_LOAD_MACHINE + " \"" + file + "\"";
    }

    wxGetApp().simulation()->lineReader().input(command);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximOpenMachineCmd::icon() const {
    return "open_machine.png";
}


/**
 * Returns ID of this command.
 */
int
ProximOpenMachineCmd::id() const {
    return ProximConstants::COMMAND_OPEN_MACHINE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximOpenMachineCmd*
ProximOpenMachineCmd::create() const {
    return new ProximOpenMachineCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximOpenMachineCmd::isEnabled() {
    return true;
}

/**
 * Returns shortened name of the command for toolbar button text.
 *
 * @return Short version of the command name.
 */
std::string
ProximOpenMachineCmd::shortName() const {
    return "Machine";
}
