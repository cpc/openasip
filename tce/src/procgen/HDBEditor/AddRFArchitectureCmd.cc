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
 * @file AddRFArchitectureCmd.cc
 *
 * Implementation of AddRFArchitectureCmd class.
 *
 * @author Veli-Pekka J��skel�inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "AddRFArchitectureCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBManager.hh"
#include "RFArchitectureDialog.hh"
#include "RFArchitecture.hh"
#include "ErrorDialog.hh"
#include "HDBBrowserWindow.hh"

/**
 * The Constructor.
 */
AddRFArchitectureCmd::AddRFArchitectureCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_RF_ARCHITECTURE, NULL) {
}


/**
 * The Destructor.
 */
AddRFArchitectureCmd::~AddRFArchitectureCmd() {
}

/**
 * Executes the command.
 */
bool
AddRFArchitectureCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    HDB::RFArchitecture arch(1, 1, 1, 1, 0, 1, false);

    RFArchitectureDialog dialog(parentWindow(), -1, arch);

    if (dialog.ShowModal() != wxID_OK) {
        return false;
    }

    int id = manager->addRFArchitecture(arch);
    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectRFArchitecture(id);
    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddRFArchitectureCmd::icon() const {
    return "";
}


/**
 * Returns the command identifier.
 *
 * @return Command identifier for this command.
 */
int
AddRFArchitectureCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_RF_ARCHITECTURE;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return A new instance of this command.
 */
AddRFArchitectureCmd*
AddRFArchitectureCmd::create() const {
    return new AddRFArchitectureCmd();
}


/**
 * Returns true if the command should be enabled in menu/toolbar, false if not.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddRFArchitectureCmd::isEnabled() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }
    return true;
}

