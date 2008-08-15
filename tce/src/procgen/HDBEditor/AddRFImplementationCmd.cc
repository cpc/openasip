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
 * @file AddRFImplementationCmd.cc
 *
 * Implementation of AddRFImplementationCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddRFImplementationCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBBrowserWindow.hh"
#include "HDBManager.hh"
#include "RFImplementationDialog.hh"
#include "RFImplementation.hh"
#include "RFEntry.hh"
#include "ErrorDialog.hh"


/**
 * The Constructor.
 */
AddRFImplementationCmd::AddRFImplementationCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_RF_IMPLEMENTATION, NULL) {
}


/**
 * The Destructor.
 */
AddRFImplementationCmd::~AddRFImplementationCmd() {
}

/**
 * Executes the command.
 */
bool
AddRFImplementationCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    HDB::RFImplementation* implementation
        = new HDB::RFImplementation("", "", "","", "", "", "");

    HDB::RFEntry* entry = NULL;

    if (browser->isRFEntrySelected()) {
        entry = manager->rfByEntryID(browser->selectedRFEntry());
    } else if (browser->isRFArchitectureSelected()) {
        int id = manager->addRFEntry();
        manager->setArchitectureForRF(id, browser->selectedRFArchitecture());
        entry = manager->rfByEntryID(id);
    } else {
        return false;
    }

    RFImplementationDialog dialog(parentWindow(), -1, *implementation);

    if (dialog.ShowModal() == wxID_OK) {
        try {
            int id =
                manager->addRFImplementation(*implementation, entry->id());
            wxGetApp().mainFrame().update();
            wxGetApp().mainFrame().browser()->selectRFImplementation(id);
        } catch (Exception& e) {
            wxString message = _T("Error:\n");
            message.Append(WxConversion::toWxString(e.errorMessage()));
            message.Append(_T("\n"));
            message.Append(WxConversion::toWxString(e.lineNum()));
             message.Append(_T(": "));
            message.Append(WxConversion::toWxString(e.fileName()));
            ErrorDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            delete entry;
            delete implementation;
            return false;
        }
    } else {
        delete implementation;
    }

    delete entry;
    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddRFImplementationCmd::icon() const {
    return "";
}

/**
 * Returns the command id.
 *
 * @return Command identifier.
 */
int
AddRFImplementationCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_RF_IMPLEMENTATION;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFImplementationCmd*
AddRFImplementationCmd::create() const {
    return new AddRFImplementationCmd();
}


/**
 * Returns true if the command should be enabled in the tool/menubar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddRFImplementationCmd::isEnabled() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();


    if (browser->isRFArchitectureSelected()) {
        return true;
    } else if (browser->isRFEntrySelected()) {
        HDB::RFEntry* entry = manager->rfByEntryID(browser->selectedRFEntry());
        if (entry->hasArchitecture() && !entry->hasImplementation()) {
            delete entry;
            return true;
        }
        delete entry;
    }

    return false;
}
