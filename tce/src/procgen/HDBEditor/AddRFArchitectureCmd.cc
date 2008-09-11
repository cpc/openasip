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
 * @file AddRFArchitectureCmd.cc
 *
 * Implementation of AddRFArchitectureCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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

